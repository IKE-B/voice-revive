/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioFilePlayerAudioProcessorEditor::AudioFilePlayerAudioProcessorEditor (AudioFilePlayerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), state(Stopped)
{
    // Open Button
    addAndMakeVisible(&openButton);
    openButton.setButtonText("Open...");
    openButton.onClick = [this] { openButtonClicked(); };

    // Play Button
    addAndMakeVisible(&playButton);
    playButton.setButtonText("Play");
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    playButton.setEnabled(false);

    // Stop Button
    addAndMakeVisible(&stopButton);
    stopButton.setButtonText("Stop");
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    stopButton.setEnabled(false);

    setSize (300, 200);

    formatManager.registerBasicFormats();
    audioProcessor.transportSource.addChangeListener(this);
}

AudioFilePlayerAudioProcessorEditor::~AudioFilePlayerAudioProcessorEditor()
{
}

//==============================================================================
void AudioFilePlayerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    //g.drawFittedText ("Hello World!", 0, 0, getWidth(), 30, juce::Justification::centred, 1);
}

void AudioFilePlayerAudioProcessorEditor::resized()
{
    openButton.setBounds(10, 10, getWidth() - 20, 20);
    playButton.setBounds(10, 40, getWidth() - 20, 20);
    stopButton.setBounds(10, 70, getWidth() - 20, 20);
}
