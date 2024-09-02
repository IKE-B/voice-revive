/*
  ==============================================================================

    CompressorProcessor.h
    Created: 6 Aug 2024 5:14:48pm
    Author:  Simon

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "ProcessorBase.h"

struct CompressorBandAll
{
    juce::AudioParameterFloat* threshold{ nullptr };
    juce::AudioParameterFloat* attack{ nullptr };
    juce::AudioParameterFloat* release{ nullptr };
    juce::AudioParameterChoice* ratio{ nullptr };
    juce::AudioParameterBool* bypassed{ nullptr };
    juce::AudioParameterBool* mute{ nullptr };

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
class CompressorProcessor : public ProcessorBase
{
public:
    CompressorProcessor() {}

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void updateState();

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    void reset() override {}

    std::array<CompressorBandAll, 1> compressors;
    CompressorBandAll& comp = compressors[0];

    const juce::String getName() const override { return "Compressor"; }

private:
};