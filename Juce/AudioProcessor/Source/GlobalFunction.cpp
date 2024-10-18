/*
  ==============================================================================

    GlobalFunction.cpp
    Created: 18 Oct 2024 2:13:05pm
    Author:  maxig

  ==============================================================================
*/

#include "GlobalFunction.h"

#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "GlobalFunctions.h"
#include <include_juce_audio_processors.cpp>
#include "../../Source/CompGainEQ.h"

CompGainEQ myProcessor;

void startModulation()
{
    double sampleRate = 44100.0;
    int samplesPerBlock = 512;
    myProcessor.prepareToPlay(sampleRate, samplesPerBlock);

    juce::AudioBuffer<float> buffer(2, samplesPerBlock);
    juce::MidiBuffer midiBuffer;

    myProcessor.processBlock(buffer, midiBuffer);
}

void stopModulation()
{
    juce::JUCEApplicationBase::quit();
    //juce::MessageManager::callAsync([] { juce::JUCEApplicationBase::quit(); });
}


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