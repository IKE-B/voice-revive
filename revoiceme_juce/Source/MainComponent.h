#pragma once

#include <JuceHeader.h>
#include "StartComponent.h"
#include "ConfigComponent.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent, 
    public juce::ChangeListener
    //private juce::Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void changeListenerCallback(juce::ChangeBroadcaster *source) override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
    /*enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Stopping
    };*/

    bool state;
    
    void changePlaybackState(bool newState);
    void dumpDeviceInfo();
    void logMessage(const juce::String &m);
    //void timerCallback() override;

    
    //juce::Label cpuUsageLabel;
    //juce::Label cpuUsageText;
    //juce::TextEditor diagnosticsBox;

    //TransportState state;

   

    //juce::AudioDeviceSelectorComponent audioSetupComp;
    juce::TabbedComponent tabs;
    Component::SafePointer<StartComponent> startTab;
    Component::SafePointer<ConfigComponent> configTab;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
