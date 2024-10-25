/*
  ==============================================================================

    HelperStructs.h
    Created: 25 Oct 2024 3:08:40pm
    Author:  lukas

  ==============================================================================
*/

#pragma once

#pragma once

#include <JuceHeader.h>

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

struct CompressorBand
{
    float threshold = 0.0f;
    float attack = 40.0f;
    float release = 40.0f;
    float ratio = 3.0f;
    bool bypassed = false;
    bool mute = false;
    bool solo = false;

    void prepare(const juce::dsp::ProcessSpec &spec)
    {
        compressor.prepare(spec);
    }

    void process(juce::AudioBuffer<float> &buffer)
    {
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto context = juce::dsp::ProcessContextReplacing<float>(block);

        context.isBypassed = bypassed;

        compressor.process(context);
    }
private:
    juce::dsp::Compressor<float> compressor;
};

