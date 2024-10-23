/*
  ==============================================================================

    ConfigComponent.h
    Created: 23 Oct 2024 3:34:13pm
    Author:  lukas

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class ConfigComponent  : public juce::Component
{
public:
    ConfigComponent();
    ~ConfigComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConfigComponent)
};
