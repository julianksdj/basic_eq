/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class Basic_eqAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                      public juce::Slider::Listener,
                                      public juce::Button::Listener
{
public:
    Basic_eqAudioProcessorEditor (Basic_eqAudioProcessor&);
    ~Basic_eqAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
        
    // Controls
    void sliderValueChanged(juce::Slider *slider) override;
    void buttonClicked(juce::Button*) override;
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Basic_eqAudioProcessor& audioProcessor;
    
    // Controls
    juce::Slider band_sld[NUMBER_OF_BANDS];
    juce::Slider band_knb[NUMBER_OF_BANDS];
    juce::Slider lpf_freq_knb;
    juce::Slider lpf_q_knb;
    juce::Slider hpf_freq_knb;
    juce::Slider hpf_q_knb;
    juce::ToggleButton band_tgl[NUMBER_OF_BANDS];
    juce::ToggleButton lpf_tgl;
    juce::ToggleButton hpf_tgl;
    
    // Labels
    juce::Label band_lbl[NUMBER_OF_BANDS];
    juce::Label lpf_lbl;
    juce::Label hpf_lbl;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Basic_eqAudioProcessorEditor)
};
