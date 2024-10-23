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
StartComponent::StartComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    addAndMakeVisible(&playButton);
    playButton.setButtonText("Play");
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    playButton.setEnabled(false);

    addAndMakeVisible(&stopButton);
    stopButton.setButtonText("Stop");
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    stopButton.setEnabled(false);
}

StartComponent::~StartComponent()
{
}

void StartComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    //g.setColour (juce::Colours::grey);
    //g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    //g.setFont (juce::FontOptions (14.0f));
    //g.drawText ("StartComponent", getLocalBounds(),
                //juce::Justification::centred, true);   // draw some placeholder text
}

void StartComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    playButton.setBounds(getWidth() / 2, getHeight() / 2, 50, 40);
    stopButton.setBounds(getWidth() / 2, getHeight() / 2 + 50, 50, 40);
}

void StartComponent::playButtonClicked()
{
    stopButton.setButtonText("Stop");
    stopButton.setEnabled(true);
    playButton.setEnabled(false);
}

void StartComponent::stopButtonClicked()
{
    playButton.setButtonText("Play");
    stopButton.setButtonText("Stop");
    playButton.setEnabled(true);
    stopButton.setEnabled(false);
}
