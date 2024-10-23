/*
  ==============================================================================

    ConfigComponent.h
    Created: 23 Oct 2024 3:34:13pm
    Author:  lukas

  ==============================================================================
*/


/*
#pragma once

#include <JuceHeader.h>

class ConfigComponent : public juce::Component
{
public:
    ConfigComponent();
    ~ConfigComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    juce::Slider gain;
    juce::Slider CompAllAttack;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigComponent)
};
*/

#pragma once

#include <JuceHeader.h>

class ConfigComponent : public juce::Component
{
public:
    // Konstruktor und Destruktor
    ConfigComponent();
    ~ConfigComponent() override;

    // Methoden, die von juce::Component �berschrieben werden
    void paint(juce::Graphics& g) override;
    void resized() override;
    void paintOverChildren(juce::Graphics& g) override; // Um die R�nder um die Slider zu zeichnen

private:
    juce::Viewport viewport; // Der Viewport f�r das Scrollen
    std::unique_ptr<juce::Component> componentContainer; // Container f�r die Slider und Labels

    // Arrays, um Slider und Labels zu speichern
    juce::OwnedArray<juce::Slider> sliders;
    juce::OwnedArray<juce::Label> labels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigComponent)
};
