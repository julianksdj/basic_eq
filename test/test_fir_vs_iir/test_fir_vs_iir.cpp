#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cmath>

#include "FIR.h"
#include "IIR.h"

struct WAVHeader {
    char chunkID[4] = {'R', 'I', 'F', 'F'};
    uint32_t chunkSize;
    char format[4] = {'W', 'A', 'V', 'E'};
    char subchunk1ID[4] = {'f', 'm', 't', ' '};
    uint32_t subchunk1Size = 16;
    uint16_t audioFormat = 1; // PCM
    uint16_t numChannels = 1;
    uint32_t sampleRate = 48000;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample = 16;
    char subchunk2ID[4] = {'d', 'a', 't', 'a'};
    uint32_t subchunk2Size;

    WAVHeader(int numSamples) {
        byteRate = sampleRate * numChannels * bitsPerSample / 8;
        blockAlign = numChannels * bitsPerSample / 8;
        subchunk2Size = numSamples * numChannels * bitsPerSample / 8;
        chunkSize = 36 + subchunk2Size;
    }
};

int main() {
    const int sampleRate = 48000;
    const int duration = 2; // Duration of the sine sweep in seconds
    const int totalSamples = sampleRate * duration;
    
    // input sweep params
    const float startFrequency = 20.0; 
    const float endFrequency = 20000.0;
    const float PI = juce::MathConstants<float>::pi;
    
    // filter params
    const float filterCut  = 2400.f;
    const float filterQ  = 0.7f;
    const float filterGain  = -10.f;
    const int filterType = 0; //peak filter type

    // test params
    const std::string out_files_path = "test/test_fir_vs_iir/";
    const std::string iir_file_name = out_files_path + "iir.wav";
    const std::string fir_file_name = out_files_path + "fir.wav";
    const std::string sweep_file_name = out_files_path + "sweep.wav";;

    // generate input sine sweep data and write to wav file for reference
    float sweep_wave[totalSamples];
    std::ofstream sweep_file(sweep_file_name, std::ios::binary);
    if (!sweep_file) {
        std::cerr << "Unable to open file for writing: " << sweep_file_name << std::endl;
        return 1;
    }
    std::vector<int16_t> sweep_samples(totalSamples);
    WAVHeader sweep_header(sweep_samples.size());
    sweep_file.write(reinterpret_cast<const char*>(&sweep_header), sizeof(sweep_header));
    float k = (endFrequency - startFrequency) / totalSamples / 2;  // Slope of the linear frequency increase
    for (int i = 0; i < totalSamples; i++) {
        float freq = startFrequency + k * i;
        sweep_wave[i] = 0.1f * sinf(2.0f * PI * freq * i / sampleRate);
        sweep_samples[i] = static_cast<int16_t>(32767.0f * sweep_wave[i]);
    }
    sweep_file.write(reinterpret_cast<const char*>(sweep_samples.data()), sweep_samples.size() * sizeof(int16_t));
    sweep_file.close();

    //compute fir
    FIR fir = FIR(sampleRate, filterCut, filterType, 1);
    fir.firParams.g = filterGain;
    fir.firParams.q = filterQ;
    fir.computePeakingCoeffs();

    //compute iir
    IIR iir = IIR(sampleRate, filterCut, filterType, 1);
    iir.iirParams.g = filterGain;
    iir.iirParams.q = filterQ;
    iir.computePeakingCoeffs();

    //prepare output
    int numChannels = 2;
    int blockSize = 256;
    int numBlocks = totalSamples / blockSize;
    juce::AudioBuffer<float> buffer = juce::AudioBuffer<float>(numChannels, blockSize);
    std::ofstream fir_file(fir_file_name, std::ios::binary);
    std::ofstream iir_file(iir_file_name, std::ios::binary);
    if (!fir_file || !iir_file) {
        std::cerr << "Unable to open file for writing: " << std::endl;
        return 1;
    }

    // process sweep and write result to output wav
    std::vector<int16_t> samples(blockSize);
    WAVHeader fir_header(samples.size() * numBlocks);
    fir_file.write(reinterpret_cast<const char*>(&fir_header), sizeof(fir_header));
    WAVHeader iir_header(samples.size() * numBlocks);
    iir_file.write(reinterpret_cast<const char*>(&iir_header), sizeof(iir_header));
    int sweep_iir_idx = 0; 
    int sweep_fir_idx = 0; 
    for (int i = 0; i < numBlocks; i++) {
        // FIR
        //put sweep in the buffer
        for (int j = 0; j < blockSize; j++) {
            buffer.setSample(0, j, sweep_wave[sweep_fir_idx]);
            sweep_fir_idx++;
        }
        //filter the block
        fir.runFilter(&buffer);
        //convert to int16 and write to wav file
        for (int j = 0; j < blockSize; j++) {
            float x = buffer.getSample(0, j);
            samples[j] = static_cast<int16_t>(32767.0 * x);
        }
        fir_file.write(reinterpret_cast<const char*>(samples.data()), samples.size() * sizeof(int16_t));

        // IIR
        //put sweep in the buffer
        for (int j = 0; j < blockSize; j++) {
            buffer.setSample(0, j, sweep_wave[sweep_iir_idx]);
            sweep_iir_idx++;
        }
        //filter the buffer
        iir.runFilter(&buffer);
        //convert buffer samples to int16 and write to wav file
        for (int j = 0; j < blockSize; j++) {
            float x = buffer.getSample(0, j);
            samples[j] = static_cast<int16_t>(32767.0 * x);
        }
        iir_file.write(reinterpret_cast<const char*>(samples.data()), samples.size() * sizeof(int16_t));
    }

    iir_file.close();
    fir_file.close();

    return 0;

}