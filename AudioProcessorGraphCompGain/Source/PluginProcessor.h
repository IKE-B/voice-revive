/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GainProcessor.h"
#include "MultCompressorProcessor.h"
#include "CompressorProcessor.h"

namespace Params
{
    enum Names
    {
        Low_Mid_Crossover_Freq,
        Mid_High_Crossover_Freq,

        Threshold_Low_Band,
        Threshold_Mid_Band,
        Threshold_High_Band,
        Threshold_All,

        Attack_Low_Band,
        Attack_Mid_Band,
        Attack_High_Band,
        Attack_All,

        Release_Low_Band,
        Release_Mid_Band,
        Release_High_Band,
        Release_All,

        Ratio_Low_Band,
        Ratio_Mid_Band,
        Ratio_High_Band,
        Ratio_All,

        Bypassed_Low_Band,
        Bypassed_Mid_Band,
        Bypassed_High_Band,
        Bypassed_All,

        Solo_Low_Band,
        Solo_Mid_Band,
        Solo_High_Band,

        Mute_Low_Band,
        Mute_Mid_Band,
        Mute_High_Band,
        Mute_All,

        Gain,
    };

    inline const std::map<Names, juce::String>& GetParams()
    {
        static std::map<Names, juce::String> params =
        {
            {Low_Mid_Crossover_Freq, "Low Mid Crossover Freq"},
            {Mid_High_Crossover_Freq, "Mid High Crossover Freq"},
            {Threshold_Low_Band, "Threshold Low Band"},
            {Threshold_Mid_Band, "Threshold Mid Band"},
            {Threshold_High_Band, "Threshold High Band"},
            {Threshold_All, "Threshold All"},
            {Attack_Low_Band, "Attack Low Band"},
            {Attack_Mid_Band, "Attack Mid Band"},
            {Attack_High_Band, "Attack High Band"},
            {Attack_All, "Attack All"},
            {Release_Low_Band, "Release Low Band"},
            {Release_Mid_Band, "Release Mid Band"},
            {Release_High_Band, "Release High Band"},
            {Release_All, "Release All"},
            {Ratio_Low_Band, "Ratio Low Band"},
            {Ratio_Mid_Band, "Ratio Mid Band"},
            {Ratio_High_Band, "Ratio High Band"},
            {Ratio_All, "Ratio All"},
            {Bypassed_Low_Band, "Bypassed Low Band"},
            {Bypassed_Mid_Band, "Bypassed Mid Band"},
            {Bypassed_High_Band, "Bypassed High Band"},
            {Bypassed_All, "Bypassed All"},
            {Mute_Low_Band, "Mute Low Band"},
            {Mute_Mid_Band, "Mute Mid Band"},
            {Mute_High_Band, "Mute High Band"},
            {Mute_All, "Mute All"},
            {Solo_Low_Band, "Solo Low Band"},
            {Solo_Mid_Band, "Solo Mid Band"},
            {Solo_High_Band, "Solo High Band"},
            {Gain, "Gain"},
        };

        return params;
    }
}

//==============================================================================
/**
*/
class AudioProcessorGraphCompGainAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioProcessorGraphCompGainAudioProcessor();
    ~AudioProcessorGraphCompGainAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    using APVTS = juce::AudioProcessorValueTreeState;
    static APVTS::ParameterLayout createParameterLayout();

    APVTS apvts{ *this, nullptr, "Parameters", createParameterLayout() };

private:
    GainProcessor gainProcess{};
    MultCompressorProcessor compProcess{};
    CompressorProcessor compAllProcess{};

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioProcessorGraphCompGainAudioProcessor)
};
