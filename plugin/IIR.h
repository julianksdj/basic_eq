/*
  ==============================================================================

    IIR.h
    Created: 29 Jan 2024 10:42:00pm
    Author:  Keepin' Soul

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

class IIR {
    
    struct stereoCoef_t {
        float coefL;
        float coefR;
    };
    
    struct biquadCoeffs_t {
        stereoCoef_t a1;
        stereoCoef_t a2;
        stereoCoef_t b0;
        stereoCoef_t b1;
        stereoCoef_t b2;
    };
    
    struct filterParams_t {
        bool state;
        float g; // gain (dB)
        float f;
        float q;
        float fs; //sampling rate
        biquadCoeffs_t iirCoeffs;
        stereoCoef_t z1_i; // x(n-1)
        stereoCoef_t z2_i; // x(n-2)
        stereoCoef_t z1_o; // y(n-1)
        stereoCoef_t z2_o; // y(n-2)
    };
    
public:
    IIR();
    IIR(float fs, float f, int type, bool state);
    void computePeakingCoeffs();
    void computeLowPassCoeffs();
    void computeHighPassCoeffs();
    void runFilter(juce::AudioBuffer<float>* buffer);
    
    filterParams_t iirParams;
};
