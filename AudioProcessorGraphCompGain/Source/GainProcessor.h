/*
  ==============================================================================

    GainProcessor.h
    Created: 3 Aug 2024 1:14:00pm
    Author:  Simon

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ProcessorBase.h"

//==============================================================================
class GainProcessor : public ProcessorBase
{
public:
    GainProcessor() {
        gain.setGainDecibels(0.f);
    }

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void updateState();

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    void reset() override
    {
        gain.reset();
    }

    juce::dsp::Gain<float> gain;
    juce::AudioParameterFloat* gainParam{ nullptr };

    const juce::String getName() const override { return "Gain"; }

private:

    template<typename T, typename U>
    void applyGain(T& buffer, U& gain)
    {
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto ctx = juce::dsp::ProcessContextReplacing<float>(block);
        gain.process(ctx);
    }
};