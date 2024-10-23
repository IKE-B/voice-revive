#pragma once

#include <JuceHeader.h>
#include "StartComponent.h"
#include "ConfigComponent.h"

enum SlopeEQ
{
    SlopeEQ_12,
    SlopeEQ_24,
    SlopeEQ_36,
    SlopeEQ_48
};

struct ChainSettingsEQ
{
    float peakFreq {0}, peakGainInDecibels {0}, peakQuality {1.f};
    float lowCutFreq {0}, highCutFreq {0};
    SlopeEQ lowCutSlope {SlopeEQ::SlopeEQ_12}, highCutSlope {SlopeEQ::SlopeEQ_12};
};

ChainSettingsEQ getChainSettingsEQ(float lowCutFreqNew, float highCutFreqNew, float peakFreqNew,
                                   float peakGainInDecibelsNew, float peakQualityNew, SlopeEQ lowCutSlopeNew, SlopeEQ highCutSlopeNew);

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:

    //EQ
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

    void updatePeakFilter(const ChainSettingsEQ &chainSettings);
    using Coefficients = Filter::CoefficientsPtr;
    static void updateCoefficients(Coefficients &old, const Coefficients &replacements);

    template<int Index, typename ChainType, typename CoefficientType>
    void update(ChainType &chain, const CoefficientType &coefficients)
    {
        updateCoefficients(chain.template get<Index>().coefficients, coefficients[Index]);
        chain.template setBypassed<Index>(false);
    }

    template<typename ChainType, typename CoefficientType>
    void updateCutFilter(ChainType &chain, const CoefficientType &coefficients,
                         const SlopeEQ &slope)
    {
        chain.template setBypassed<0>(true);
        chain.template setBypassed<1>(true);
        chain.template setBypassed<2>(true);
        chain.template setBypassed<3>(true);

        switch (slope)
        {
            case SlopeEQ_48:
            {
                update<3>(chain, coefficients);
            }
            case SlopeEQ_36:
            {
                update<2>(chain, coefficients);
            }
            case SlopeEQ_24:
            {
                update<1>(chain, coefficients);
            }
            case SlopeEQ_12:
            {
                update<0>(chain, coefficients);
            }
        }
    }

    void updateLowCutFilters(const ChainSettingsEQ &chainSettings);
    void updateHighCutFilters(const ChainSettingsEQ &chainSettings);

    void updateFilters();

    //==============================================================================
    // Your private member variables go here...
    
    juce::TabbedComponent tabs;
    Component::SafePointer<StartComponent> startTab;
    Component::SafePointer<ConfigComponent> configTab;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
