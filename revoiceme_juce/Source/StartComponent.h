/*
  ==============================================================================

    StartComponent.h
    Created: 23 Oct 2024 3:37:44pm
    Author:  lukas

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class StartComponent  : public juce::Component
{
public:
    StartComponent();
    ~StartComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:

    void playButtonClicked();
    void stopButtonClicked();

    juce::TextButton playButton;
    juce::TextButton stopButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StartComponent)
};
