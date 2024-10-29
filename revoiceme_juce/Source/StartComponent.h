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
class StartComponent  : public juce::Component, juce::ChangeListener
{
public:
    StartComponent(juce::AudioDeviceManager &deviceManager);
    ~StartComponent() override;

    void changeListenerCallback(juce::ChangeBroadcaster *source) override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    enum audioState
    {
        Stopped,
        Playing,
    };

    void playButtonClicked();
    void stopButtonClicked();
    void changeAudioState(audioState newState);

    audioState state;
    juce::TextButton playStopButton;
    //juce::TextButton playButton;
    //juce::TextButton stopButton;

    juce::AudioDeviceManager &deviceManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StartComponent)
};
