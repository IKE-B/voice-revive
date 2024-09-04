/*
  ==============================================================================

    CompGainEQ.h
    Created: 3 Sep 2024 4:26:59pm
    Author:  Simon

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct ChainSettings
{
    float peakFreq{ 0 }, peakGainInDecibels{ 0 }, peakQuality{ 1.f };
    float lowCutFreq{ 0 }, highCutFreq{ 0 };
    float lowCutSlope{ 12.0f }, highCutSlope{ 12.0f };
};

ChainSettings getChainSettings(const std::unordered_map<std::string, float>& floatValues);
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
    void CompGainEQ::prepareToPlayEQ(double sampleRate, int samplesPerBlock);
    void CompGainEQ::prepareToPlayCompAll(double sampleRate, int samplesPerBlock);
    void CompGainEQ::prepareToPlayCompMultBand(double sampleRate, int samplesPerBlock);
    void CompGainEQ::prepareToPlayGain(double sampleRate, int samplesPerBlock);

    void releaseResources() override;

    #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    #endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void CompGainEQ::processBlockEQ(juce::AudioBuffer<float>&, juce::MidiBuffer&);
    void CompGainEQ::processBlockCompAll(juce::AudioBuffer<float>&, juce::MidiBuffer&);
    void CompGainEQ::processBlockCompMultBand(juce::AudioBuffer<float>&, juce::MidiBuffer&);
    void CompGainEQ::splitBands(const juce::AudioBuffer<float>& inputBuffer);

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

    ChainSettings getChainSettings(const std::unordered_map<std::string, float>& floatValues);

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

    std::array<juce::AudioBuffer<float>, 3> filterBuffers;

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

    template<typename T, typename U>
    void applyGain(T& buffer, U& gain)
    {
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto ctx = juce::dsp::ProcessContextReplacing<float>(block);
        gain.process(ctx);
    }

    // EQ
    using Filter = juce::dsp::IIR::Filter<float>;
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
    using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;
    MonoChain leftChain, rightChain;

    enum ChainPositions
    {
        LowCut,
        Peak,
        HighCut
    };

    void updatePeakFilter(const ChainSettings& chainSettings);
    using Coefficients = Filter::CoefficientsPtr;
    static void updateCoefficients(Coefficients& old, const Coefficients& replacements);

    template<int Index, typename ChainType, typename CoefficientType>
    void update(ChainType& chain, const CoefficientType& coefficients)
    {
        updateCoefficients(chain.template get<Index>().coefficients, coefficients[Index]);
        chain.template setBypassed<Index>(false);
    }

    template<typename ChainType, typename CoefficientType>
    void updateCutFilter(ChainType& chain, const CoefficientType& cutCoefficients, const float lowCutSlope)
    {
        chain.template setBypassed<0>(true);
        chain.template setBypassed<1>(true);
        chain.template setBypassed<2>(true);
        chain.template setBypassed<3>(true);

        switch (lowCutSlope)
        {
        case 48.0f:
        {
            update<3>(chain, cutCoefficients);
        }
        case 36.0f:
        {
            update<2>(chain, cutCoefficients);
        }
        case 24.0f:
        {
            update<1>(chain, cutCoefficients);
        }
        case 12.0f:
        {
            update<0>(chain, cutCoefficients);
        }
        }
    }

    void updateLowCutFilters(const ChainSettings& chainSettings);
    void updateHighCutFilters(const ChainSettings& chainSettings);

    void updateFilters(const std::unordered_map<std::string, float>& floatValues);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompGainEQ);
};