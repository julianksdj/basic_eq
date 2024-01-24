/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#define NUMBER_OF_BANDS 8

//==============================================================================
/**
*/
class Basic_eqAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                      public juce::Slider::Listener
{
public:
    Basic_eqAudioProcessorEditor (Basic_eqAudioProcessor&);
    ~Basic_eqAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
        
    // Controls
    void sliderValueChanged (juce::Slider *slider) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Basic_eqAudioProcessor& audioProcessor;
    
    // Controls
    juce::Slider band_sld[NUMBER_OF_BANDS];
    
    // Labels
    juce::Label band_lbl[NUMBER_OF_BANDS];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Basic_eqAudioProcessorEditor)
};
