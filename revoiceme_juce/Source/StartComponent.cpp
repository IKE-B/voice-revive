/*
  ==============================================================================

    StartComponent.cpp
    Created: 23 Oct 2024 3:37:44pm
    Author:  lukas

  ==============================================================================
*/

#include <JuceHeader.h>
#include "StartComponent.h"

//==============================================================================
StartComponent::StartComponent(juce::AudioDeviceManager& deviceMgt) : deviceManager(deviceMgt), state(Stopped)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    addAndMakeVisible(&playStopButton);
    playStopButton.setButtonText("PLAY");
    playStopButton.onClick = [this] { playButtonClicked(); };
    playStopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    
    /*addAndMakeVisible(&playButton);
    playButton.setButtonText("Play");
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    playButton.setEnabled(false);

    addAndMakeVisible(&stopButton);
    stopButton.setButtonText("Stop");
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    stopButton.setEnabled(false);*/

    deviceManager.addChangeListener(this);
}

StartComponent::~StartComponent()
{
    deviceManager.removeChangeListener(this);
}

void StartComponent::changeListenerCallback(juce::ChangeBroadcaster *source)
{
    if (source == &deviceManager)
    {
        if (deviceManager.getCurrentAudioDevice()->isPlaying())
        {
            changeAudioState(Playing);
        }
        else changeAudioState(Stopped);
    }
}

void StartComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 2);   // draw an outline around the component

    //g.setColour (juce::Colours::white);
    //g.setFont (juce::FontOptions (14.0f));
    //g.drawText ("StartComponent", getLocalBounds(),
                //juce::Justification::centred, true);   // draw some placeholder text
}

void StartComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    //playButton.setBounds(getLocalBounds());
    //stopButton.setBounds(getLocalBounds());

    //playButton.setBounds(getWidth() / 2 - playButton.getWidth() / 2, getHeight() / 2 - playButton.getHeight() / 2, 50, 40);
    //stopButton.setBounds(getWidth() / 2 - stopButton.getWidth() / 2, getHeight() / 2 - stopButton.getHeight() / 2 + 50, 50, 40);
    playStopButton.setBounds(getWidth() / 2 - playStopButton.getWidth() / 2, getHeight() / 2 - playStopButton.getHeight() / 2, 100, 100);
}

void StartComponent::playButtonClicked()
{
    changeAudioState(Playing);
}

void StartComponent::stopButtonClicked()
{
    changeAudioState(Stopped);
}

void StartComponent::changeAudioState(audioState newState)
{
    if (state != newState)
    {
        state = newState;

        switch (state)
        {
            case Playing:
                deviceManager.initialiseWithDefaultDevices(2,2);
                //stopButton.setEnabled(true);
                //playButton.setEnabled(false);
                playStopButton.setButtonText("STOP");
                playStopButton.onClick = [this] { stopButtonClicked(); };
                playStopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
                break;
            case Stopped:
                //playButton.setEnabled(true);
                //stopButton.setEnabled(false);
                deviceManager.getCurrentAudioDevice()->stop();
                playStopButton.setButtonText("PLAY");
                playStopButton.onClick = [this] { playButtonClicked(); };
                playStopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
                break;
        }
    }
}
