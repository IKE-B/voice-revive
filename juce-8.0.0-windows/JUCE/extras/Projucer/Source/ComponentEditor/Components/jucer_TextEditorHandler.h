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


//==============================================================================
class TextEditorHandler  : public ComponentTypeHandler
{
public:
    TextEditorHandler()
        : ComponentTypeHandler ("Text Editor", "juce::TextEditor", typeid (TextEditor), 150, 24)
    {
        registerColour (juce::TextEditor::textColourId, "text", "textcol");
        registerColour (juce::TextEditor::backgroundColourId, "background", "bkgcol");
        registerColour (juce::TextEditor::highlightColourId, "highlight", "hilitecol");
        registerColour (juce::TextEditor::outlineColourId, "outline", "outlinecol");
        registerColour (juce::TextEditor::shadowColourId, "shadow", "shadowcol");
        registerColour (juce::CaretComponent::caretColourId, "caret", "caretcol");
    }

    Component* createNewComponent (JucerDocument*) override
    {
        return new TextEditor ("new text editor");
    }

    XmlElement* createXmlFor (Component* comp, const ComponentLayout* layout) override
    {
        XmlElement* e = ComponentTypeHandler::createXmlFor (comp, layout);
        TextEditor* te = (TextEditor*) comp;

        e->setAttribute ("initialText", comp->getProperties() ["initialText"].toString());

        e->setAttribute ("multiline", te->isMultiLine());
        e->setAttribute ("retKeyStartsLine", te->getReturnKeyStartsNewLine());
        e->setAttribute ("readonly", te->isReadOnly());
        e->setAttribute ("scrollbars", te->areScrollbarsShown());
        e->setAttribute ("caret", te->isCaretVisible());
        e->setAttribute ("popupmenu", te->isPopupMenuEnabled());

        return e;
    }

    bool restoreFromXml (const XmlElement& xml, Component* comp, const ComponentLayout* layout) override
    {
        if (! ComponentTypeHandler::restoreFromXml (xml, comp, layout))
            return false;

        TextEditor* te = (TextEditor*) comp;
        TextEditor defaultEditor;

        te->setMultiLine (xml.getBoolAttribute ("multiline", defaultEditor.isMultiLine()));
        te->setReturnKeyStartsNewLine (xml.getBoolAttribute ("retKeyStartsLine", defaultEditor.getReturnKeyStartsNewLine()));
        te->setReadOnly (xml.getBoolAttribute ("readonly", defaultEditor.isReadOnly()));
        te->setScrollbarsShown (xml.getBoolAttribute ("scrollbars", defaultEditor.areScrollbarsShown()));
        te->setCaretVisible (xml.getBoolAttribute ("caret", defaultEditor.isCaretVisible()));
        te->setPopupMenuEnabled (xml.getBoolAttribute ("popupmenu", defaultEditor.isPopupMenuEnabled()));

        const String initialText (xml.getStringAttribute ("initialText"));
        te->setText (initialText, false);
        te->getProperties().set ("initialText", initialText);
        return true;
    }

    void getEditableProperties (Component* component, JucerDocument& document,
                                Array<PropertyComponent*>& props, bool multipleSelected) override
    {
        ComponentTypeHandler::getEditableProperties (component, document, props, multipleSelected);

        if (multipleSelected)
            return;

        if (auto* t = dynamic_cast<TextEditor*> (component))
        {
            props.add (new TextEditorInitialTextProperty (t, document));
            props.add (new TextEditorMultiLineProperty (t, document));
            props.add (new TextEditorReadOnlyProperty (t, document));
            props.add (new TextEditorScrollbarsProperty (t, document));
            props.add (new TextEditorCaretProperty (t, document));
            props.add (new TextEditorPopupMenuProperty (t, document));

            addColourProperties (t, document, props);
        }
    }

    String getCreationParameters (GeneratedCode&, Component* component) override
    {
        return quotedString (component->getName(), false);
    }

    void fillInCreationCode (GeneratedCode& code, Component* component, const String& memberVariableName) override
    {
        ComponentTypeHandler::fillInCreationCode (code, component, memberVariableName);

        if (auto* te = dynamic_cast<TextEditor*> (component))
        {
            String s;
            s << memberVariableName << "->setMultiLine (" << CodeHelpers::boolLiteral (te->isMultiLine()) << ");\n"
              << memberVariableName << "->setReturnKeyStartsNewLine (" << CodeHelpers::boolLiteral (te->getReturnKeyStartsNewLine()) << ");\n"
              << memberVariableName << "->setReadOnly (" << CodeHelpers::boolLiteral (te->isReadOnly()) << ");\n"
              << memberVariableName << "->setScrollbarsShown (" << CodeHelpers::boolLiteral (te->areScrollbarsShown()) << ");\n"
              << memberVariableName << "->setCaretVisible (" << CodeHelpers::boolLiteral (te->isCaretVisible()) << ");\n"
              << memberVariableName << "->setPopupMenuEnabled (" << CodeHelpers::boolLiteral (te->isPopupMenuEnabled()) << ");\n"
              << getColourIntialisationCode (component, memberVariableName)
              << memberVariableName << "->setText (" << quotedString (te->getProperties() ["initialText"].toString(), code.shouldUseTransMacro()) << ");\n\n";

            code.constructorCode += s;
        }
    }

private:
    //==============================================================================
    class TextEditorMultiLineProperty  : public ComponentChoiceProperty <TextEditor>
    {
    public:
        TextEditorMultiLineProperty (TextEditor* comp, JucerDocument& doc)
            : ComponentChoiceProperty <TextEditor> ("mode", comp, doc)
        {
            choices.add ("single line");
            choices.add ("multi-line, return key starts new line");
            choices.add ("multi-line, return key disabled");
        }

        void setIndex (int newIndex) override
        {
            document.perform (new TextEditorMultilineChangeAction (component, *document.getComponentLayout(), newIndex),
                              "Change TextEditor multiline mode");
        }

        int getIndex() const override
        {
            return component->isMultiLine() ? (component->getReturnKeyStartsNewLine() ? 1 : 2) : 0;
        }

    private:
        class TextEditorMultilineChangeAction  : public ComponentUndoableAction <TextEditor>
        {
        public:
            TextEditorMultilineChangeAction (TextEditor* const comp, ComponentLayout& l, const int newState_)
                : ComponentUndoableAction <TextEditor> (comp, l),
                  newState (newState_)
            {
                oldState = comp->isMultiLine() ? (comp->getReturnKeyStartsNewLine() ? 1 : 2) : 0;
            }

            bool perform() override
            {
                showCorrectTab();
                getComponent()->setMultiLine (newState > 0);
                getComponent()->setReturnKeyStartsNewLine (newState == 1);
                changed();
                return true;
            }

            bool undo() override
            {
                showCorrectTab();
                getComponent()->setMultiLine (oldState > 0);
                getComponent()->setReturnKeyStartsNewLine (oldState == 1);
                changed();
                return true;
            }

            int newState, oldState;
        };
    };

    //==============================================================================
    class TextEditorReadOnlyProperty  : public ComponentBooleanProperty <TextEditor>
    {
    public:
        TextEditorReadOnlyProperty (TextEditor* comp, JucerDocument& doc)
            : ComponentBooleanProperty <TextEditor> ("editable", "Editable", "Editable", comp, doc)
        {
        }

        void setState (bool newState) override
        {
            document.perform (new TextEditorReadonlyChangeAction (component, *document.getComponentLayout(), ! newState),
                              "Change TextEditor read-only mode");
        }

        bool getState() const override        { return ! component->isReadOnly(); }

    private:
        class TextEditorReadonlyChangeAction  : public ComponentUndoableAction <TextEditor>
        {
        public:
            TextEditorReadonlyChangeAction (TextEditor* const comp, ComponentLayout& l, const bool newState_)
                : ComponentUndoableAction <TextEditor> (comp, l),
                  newState (newState_)
            {
                oldState = comp->isReadOnly();
            }

            bool perform() override
            {
                showCorrectTab();
                getComponent()->setReadOnly (newState);
                changed();
                return true;
            }

            bool undo() override
            {
                showCorrectTab();
                getComponent()->setReadOnly (oldState);
                changed();
                return true;
            }

            bool newState, oldState;
        };
    };

    //==============================================================================
    class TextEditorScrollbarsProperty  : public ComponentBooleanProperty <TextEditor>
    {
    public:
        TextEditorScrollbarsProperty (TextEditor* comp, JucerDocument& doc)
            : ComponentBooleanProperty <TextEditor> ("scrollbars", "Scrollbars enabled", "Scrollbars enabled", comp, doc)
        {
        }

        void setState (bool newState) override
        {
            document.perform (new TextEditorScrollbarChangeAction (component, *document.getComponentLayout(), newState),
                              "Change TextEditor scrollbars");
        }

        bool getState() const override        { return component->areScrollbarsShown(); }

    private:
        class TextEditorScrollbarChangeAction  : public ComponentUndoableAction <TextEditor>
        {
        public:
            TextEditorScrollbarChangeAction (TextEditor* const comp, ComponentLayout& l, const bool newState_)
                : ComponentUndoableAction <TextEditor> (comp, l),
                  newState (newState_)
            {
                oldState = comp->areScrollbarsShown();
            }

            bool perform() override
            {
                showCorrectTab();
                getComponent()->setScrollbarsShown (newState);
                changed();
                return true;
            }

            bool undo() override
            {
                showCorrectTab();
                getComponent()->setScrollbarsShown (oldState);
                changed();
                return true;
            }

            bool newState, oldState;
        };
    };

    //==============================================================================
    class TextEditorCaretProperty  : public ComponentBooleanProperty <TextEditor>
    {
    public:
        TextEditorCaretProperty (TextEditor* comp, JucerDocument& doc)
            : ComponentBooleanProperty <TextEditor> ("caret", "Caret visible", "Caret visible", comp, doc)
        {
        }

        void setState (bool newState) override
        {
            document.perform (new TextEditorCaretChangeAction (component, *document.getComponentLayout(), newState),
                              "Change TextEditor caret");
        }

        bool getState() const override        { return component->isCaretVisible(); }

    private:
        class TextEditorCaretChangeAction  : public ComponentUndoableAction <TextEditor>
        {
        public:
            TextEditorCaretChangeAction (TextEditor* const comp, ComponentLayout& l, const bool newState_)
                : ComponentUndoableAction <TextEditor> (comp, l),
                  newState (newState_)
            {
                oldState = comp->isCaretVisible();
            }

            bool perform() override
            {
                showCorrectTab();
                getComponent()->setCaretVisible (newState);
                changed();
                return true;
            }

            bool undo() override
            {
                showCorrectTab();
                getComponent()->setCaretVisible (oldState);
                changed();
                return true;
            }

            bool newState, oldState;
        };
    };

    //==============================================================================
    class TextEditorPopupMenuProperty  : public ComponentBooleanProperty <TextEditor>
    {
    public:
        TextEditorPopupMenuProperty (TextEditor* comp, JucerDocument& doc)
            : ComponentBooleanProperty <TextEditor> ("popup menu", "Popup menu enabled", "Popup menu enabled", comp, doc)
        {
        }

        void setState (bool newState) override
        {
            document.perform (new TextEditorPopupMenuChangeAction (component, *document.getComponentLayout(), newState),
                              "Change TextEditor popup menu");
        }

        bool getState() const override        { return component->isPopupMenuEnabled(); }

    private:
        class TextEditorPopupMenuChangeAction  : public ComponentUndoableAction <TextEditor>
        {
        public:
            TextEditorPopupMenuChangeAction (TextEditor* const comp, ComponentLayout& l, const bool newState_)
                : ComponentUndoableAction <TextEditor> (comp, l),
                  newState (newState_)
            {
                oldState = comp->isPopupMenuEnabled();
            }

            bool perform() override
            {
                showCorrectTab();
                getComponent()->setPopupMenuEnabled (newState);
                changed();
                return true;
            }

            bool undo() override
            {
                showCorrectTab();
                getComponent()->setPopupMenuEnabled (oldState);
                changed();
                return true;
            }

            bool newState, oldState;
        };
    };

    //==============================================================================
    class TextEditorInitialTextProperty  : public ComponentTextProperty <TextEditor>
    {
    public:
        TextEditorInitialTextProperty (TextEditor* comp, JucerDocument& doc)
            : ComponentTextProperty <TextEditor> ("initial text", 10000, true, comp, doc)
        {}

        void setText (const String& newText) override
        {
            document.perform (new TextEditorInitialTextChangeAction (component, *document.getComponentLayout(), newText),
                              "Change TextEditor initial text");
        }

        String getText() const override
        {
            return component->getProperties() ["initialText"];
        }

    private:
        class TextEditorInitialTextChangeAction  : public ComponentUndoableAction <TextEditor>
        {
        public:
            TextEditorInitialTextChangeAction (TextEditor* const comp, ComponentLayout& l, const String& newState_)
                : ComponentUndoableAction <TextEditor> (comp, l),
                  newState (newState_)
            {
                oldState = comp->getProperties() ["initialText"];
            }

            bool perform() override
            {
                showCorrectTab();
                getComponent()->setText (newState, false);
                getComponent()->getProperties().set ("initialText", newState);
                changed();
                return true;
            }

            bool undo() override
            {
                showCorrectTab();
                getComponent()->setText (oldState, false);
                getComponent()->getProperties().set ("initialText", oldState);
                changed();
                return true;
            }

            String newState, oldState;
        };
    };
};
