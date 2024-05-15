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

Filter::Filter(float fs, float f) {
    biquadFilterParams.fs = fs;
    biquadFilterParams.g = 1.0f; // TODO: default values in header file
    biquadFilterParams.f = f;
    biquadFilterParams.q = 0.7f;
    biquadFilterParams.state = true;
    biquadFilterParams.z1_i.coefL = 0.0f;
    biquadFilterParams.z2_i.coefL = 0.0f;
    biquadFilterParams.z1_o.coefL = 0.0f;
    biquadFilterParams.z2_o.coefL = 0.0f;
    biquadFilterParams.z1_i.coefR = 0.0f;
    biquadFilterParams.z2_i.coefR = 0.0f;
    biquadFilterParams.z1_o.coefR = 0.0f;
    biquadFilterParams.z2_o.coefR = 0.0f;
    computeBiquadCoeffs();
}

void Filter::setGain(float g) {
    biquadFilterParams.g = g;
    computeBiquadCoeffs();
}

void Filter::setCutoff(float f) {
    biquadFilterParams.f = f;
    computeBiquadCoeffs();
}

void Filter::setQ(float q) {
    biquadFilterParams.q = q;
    computeBiquadCoeffs();
}

void Filter::setState(bool on) {
    biquadFilterParams.state = on;
}

bool Filter::getState() {
    return biquadFilterParams.state;
}

void Filter::computeBiquadCoeffs() {
    float w0 = 2 * M_PI * (biquadFilterParams.f / biquadFilterParams.fs);
    float sinw = sinf(w0);
    float cosw = cosf(w0);
    float a = powf(10, biquadFilterParams.g / 40);
    float alpha = 0.5 * sinw / (float) biquadFilterParams.q;
    
    // Peaking filter
    double a0 = (1 + (alpha/a));
    biquadFilterParams.biquadCoeffs.a1.coefL = (-2 * cosw) / a0;
    biquadFilterParams.biquadCoeffs.a1.coefR = biquadFilterParams.biquadCoeffs.a1.coefL;
    biquadFilterParams.biquadCoeffs.a2.coefL = (1 - (alpha/a)) / a0;
    biquadFilterParams.biquadCoeffs.a2.coefR = biquadFilterParams.biquadCoeffs.a2.coefL;
    biquadFilterParams.biquadCoeffs.b0.coefL = (1 + (alpha*a)) / a0;
    biquadFilterParams.biquadCoeffs.b0.coefR = biquadFilterParams.biquadCoeffs.b0.coefL;
    biquadFilterParams.biquadCoeffs.b1.coefL = (-2 * cosw) / a0;
    biquadFilterParams.biquadCoeffs.b1.coefR= biquadFilterParams.biquadCoeffs.b1.coefL;
    biquadFilterParams.biquadCoeffs.b2.coefL = (1 - (alpha*a)) / a0;
    biquadFilterParams.biquadCoeffs.b2.coefR = biquadFilterParams.biquadCoeffs.b2.coefL;
}

// Peaking filter
void Filter::runBiquadFilter(juce::AudioBuffer<float>* buffer) {
    if (biquadFilterParams.state) {
        int size = buffer->getNumSamples();
        float currentInL;
        float currentInR;
        for (unsigned n = 0; n < size; n++) {
            currentInL = buffer->getWritePointer(0)[n];
            currentInR = buffer->getWritePointer(1)[n];
            float currentOutL = (biquadFilterParams.biquadCoeffs.b0.coefL * currentInL
                         + biquadFilterParams.biquadCoeffs.b1.coefL * biquadFilterParams.z1_i.coefL
                         + biquadFilterParams.biquadCoeffs.b2.coefL * biquadFilterParams.z2_i.coefL
                         - biquadFilterParams.biquadCoeffs.a1.coefL * biquadFilterParams.z1_o.coefL
                         - biquadFilterParams.biquadCoeffs.a2.coefL * biquadFilterParams.z2_o.coefL);
            float currentOutR = (biquadFilterParams.biquadCoeffs.b0.coefR * currentInR
                         + biquadFilterParams.biquadCoeffs.b1.coefR * biquadFilterParams.z1_i.coefR
                         + biquadFilterParams.biquadCoeffs.b2.coefR * biquadFilterParams.z2_i.coefR
                         - biquadFilterParams.biquadCoeffs.a1.coefR * biquadFilterParams.z1_o.coefR
                         - biquadFilterParams.biquadCoeffs.a2.coefR * biquadFilterParams.z2_o.coefR);
            buffer->setSample(0, n, currentOutL);
            buffer->setSample(1, n, currentOutR);
            biquadFilterParams.z2_i = biquadFilterParams.z1_i;
            biquadFilterParams.z1_i.coefL = currentInL;
            biquadFilterParams.z1_i.coefR = currentInR;
            biquadFilterParams.z2_o = biquadFilterParams.z1_o;
            biquadFilterParams.z1_o.coefL = currentOutL;
            biquadFilterParams.z1_o.coefR = currentOutR;
        }
    }
}
