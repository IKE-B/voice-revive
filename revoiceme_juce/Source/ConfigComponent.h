#pragma once

#include <JuceHeader.h>

// Benutzerdefinierte Slider-Klasse mit Label und Rand
class CustomSliderWithLabel : public juce::Component
{
public:
    CustomSliderWithLabel(const juce::String& labelText)
    {
        slider.setSliderStyle(juce::Slider::LinearHorizontal);
        slider.setRange(0, 10, 0.01);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        slider.setNumDecimalPlacesToDisplay(2);
        addAndMakeVisible(slider);

        label.setText(labelText, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
    }

    void paint(juce::Graphics& g) override
    {
        g.setColour(juce::Colours::black);
        g.drawRect(getLocalBounds().reduced(5), 2); // Rahmen mit 2 Pixeln Dicke
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced(10);
        label.setBounds(bounds.removeFromTop(20));
        slider.setBounds(bounds.reduced(5));
    }

    juce::Slider slider;
    juce::Label label;
};

// Benutzerdefinierte Checkbox-Klasse mit Rand
class CustomCheckbox : public juce::Component
{
public:
    CustomCheckbox(const juce::String& checkboxText)
    {
        checkbox.setButtonText(checkboxText);
        checkbox.setToggleState(false, juce::dontSendNotification);
        addAndMakeVisible(checkbox);
    }

    void paint(juce::Graphics& g) override
    {
        g.setColour(juce::Colours::black);
        g.drawRect(getLocalBounds().reduced(5), 2); // Rahmen mit 2 Pixeln Dicke
    }

    void resized() override
    {
        checkbox.setBounds(getLocalBounds().reduced(10));
    }

    juce::ToggleButton checkbox;
};

// ConfigComponent Klasse
class ConfigComponent : public juce::Component
{
public:
    ConfigComponent();
    ~ConfigComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void paintOverChildren(juce::Graphics& g) override;

private:
    juce::Viewport viewport; // Der Viewport f�r das Scrollen
    std::unique_ptr<juce::Component> componentContainer; // Container f�r die Slider und Labels

    // Arrays, um Slider und Checkboxen zu speichern
    juce::OwnedArray<CustomSliderWithLabel> sliders;
    juce::OwnedArray<CustomCheckbox> checkboxes;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigComponent)
};
