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
#include "jucer_ResourceEditorPanel.h"

//==============================================================================
class ResourceListButton  : public Component
{
public:
    explicit ResourceListButton (JucerDocument& doc)
        : document (doc), reloadButton ("Reload"), row (0)
    {
        setInterceptsMouseClicks (false, true);
        addAndMakeVisible (reloadButton);
        reloadButton.onClick = [this]
        {
            if (auto* r = document.getResources() [row])
                document.getResources().browseForResource ("Select a file to replace this resource", "*",
                                                           File (r->originalFilename), r->name, nullptr);
        };
    }

    void update (int newRow)
    {
        row = newRow;
        reloadButton.setVisible (document.getResources() [row] != nullptr);
    }

    void resized() override
    {
        reloadButton.setBoundsInset (BorderSize<int> (2));
    }

private:
    JucerDocument& document;
    TextButton reloadButton;
    int row;
};


//==============================================================================
ResourceEditorPanel::ResourceEditorPanel (JucerDocument& doc)
    : document (doc),
      addButton ("Add new resource..."),
      reloadAllButton ("Reload all resources"),
      delButton ("Delete selected resources")
{
    addAndMakeVisible (addButton);
    addButton.onClick = [this]
    {
        document.getResources().browseForResource ("Select a file to add as a resource", "*", {}, {}, nullptr);
    };

    addAndMakeVisible (reloadAllButton);
    reloadAllButton.onClick = [this] { reloadAll(); };

    addAndMakeVisible (delButton);
    delButton.setEnabled (false);
    delButton.onClick = [this] { document.getResources().remove (listBox->getSelectedRow (0)); };

    listBox.reset (new TableListBox (String(), this));
    addAndMakeVisible (listBox.get());
    listBox->getHeader().addColumn ("name", 1, 150, 80, 400);
    listBox->getHeader().addColumn ("original file", 2, 350, 80, 800);
    listBox->getHeader().addColumn ("size", 3, 100, 40, 150);
    listBox->getHeader().addColumn ("reload", 4, 100, 100, 100, TableHeaderComponent::notResizableOrSortable);
    listBox->getHeader().setStretchToFitActive (true);

    listBox->setOutlineThickness (1);
    listBox->updateContent();

    document.addChangeListener (this);
    handleCommandMessage (1);

    updateLookAndFeel();
}

ResourceEditorPanel::~ResourceEditorPanel()
{
    document.removeChangeListener (this);
}

int ResourceEditorPanel::getNumRows()
{
    return document.getResources().size();
}

void ResourceEditorPanel::paintRowBackground (Graphics& g, int /*rowNumber*/,
                                              int /*width*/, int /*height*/, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll (findColour (defaultHighlightColourId));
}

void ResourceEditorPanel::paintCell (Graphics& g, int rowNumber, int columnId, int width, int height,
                                     bool rowIsSelected)
{
    if (const BinaryResources::BinaryResource* const r = document.getResources() [rowNumber])
    {
        String text;

        if (columnId == 1)
            text = r->name;
        else if (columnId == 2)
            text = r->originalFilename;
        else if (columnId == 3)
            text = File::descriptionOfSizeInBytes ((int64) r->data.getSize());

        if (rowIsSelected)
            g.setColour (findColour (defaultHighlightedTextColourId));
        else
            g.setColour (findColour (defaultTextColourId));

        g.setFont (13.0f);
        g.drawText (text, 4, 0, width - 6, height, Justification::centredLeft, true);
    }
}

Component* ResourceEditorPanel::refreshComponentForCell (int rowNumber, int columnId, bool /*isRowSelected*/,
                                                         Component* existingComponentToUpdate)
{
    if (columnId != 4)
        return nullptr;

    if (existingComponentToUpdate == nullptr)
        existingComponentToUpdate = new ResourceListButton (document);

    ((ResourceListButton*) existingComponentToUpdate)->update (rowNumber);

    return existingComponentToUpdate;
}

int ResourceEditorPanel::getColumnAutoSizeWidth (int columnId)
{
    if (columnId == 4)
        return 0;

    Font f (FontOptions { 13.0f });
    int widest = 40;

    for (int i = document.getResources().size(); --i >= 0;)
    {
        const BinaryResources::BinaryResource* const r = document.getResources() [i];
        jassert (r != nullptr);
        String text;

        if (columnId == 1)
            text = r->name;
        else if (columnId == 2)
            text = r->originalFilename;
        else if (columnId == 3)
            text = File::descriptionOfSizeInBytes ((int64) r->data.getSize());

        widest = jmax (widest, f.getStringWidth (text));
    }

    return widest + 10;
}

void ResourceEditorPanel::updateLookAndFeel()
{
    listBox->setColour (ListBox::backgroundColourId, findColour (secondaryBackgroundColourId));
    listBox->setColour (ListBox::outlineColourId, Colours::transparentBlack);
}

void ResourceEditorPanel::lookAndFeelChanged()
{
    updateLookAndFeel();
}

//==============================================================================
class ResourceSorter
{
public:
    ResourceSorter (const int columnId_, const bool forwards)
        : columnId (columnId_),
          direction (forwards ? 1 : -1)
    {
    }

    int compareElements (BinaryResources::BinaryResource* first, BinaryResources::BinaryResource* second)
    {
        if (columnId == 1)  return direction * first->name.compare (second->name);
        if (columnId == 2)  return direction * first->originalFilename.compare (second->originalFilename);
        if (columnId == 3)  return direction * (int) first->data.getSize() - (int) second->data.getSize();

        return 0;
    }

private:
    const int columnId, direction;
    ResourceSorter (const ResourceSorter&);
    ResourceSorter& operator= (const ResourceSorter&);
};

void ResourceEditorPanel::sortOrderChanged (int newSortColumnId, const bool isForwards)
{
    ResourceSorter sorter (newSortColumnId, isForwards);
    document.getResources().sort (sorter);
}

//==============================================================================
void ResourceEditorPanel::selectedRowsChanged (int /*lastRowSelected*/)
{
    delButton.setEnabled (listBox->getNumSelectedRows() > 0);
}

void ResourceEditorPanel::resized()
{
    auto bounds = getLocalBounds();

    auto buttonSlice = bounds.removeFromBottom (40).reduced (5, 5);

    addButton.setBounds (buttonSlice.removeFromLeft (125));
    buttonSlice.removeFromLeft (10);

    reloadAllButton.setBounds (buttonSlice.removeFromLeft (125));

    delButton.setBounds (buttonSlice.removeFromRight (125));

    listBox->setBounds (bounds);
}

void ResourceEditorPanel::paint (Graphics& g)
{
    g.fillAll (findColour (secondaryBackgroundColourId));
}

void ResourceEditorPanel::visibilityChanged()
{
    if (isVisible())
        listBox->updateContent();
}

void ResourceEditorPanel::changeListenerCallback (ChangeBroadcaster*)
{
    if (isVisible())
        listBox->updateContent();
}

void ResourceEditorPanel::reloadAll()
{
    StringArray failed;

    for (int i = 0; i < document.getResources().size(); ++i)
        if (! document.getResources().reload (i))
            failed.add (document.getResources().getResourceNames() [i]);

    if (failed.size() > 0)
    {
        auto options = MessageBoxOptions::makeOptionsOk (MessageBoxIconType::WarningIcon,
                                                         TRANS ("Reloading resources"),
                                                         TRANS ("The following resources couldn't be reloaded from their original files:\n\n")
                                                             + failed.joinIntoString (", "));
        messageBox = AlertWindow::showScopedAsync (options, nullptr);
    }
}
