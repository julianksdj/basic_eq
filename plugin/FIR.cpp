/*
  ==============================================================================

    FIR.cpp
    Created: 29 Jan 2024 10:42:00pm
    Author:  Keepin' Soul

  ==============================================================================
*/

#include "FIR.h"
#include "math.h"
#include <cstdlib>

FIR::FIR() {
}

FIR::FIR(float fs, float f, int type, bool state) {
    firParams.fs = fs;
    firParams.g = 1.0f; // TODO: default values in header file
    firParams.f = f;
    firParams.q = 0.7f;
    firParams.state = state;

    buffer_index = 0;
    
    switch (type) {
        case 0:
            computePeakingCoeffs();
            break;
        default:
            break;
    }
}

void FIR::computePeakingCoeffs() {
    // TODO: generate impulse response
    for (int i = 0; i < N_TAPS; i++) {
        firParams.firCoeffs[i] = 0.f;
    }
}

void FIR::runFilter(juce::AudioBuffer<float>* buffer) {
    if (firParams.state) {
        int size = buffer->getNumSamples();
        for (int n = 0; n < size; n++) {
            float* currentIn[2] = {buffer->getWritePointer(0), buffer->getWritePointer(1)};
            // TODO: convolution
        }
    }
}
