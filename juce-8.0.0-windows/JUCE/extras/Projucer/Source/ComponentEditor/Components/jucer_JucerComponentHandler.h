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

#pragma once

#include "../UI/jucer_TestComponent.h"
#include "../Properties/jucer_FilePropertyComponent.h"
#include "../Properties/jucer_ComponentTextProperty.h"
#include "jucer_ComponentUndoableAction.h"
#include "../../Project/UI/jucer_ProjectContentComponent.h"

//==============================================================================
class JucerComponentHandler  : public ComponentTypeHandler
{
public:
    JucerComponentHandler()
        : ComponentTypeHandler ("Projucer Component", "xxx",
                                typeid (TestComponent), 300, 200)
    {}

    Component* createNewComponent (JucerDocument* doc) override
    {
        return new TestComponent (doc, nullptr, false);
    }

    String getXmlTagName() const noexcept override               { return "JUCERCOMP"; }

    XmlElement* createXmlFor (Component* comp, const ComponentLayout* layout) override
    {
        TestComponent* const tc = dynamic_cast<TestComponent*> (comp);

        XmlElement* e = ComponentTypeHandler::createXmlFor (comp, layout);
        e->setAttribute ("sourceFile", tc->getFilename());
        e->setAttribute ("constructorParams", tc->getConstructorParams());

        return e;
    }

    bool restoreFromXml (const XmlElement& xml, Component* comp, const ComponentLayout* layout) override
    {
        auto tc = dynamic_cast<TestComponent*> (comp);

        if (! ComponentTypeHandler::restoreFromXml (xml, comp, layout))
            return false;

        tc->setFilename (xml.getStringAttribute ("sourceFile", tc->getFilename()));
        tc->setConstructorParams (xml.getStringAttribute ("constructorParams"));

        return true;
    }

    String getClassName (Component* comp) const override
    {
        auto tc = dynamic_cast<TestComponent*> (comp);

        String jucerCompClassName;

        if (tc->getDocument() != nullptr)
            jucerCompClassName = tc->getDocument()->getClassName();

        if (jucerCompClassName.isEmpty())
            jucerCompClassName = "juce::Component";

        return jucerCompClassName;
    }

    void getEditableProperties (Component* component, JucerDocument& document,
                                Array<PropertyComponent*>& props, bool multipleSelected) override
    {
        ComponentTypeHandler::getEditableProperties (component, document, props, multipleSelected);

        if (multipleSelected)
            return;

        if (auto tc = dynamic_cast<TestComponent*> (component))
        {
            props.add (new JucerCompFileProperty (tc, document));
            props.add (new ConstructorParamsProperty (tc, document));
            props.add (new JucerCompOpenDocProperty (tc));
        }
    }

    String getCreationParameters (GeneratedCode&, Component* component) override
    {
        return dynamic_cast<TestComponent*> (component)->getConstructorParams().trim();
    }

    void fillInCreationCode (GeneratedCode& code, Component* component, const String& memberVariableName) override
    {
        ComponentTypeHandler::fillInCreationCode (code, component, memberVariableName);

        if (auto tc = dynamic_cast<TestComponent*> (component))
            code.includeFilesH.add (tc->findFile().withFileExtension (".h"));
        else
            jassertfalse;
    }

    //==============================================================================
    class JucerCompFileChangeAction  : public ComponentUndoableAction <TestComponent>
    {
    public:
        JucerCompFileChangeAction (TestComponent* const comp, ComponentLayout& l, const String& newState_)
            : ComponentUndoableAction <TestComponent> (comp, l),
              newState (newState_)
        {
            oldState = comp->getFilename();
        }

        bool perform() override
        {
            showCorrectTab();
            getComponent()->setFilename (newState);
            changed();
            return true;
        }

        bool undo() override
        {
            showCorrectTab();
            getComponent()->setFilename (oldState);
            changed();
            return true;
        }

        String newState, oldState;
    };

    static void setJucerComponentFile (JucerDocument& document, TestComponent* comp, const String& newFilename)
    {
        jassert (comp != nullptr);

        if (comp != nullptr)
            document.perform (new JucerCompFileChangeAction (comp, *document.getComponentLayout(), newFilename),
                              "Change Projucer component file");
    }

private:
    //==============================================================================
    class JucerCompFileProperty  : public FilePropertyComponent,
                                   private ChangeListener
    {
    public:
        JucerCompFileProperty (TestComponent* const comp, JucerDocument& doc)
            : FilePropertyComponent ("Jucer file", false, true),
              component (comp),
              document (doc)
        {
            document.addChangeListener (this);
        }

        ~JucerCompFileProperty() override
        {
            document.removeChangeListener (this);
        }

        void setFile (const File& newFile) override
        {
            setJucerComponentFile (document, component,
                                   newFile.getRelativePathFrom (document.getCppFile().getParentDirectory())
                                          .replaceCharacter ('\\', '/'));
        }

        File getFile() const override
        {
            return component->findFile();
        }

    private:
        void changeListenerCallback (ChangeBroadcaster*) override
        {
            refresh();
        }

        TestComponent* const component;
        JucerDocument& document;
    };

    //==============================================================================
    struct JucerCompOpenDocProperty  : public ButtonPropertyComponent
    {
        JucerCompOpenDocProperty (TestComponent* const c)
            : ButtonPropertyComponent ("edit", false),
              component (c)
        {
        }

        void buttonClicked() override
        {
            if (ProjectContentComponent* const pcc = findParentComponentOfClass<ProjectContentComponent>())
                pcc->showEditorForFile (component->findFile(), true);
        }

        String getButtonText() const override
        {
            return "Open file for editing";
        }

        TestComponent* const component;
    };

    //==============================================================================
    struct ConstructorParamsProperty   : public ComponentTextProperty <TestComponent>
    {
        ConstructorParamsProperty (TestComponent* comp, JucerDocument& doc)
            : ComponentTextProperty <TestComponent> ("constructor params", 512, false, comp, doc)
        {
        }

        void setText (const String& newText) override
        {
            document.perform (new ConstructorParamChangeAction (component, *document.getComponentLayout(), newText),
                              "Change Viewport content constructor params");
        }

        String getText() const override
        {
            return component->getConstructorParams();
        }

    private:
        struct ConstructorParamChangeAction  : public ComponentUndoableAction <TestComponent>
        {
            ConstructorParamChangeAction (TestComponent* const comp, ComponentLayout& l, const String& newValue_)
                : ComponentUndoableAction <TestComponent> (comp, l),
                  newValue (newValue_)
            {
                oldValue = comp->getConstructorParams();
            }

            bool perform() override
            {
                showCorrectTab();
                getComponent()->setConstructorParams (newValue);
                changed();
                layout.getDocument()->refreshAllPropertyComps();
                return true;
            }

            bool undo() override
            {
                showCorrectTab();
                getComponent()->setConstructorParams (oldValue);
                changed();
                layout.getDocument()->refreshAllPropertyComps();
                return true;
            }

            String newValue, oldValue;
        };
    };
};
