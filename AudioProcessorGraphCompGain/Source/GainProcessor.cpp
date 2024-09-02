/*
  ==============================================================================

    GainProcessor.cpp
    Created: 3 Aug 2024 1:14:13pm
    Author:  Simon

  ==============================================================================
*/

#include "GainProcessor.h"

void GainProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // prepare Compressor for compressing the AudioInput
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());
    spec.sampleRate = sampleRate;

    gain.prepare(spec);

    gain.setRampDurationSeconds(0.05);
}

void GainProcessor::updateState()
{
    gain.setGainDecibels(gainParam->get());
}

void GainProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // get the current value
    updateState();

    // apply the gain to the buffer
    applyGain(buffer, gain);
}
