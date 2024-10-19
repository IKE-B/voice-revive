/*
  ==============================================================================

    GlobalFunction.cpp
    Created: 18 Oct 2024 2:13:05pm
    Author:  maxig

  ==============================================================================
*/

#ifdef WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT extern "C" __attribute__((visibility("default"))) __attribute__((used))
#endif

#include "CompGainEQ.h"

CompGainEQ myProcessor;

EXPORT
void startModulation()
{
    double sampleRate = 44100.0;
    int samplesPerBlock = 512;
    myProcessor.prepareToPlay(sampleRate, samplesPerBlock);

    juce::AudioBuffer<float> buffer(2, samplesPerBlock);
    juce::MidiBuffer midiBuffer;

    myProcessor.updateValues(0.0f, 50.0f, 250.0f, 0.0f, 3.0f, false, false,
        50.0f, 250.0f, 0.0f, 3.0f, false, false, false,
        50.0f, 250.0f, 0.0f, 3.0f, false, false, false,
        50.0f, 250.0f, 0.0f, 3.0f, false, false, false,
        400.0f, 2000.0f,
        20.0f, 20000.0f, 750.0f, 0.0f, 1.0f, 12.0f, 12.0f);

    myProcessor.processBlock(buffer, midiBuffer);
}

EXPORT
void stopModulation()
{
    juce::JUCEApplicationBase::quit();
    //juce::MessageManager::callAsync([] { juce::JUCEApplicationBase::quit(); });
}

EXPORT
void updateValues(float gainNew,
    float compAllAttack, float compAllRelease, float compAllThreshold, float compAllRatio,
    bool compAllBypassedNew, bool compAllMuteNew,
    float compLowAttack, float compLowRelease, float compLowThreshold, float compLowRatio,
    bool compLowBypassedNew, bool compLowMuteNew, bool compLowSoloNew,
    float compMidAttack, float compMidRelease, float compMidThreshold, float compMidRatio,
    bool compMidBypassedNew, bool compMidMuteNew, bool compMidSoloNew,
    float compHighAttack, float compHighRelease, float compHighThreshold, float compHighRatio,
    bool compHighBypassedNew, bool compHighMuteNew, bool compHighSoloNew,
    float lowMidCutoff, float midHighCutoff,
    float lowCutFreqNew, float highCutFreqNew, float peakFreqNew,
    float peakGainInDecibelsNew, float peakQualityNew, float lowCutSlopeNew, float highCutSlopeNew) {
    myProcessor.updateValues(gainNew,
        compAllAttack, compAllRelease, compAllThreshold, compAllRatio,
        compAllBypassedNew, compAllMuteNew,
        compLowAttack, compLowRelease, compLowThreshold, compLowRatio,
        compLowBypassedNew, compLowMuteNew, compLowSoloNew,
        compMidAttack, compMidRelease, compMidThreshold, compMidRatio,
        compMidBypassedNew, compMidMuteNew, compMidSoloNew,
        compHighAttack, compHighRelease, compHighThreshold, compHighRatio,
        compHighBypassedNew, compHighMuteNew, compHighSoloNew,
        lowMidCutoff, midHighCutoff,
        lowCutFreqNew, highCutFreqNew, peakFreqNew,
        peakGainInDecibelsNew, peakQualityNew, lowCutSlopeNew, highCutSlopeNew);
}