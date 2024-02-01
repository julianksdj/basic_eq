/*
  ==============================================================================

    Filter.cpp
    Created: 29 Jan 2024 10:42:00pm
    Author:  Keepin' Soul

  ==============================================================================
*/

#include "Filter.h"

Filter::Filter() {
}

Filter::Filter(float fs) {
    biquadFilterParams.fs = fs;
    biquadFilterParams.z1_i = 0.0f;
    biquadFilterParams.z2_i = 0.0f;
    biquadFilterParams.z1_o = 0.0f;
    biquadFilterParams.z2_o = 0.0f;
    computeBiquadCoeffs(0.0f, 1000.0f, 2.0f);
}

void Filter::computeBiquadCoeffs(float g, float f, float q) {
    float w0 = 2 * M_PI * (f / biquadFilterParams.fs);
    float sinw = sinf(w0);
    float cosw = cosf(w0);
    float a = powf(10, g / 40);
    float alpha = 0.5 * sinw / q;
    
    // Peaking filter
    double a0 = 1 + (alpha/a);
    biquadFilterParams.biquadCoeffs.a1 = (-2 * cosw) / a0;
    biquadFilterParams.biquadCoeffs.a2 = (1 - (alpha/a)) / a0;
    biquadFilterParams.biquadCoeffs.b0 = (1 + (alpha*a)) / a0;
    biquadFilterParams.biquadCoeffs.b1 = (-2 * cosw) / a0;
    biquadFilterParams.biquadCoeffs.b2 = (1 - (alpha*a)) / a0;
}

void Filter::runBiquadFilter(float* buffer, int size) {
    for (unsigned n = 0; n < size; n++) {
        // Peaking filter
        biquadFilterParams.z2_i = biquadFilterParams.z1_i;
        biquadFilterParams.z1_i = buffer[n];
        buffer[n] = biquadFilterParams.biquadCoeffs.b0 * buffer[n] +
        biquadFilterParams.biquadCoeffs.b1 * biquadFilterParams.z1_i +
        biquadFilterParams.biquadCoeffs.b2 * biquadFilterParams.z2_i -
        biquadFilterParams.biquadCoeffs.a1 * biquadFilterParams.z1_o -
        biquadFilterParams.biquadCoeffs.a2 * biquadFilterParams.z2_o;
        biquadFilterParams.z2_o = biquadFilterParams.z1_o;
        biquadFilterParams.z1_o = buffer[n];
    }
}
