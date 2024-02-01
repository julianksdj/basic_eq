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
    
    struct biquadCoeffs_t {
        float a0;
        float a1;
        float a2;
        float b0;
        float b1;
        float b2;
    };
    
    struct filterParams_t {
        float fs; //sampling rate
        biquadCoeffs_t biquadCoeffs;
        float z1_i; // x(n-1)
        float z2_i; // x(n-2)
        float z1_o; // y(n-1)
        float z2_o; // y(n-2)
    };
    
public:
    Filter();
    Filter(float fs);
    void computeBiquadCoeffs(float g, float f, float q);
    void runBiquadFilter(float* channelData, int size);
    
private:
    filterParams_t biquadFilterParams;
};
