/*
  ==============================================================================

    MultCompressorProcessor.h
    Created: 6 Aug 2024 4:36:06pm
    Author:  Simon

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ProcessorBase.h"

struct CompressorBand
{
    juce::AudioParameterFloat* threshold{ nullptr };
    juce::AudioParameterFloat* attack{ nullptr };
    juce::AudioParameterFloat* release{ nullptr };
    juce::AudioParameterChoice* ratio{ nullptr };
    juce::AudioParameterBool* bypassed{ nullptr };
    juce::AudioParameterBool* mute{ nullptr };
    juce::AudioParameterBool* solo{ nullptr };

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        compressor.prepare(spec);
    }

    void updateCompressorSettings()
    {
        // prepare the compressor with the values from our GUI
        compressor.setAttack(attack->get());
        compressor.setRelease(release->get());
        compressor.setThreshold(threshold->get());
        compressor.setRatio(ratio->getCurrentChoiceName().getFloatValue());
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto context = juce::dsp::ProcessContextReplacing<float>(block);

        context.isBypassed = bypassed->get();

        compressor.process(context);
    }
private:
    juce::dsp::Compressor<float> compressor;
};

//==============================================================================
class MultCompressorProcessor : public ProcessorBase
{
public:
    MultCompressorProcessor() {}

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void updateState();

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    void reset() override {}

    void splitBands(const juce::AudioBuffer<float>& inputBuffer);

    std::array<CompressorBand, 3> compressors;
    CompressorBand& lowBandComp = compressors[0];
    CompressorBand& midBandComp = compressors[1];
    CompressorBand& highBandComp = compressors[2];

    using Filter = juce::dsp::LinkwitzRileyFilter<float>;
    Filter LP1, AP2,
        HP1, LP2,
        HP2;

    juce::AudioParameterFloat* lowMidCrossover{ nullptr };
    juce::AudioParameterFloat* midHighCrossover{ nullptr };

    std::array<juce::AudioBuffer<float>, 3> filterBuffers;

    const juce::String getName() const override { return "MultiBandCompressor"; }

private:
};