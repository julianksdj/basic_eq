/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Filter.h"

#define NUMBER_OF_BANDS 8
#define CUTOFF_1 20.0
#define CUTOFF_2 100.0
#define CUTOFF_3 500.0
#define CUTOFF_4 1000.0
#define CUTOFF_5 2000.0
#define CUTOFF_6 5000.0
#define CUTOFF_7 10000.0
#define CUTOFF_8 15000.0



//==============================================================================
/**
*/
class Basic_eqAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Basic_eqAudioProcessor();
    ~Basic_eqAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void updateFilterGain(int band, float g);
    void updateFilterCutoff(int band, float f);
    void updateFilterQ(int band, float q);
    void setFilterState(int band, bool b);

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Basic_eqAudioProcessor)
    
    Filter parametricFilter[NUMBER_OF_BANDS];    
};
