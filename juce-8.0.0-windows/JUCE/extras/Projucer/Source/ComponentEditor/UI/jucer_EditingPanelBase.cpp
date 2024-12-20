/*
  ==============================================================================

   This file is part of the JUCE framework.
   Copyright (c) Raw Material Software Limited

   JUCE is an open source framework subject to commercial or open source
   licensing.

   By downloading, installing, or using the JUCE framework, or combining the
   JUCE framework with any other source code, object code, content or any other
   copyrightable work, you agree to the terms of the JUCE End User Licence
   Agreement, and all incorporated terms including the JUCE Privacy Policy and
   the JUCE Website Terms of Service, as applicable, which will bind you. If you
   do not agree to the terms of these agreements, we will not license the JUCE
   framework to you, and you must discontinue the installation or download
   process and cease use of the JUCE framework.

   JUCE End User Licence Agreement: https://juce.com/legal/juce-8-licence/
   JUCE Privacy Policy: https://juce.com/juce-privacy-policy
   JUCE Website Terms of Service: https://juce.com/juce-website-terms-of-service/

   Or:

   You may also use this code under the terms of the AGPLv3:
   https://www.gnu.org/licenses/agpl-3.0.en.html

   THE JUCE FRAMEWORK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL
   WARRANTIES, WHETHER EXPRESSED OR IMPLIED, INCLUDING WARRANTY OF
   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

#include "../../Application/jucer_Headers.h"
#include "jucer_EditingPanelBase.h"
#include "jucer_JucerDocumentEditor.h"

//==============================================================================
class EditingPanelBase::MagnifierComponent  : public Component
{
public:
    explicit MagnifierComponent (Component* c) : content (c)
    {
        addAndMakeVisible (content.get());
        updateBounds (content.get());
    }

    void childBoundsChanged (Component* child) override
    {
        updateBounds (child);
    }

    double getScaleFactor() const   { return scaleFactor; }

    void setScaleFactor (double newScale)
    {
        scaleFactor = newScale;
        content->setTransform (AffineTransform::scale ((float) scaleFactor));
    }

private:
    void updateBounds (Component* child)
    {
        auto childArea = getLocalArea (child, child->getLocalBounds());
        setSize (childArea.getWidth(), childArea.getHeight());
    }

    double scaleFactor = 1.0;
    std::unique_ptr<Component> content;
};

//==============================================================================
class ZoomingViewport   : public Viewport
{
public:
    explicit ZoomingViewport (EditingPanelBase* p) : panel (p)
    {
    }

    void mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel) override
    {
        if (e.mods.isCtrlDown() || e.mods.isAltDown() || e.mods.isCommandDown())
            mouseMagnify (e, 1.0f / (1.0f - wheel.deltaY));
        else
            Viewport::mouseWheelMove (e, wheel);
    }

    void mouseMagnify (const MouseEvent& e, float factor) override
    {
        panel->setZoom (panel->getZoom() * factor, e.x, e.y);
    }

    void dragKeyHeldDown (const bool isKeyDown)
    {
        if (isSpaceDown != isKeyDown)
        {
            isSpaceDown = isKeyDown;

            if (isSpaceDown)
            {
                auto dc = new DraggerOverlayComp();
                addAndMakeVisible (dc);
                dc->setBounds (getLocalBounds());
            }
            else
            {
                for (int i = getNumChildComponents(); --i >= 0;)
                    std::unique_ptr<DraggerOverlayComp> deleter (dynamic_cast<DraggerOverlayComp*> (getChildComponent (i)));
            }
        }
    }

private:
    EditingPanelBase* const panel;
    bool isSpaceDown = false;

    //==============================================================================
    class DraggerOverlayComp    : public Component
    {
    public:
        DraggerOverlayComp()
        {
            setMouseCursor (MouseCursor::DraggingHandCursor);
            setAlwaysOnTop (true);
        }

        void mouseDown (const MouseEvent&) override
        {
            if (Viewport* viewport = findParentComponentOfClass<Viewport>())
            {
                startX = viewport->getViewPositionX();
                startY = viewport->getViewPositionY();
            }
        }

        void mouseDrag (const MouseEvent& e) override
        {
            if (Viewport* viewport = findParentComponentOfClass<Viewport>())
                viewport->setViewPosition (jlimit (0, jmax (0, viewport->getViewedComponent()->getWidth() - viewport->getViewWidth()),
                                                   startX - e.getDistanceFromDragStartX()),
                                           jlimit (0, jmax (0, viewport->getViewedComponent()->getHeight() - viewport->getViewHeight()),
                                                   startY - e.getDistanceFromDragStartY()));
        }

    private:
        int startX, startY;
    };
};


//==============================================================================
EditingPanelBase::EditingPanelBase (JucerDocument& doc, Component* props, Component* editorComp)
    : document (doc),
      editor (editorComp),
      propsPanel (props)
{
    addAndMakeVisible (viewport = new ZoomingViewport (this));
    addAndMakeVisible (propsPanel);

    viewport->setViewedComponent (magnifier = new MagnifierComponent (editor));
}

EditingPanelBase::~EditingPanelBase()
{
    deleteAllChildren();
}

void EditingPanelBase::resized()
{
    const int contentW = jmax (1, getWidth() - 260);

    propsPanel->setBounds (contentW + 4, 4, jmax (100, getWidth() - contentW - 8), getHeight() - 8);

    viewport->setBounds (4, 4, contentW - 8, getHeight() - 8);

    if (document.isFixedSize())
        editor->setSize (jmax (document.getInitialWidth(),
                               roundToInt ((viewport->getWidth() - viewport->getScrollBarThickness()) / getZoom())),
                         jmax (document.getInitialHeight(),
                               roundToInt ((viewport->getHeight() - viewport->getScrollBarThickness()) / getZoom())));
    else
        editor->setSize (viewport->getWidth(),
                         viewport->getHeight());
}

void EditingPanelBase::paint (Graphics& g)
{
    g.fillAll (findColour (secondaryBackgroundColourId));
}

void EditingPanelBase::visibilityChanged()
{
    if (isVisible())
    {
        updatePropertiesList();

        if (Component* p = getParentComponent())
        {
            resized();

            if (JucerDocumentEditor* const cdh = dynamic_cast<JucerDocumentEditor*> (p->getParentComponent()))
                cdh->setViewportToLastPos (viewport, *this);

            resized();
        }
    }
    else
    {
        if (Component* p = getParentComponent())
            if (JucerDocumentEditor* const cdh = dynamic_cast<JucerDocumentEditor*> (p->getParentComponent()))
                cdh->storeLastViewportPos (viewport, *this);
    }

    editor->setVisible (isVisible());
}

double EditingPanelBase::getZoom() const
{
    return magnifier->getScaleFactor();
}

void EditingPanelBase::setZoom (double newScale)
{
    setZoom (jlimit (1.0 / 8.0, 16.0, newScale),
             viewport->getWidth() / 2,
             viewport->getHeight() / 2);
}

void EditingPanelBase::setZoom (double newScale, int anchorX, int anchorY)
{
    Point<int> anchor (editor->getLocalPoint (viewport, Point<int> (anchorX, anchorY)));

    magnifier->setScaleFactor (newScale);

    resized();

    jassert (viewport != nullptr);
    anchor = viewport->getLocalPoint (editor, anchor);

    viewport->setViewPosition (jlimit (0, jmax (0, viewport->getViewedComponent()->getWidth() - viewport->getViewWidth()),
                                       viewport->getViewPositionX() + anchor.getX() - anchorX),
                               jlimit (0, jmax (0, viewport->getViewedComponent()->getHeight() - viewport->getViewHeight()),
                                       viewport->getViewPositionY() + anchor.getY() - anchorY));
}

void EditingPanelBase::xyToTargetXY (int& x, int& y) const
{
    Point<int> pos (editor->getLocalPoint (this, Point<int> (x, y)));
    x = pos.getX();
    y = pos.getY();
}

void EditingPanelBase::dragKeyHeldDown (bool isKeyDown)
{
    ((ZoomingViewport*) viewport)->dragKeyHeldDown (isKeyDown);
}
