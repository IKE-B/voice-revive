/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class AudioFilePlayerAudioProcessorEditor  : public juce::AudioProcessorEditor,
    public juce::ChangeListener
{
public:
    AudioFilePlayerAudioProcessorEditor (AudioFilePlayerAudioProcessor&);
    ~AudioFilePlayerAudioProcessorEditor() override;

    //==============================================================================

    /*void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
    {
        if (readerSource.get() == nullptr)
        {
            bufferToFill.clearActiveBufferRegion();
            return;
        }
    }*/

    void paint (juce::Graphics&) override;
    void resized() override;

    void changeListenerCallback(juce::ChangeBroadcaster *source)
    {
        if (source == &audioProcessor.transportSource)
        {
            if (audioProcessor.transportSource.isPlaying())
                changeState(Playing);
            else
                changeState(Stopped);
        }
    }

private:
    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Stopping
    };

    void changeState(TransportState newState)
    {
        if (state != newState)
        {
            state = newState;

            switch (state)
            {
                case Stopped:
                    stopButton.setEnabled(false);
                    playButton.setEnabled(true);
                    audioProcessor.transportSource.setPosition(0.0);
                    break;

                case Starting:
                    playButton.setEnabled(false);
                    audioProcessor.transportSource.start();
                    break;

                case Playing:
                    stopButton.setEnabled(true);
                    break;

                case Stopping:
                    audioProcessor.transportSource.stop();
                    break;
            }
        }
    }
    void openButtonClicked()
    {
        fileChooser = std::make_unique<juce::FileChooser>("Select a Wave file to play...", juce::File {}, "*.wav;*.mkv;*.mp4;*.mp3");
        
        auto fileChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

        fileChooser->launchAsync(fileChooserFlags, [this](const juce::FileChooser &fc)
                                 {
                                     auto file = fc.getResult();

                                     if (file != juce::File {})
                                     {
                                         auto *reader = formatManager.createReaderFor(file);

                                         if (reader != nullptr)
                                         {
                                             auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
                                             audioProcessor.transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
                                             playButton.setEnabled(true);
                                             readerSource.reset(newSource.release());
                                         }
                                     }
                                 });
    }
    void playButtonClicked()
    { 
        changeState(Starting);
    }
    void stopButtonClicked()
    {
        changeState(Stopping);
    }

    //==============================================================================
    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;

    std::unique_ptr<juce::FileChooser> fileChooser;

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    TransportState state;
    AudioFilePlayerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioFilePlayerAudioProcessorEditor)
};
