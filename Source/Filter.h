/*
  ==============================================================================

    Filter.h
    Created: 29 Jan 2024 10:42:00pm
    Author:  Keepin' Soul

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class Filter {
    
    struct stereoCoef_t {
        float coefL;
        float coefR;
    };
    
    struct biquadCoeffs_t {
        stereoCoef_t a0;
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
        biquadCoeffs_t biquadCoeffs;
        stereoCoef_t z1_i; // x(n-1)
        stereoCoef_t z2_i; // x(n-2)
        stereoCoef_t z1_o; // y(n-1)
        stereoCoef_t z2_o; // y(n-2)
    };
    
public:
    Filter();
    Filter(float fs, float f);
    void setGain(float g);
    void setCutoff(float f);
    void setQ(float q);
    void setState(bool on);
    bool getState();
    void computeBiquadCoeffs();
    void runBiquadFilter(juce::AudioBuffer<float>* buffer);
    
private:
    filterParams_t biquadFilterParams;
};
