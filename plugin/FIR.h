/*
  ==============================================================================

    FIR.h
    Created: 29 Jan 2024 10:42:00pm
    Author:  Keepin' Soul

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#define N_TAPS 1001

class FIR {
    
    struct filterParams_t {
        bool state;
        float g; // gain (dB)
        float f;
        float q;
        float fs; //sampling rate
        float firCoeffs[N_TAPS];
    };
    
public:
    FIR();
    FIR(float fs, float f, int type, bool state);
    void computePeakingCoeffs();
    void runFilter(juce::AudioBuffer<float>* buffer);
    
    filterParams_t firParams;

private:
    int buffer_index;
};
