/*
  ==============================================================================

    GlobalFunction.h
    Created: 18 Oct 2024 2:12:48pm
    Author:  maxig

  ==============================================================================
*/

#pragma once
#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H

// Deklaration der Funktion zum Starten der Modulation
void startModulation();

// Deklaration der Funktion zum Beenden der JUCE-Anwendung
void stopModulation();

// Deklaration der Funktion zum Aktualisieren der Werte
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
    float peakGainInDecibelsNew, float peakQualityNew, float lowCutSlopeNew, float highCutSlopeNew);

#endif // GLOBALFUNCTIONS_H
