/*
  ==============================================================================

    CompGainEQ.h
    Created: 3 Sep 2024 4:26:59pm
    Author:  Simon

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
//==============================================================================
/**
*/
class CompGainEQ : public juce::AudioProcessor
{
public:
    //==============================================================================
    CompGainEQ();
    ~CompGainEQ() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    #endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void updateValues(const std::unordered_map<std::string, float>& floatValues, const std::unordered_map<std::string, bool>& boolValues);

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    template<typename K, typename V>
    V getWithDefault(const std::unordered_map<K, V>& map, const K& key, const V& defaultValue) {
        auto it = map.find(key);
        if (it != map.end()) {
            return it->second;
        }
        return defaultValue;
    }
    
private:
    // compressors
    juce::dsp::Compressor<float> compressorAll;

    juce::dsp::Compressor<float> compressorLow;
    juce::dsp::Compressor<float> compressorMid;
    juce::dsp::Compressor<float> compressorHigh;

    bool compAllMute = false;
    bool compAllBypassed = false;

    bool compLowMute = false;
    bool compLowBypassed = false;
    bool compLowSolo = false;

    bool compMidMute = false;
    bool compMidBypassed = false;
    bool compMidSolo = false;

    bool compHighMute = false;
    bool compHighBypassed = false;
    bool compHighSolo = false;

    using Filter = juce::dsp::LinkwitzRileyFilter<float>;
    Filter LP1, AP2,
        HP1, LP2,
        HP2;



    // gain
    juce::dsp::Gain<float> gain;

    // EQ
    

    

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompGainEQ);
};