/*
  ==============================================================================

    IIR.cpp
    Created: 29 Jan 2024 10:42:00pm
    Author:  Keepin' Soul

  ==============================================================================
*/

#include "IIR.h"

IIR::IIR() {
}

IIR::IIR(float fs, float f, int type, bool state) {
    iirParams.fs = fs;
    iirParams.g = 1.0f; // TODO: default values in header file
    iirParams.f = f;
    iirParams.q = 0.7f;
    iirParams.state = state;
    iirParams.z1_i.coefL = 0.0f;
    iirParams.z2_i.coefL = 0.0f;
    iirParams.z1_o.coefL = 0.0f;
    iirParams.z2_o.coefL = 0.0f;
    iirParams.z1_i.coefR = 0.0f;
    iirParams.z2_i.coefR = 0.0f;
    iirParams.z1_o.coefR = 0.0f;
    iirParams.z2_o.coefR = 0.0f;
    
    switch (type) {
        case 0:
            computePeakingCoeffs();
            break;
        case 1:
            computeHighPassCoeffs();
            break;
        case 2:
            computeLowPassCoeffs();
            break;
        default:
            break;
    }
}

void IIR::computePeakingCoeffs() {
    float w0 = 2 * juce::MathConstants<float>::pi * (iirParams.f / iirParams.fs);
    float sinw = sinf(w0);
    float cosw = cosf(w0);
    float a = powf(10, iirParams.g / 40.f);
    float alpha = 0.5f * sinw / (float) iirParams.q;
    
    float a0 = (1 + (alpha/a));
    iirParams.iirCoeffs.a1.coefL = (-2.f * cosw) / a0;
    iirParams.iirCoeffs.a1.coefR = iirParams.iirCoeffs.a1.coefL;
    iirParams.iirCoeffs.a2.coefL = (1.f - (alpha/a)) / a0;
    iirParams.iirCoeffs.a2.coefR = iirParams.iirCoeffs.a2.coefL;
    iirParams.iirCoeffs.b0.coefL = (1.f + (alpha*a)) / a0;
    iirParams.iirCoeffs.b0.coefR = iirParams.iirCoeffs.b0.coefL;
    iirParams.iirCoeffs.b1.coefL = (-2.f * cosw) / a0;
    iirParams.iirCoeffs.b1.coefR= iirParams.iirCoeffs.b1.coefL;
    iirParams.iirCoeffs.b2.coefL = (1.f - (alpha*a)) / a0;
    iirParams.iirCoeffs.b2.coefR = iirParams.iirCoeffs.b2.coefL;
}

void IIR::computeLowPassCoeffs() {
    float w0 = 2 * juce::MathConstants<float>::pi * (iirParams.f / iirParams.fs);
    float sinw = sinf(w0);
    float cosw = cosf(w0);
    float alpha = 0.5f * sinw / (float) iirParams.q;
    
    float a0 = 1 + alpha;
    iirParams.iirCoeffs.a1.coefL = (-2.f * cosw) / a0;
    iirParams.iirCoeffs.a1.coefR = iirParams.iirCoeffs.a1.coefL;
    iirParams.iirCoeffs.a2.coefL = (1.f - (alpha)) / a0;
    iirParams.iirCoeffs.a2.coefR = iirParams.iirCoeffs.a2.coefL;
    iirParams.iirCoeffs.b0.coefL = (1.f - cosw) / (2.f * a0);
    iirParams.iirCoeffs.b0.coefR = iirParams.iirCoeffs.b0.coefL;
    iirParams.iirCoeffs.b1.coefL = (1.f - cosw) / a0;
    iirParams.iirCoeffs.b1.coefR= iirParams.iirCoeffs.b1.coefL;
    iirParams.iirCoeffs.b2.coefL = (1.f - cosw) / (2.f * a0);
    iirParams.iirCoeffs.b2.coefR = iirParams.iirCoeffs.b2.coefL;
}

void IIR::computeHighPassCoeffs() {
    float w0 = 2 * juce::MathConstants<float>::pi * (iirParams.f / iirParams.fs);
    float sinw = sinf(w0);
    float cosw = cosf(w0);
    float alpha = 0.5f * sinw / (float) iirParams.q;
    
    float a0 = 1 + alpha;
    iirParams.iirCoeffs.a1.coefL = (-2.f * cosw) / a0;
    iirParams.iirCoeffs.a1.coefR = iirParams.iirCoeffs.a1.coefL;
    iirParams.iirCoeffs.a2.coefL = (1.f - alpha) / a0;
    iirParams.iirCoeffs.a2.coefR = iirParams.iirCoeffs.a2.coefL;
    iirParams.iirCoeffs.b0.coefL = (1.f + cosw) / (2.f * a0);
    iirParams.iirCoeffs.b0.coefR = iirParams.iirCoeffs.b0.coefL;
    iirParams.iirCoeffs.b1.coefL = (-1.f - cosw) / a0;
    iirParams.iirCoeffs.b1.coefR= iirParams.iirCoeffs.b1.coefL;
    iirParams.iirCoeffs.b2.coefL = (1.f + cosw) / (2.f * a0);
    iirParams.iirCoeffs.b2.coefR = iirParams.iirCoeffs.b2.coefL;
}

void IIR::runFilter(juce::AudioBuffer<float>* buffer) {
    if (iirParams.state) {
        int size = buffer->getNumSamples();
        float currentInL;
        float currentInR;
        for (int n = 0; n < size; n++) {
            currentInL = buffer->getWritePointer(0)[n];
            currentInR = buffer->getWritePointer(1)[n];
            float currentOutL = (iirParams.iirCoeffs.b0.coefL * currentInL
                         + iirParams.iirCoeffs.b1.coefL * iirParams.z1_i.coefL
                         + iirParams.iirCoeffs.b2.coefL * iirParams.z2_i.coefL
                         - iirParams.iirCoeffs.a1.coefL * iirParams.z1_o.coefL
                         - iirParams.iirCoeffs.a2.coefL * iirParams.z2_o.coefL);
            float currentOutR = (iirParams.iirCoeffs.b0.coefR * currentInR
                         + iirParams.iirCoeffs.b1.coefR * iirParams.z1_i.coefR
                         + iirParams.iirCoeffs.b2.coefR * iirParams.z2_i.coefR
                         - iirParams.iirCoeffs.a1.coefR * iirParams.z1_o.coefR
                         - iirParams.iirCoeffs.a2.coefR * iirParams.z2_o.coefR);
            buffer->setSample(0, n, currentOutL);
            buffer->setSample(1, n, currentOutR);
            iirParams.z2_i.coefL = iirParams.z1_i.coefL;
            iirParams.z2_i.coefR = iirParams.z1_i.coefR;
            iirParams.z1_i.coefL = currentInL;
            iirParams.z1_i.coefR = currentInR;
            iirParams.z2_o.coefL = iirParams.z1_o.coefL;
            iirParams.z2_o.coefR = iirParams.z1_o.coefR;
            iirParams.z1_o.coefL = currentOutL;
            iirParams.z1_o.coefR = currentOutR;
        }
    }
}
