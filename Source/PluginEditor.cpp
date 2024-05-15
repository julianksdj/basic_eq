/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define MARGIN 10
#define BAND_W 50
#define SLIDER_H 300
#define TOGGLE_W BAND_W
#define TOGGLE_H 40
#define LABEL_W BAND_W
#define LABEL_H 40
#define QKNOB_H 80
#define BAND_SEPARATION (BAND_W + MARGIN)
#define QKNOB_POS_H ((2 * MARGIN) + SLIDER_H)
#define TOGGLE_POS_H (QKNOB_POS_H + QKNOB_H + MARGIN)
#define LABEL_POS_H (TOGGLE_POS_H + TOGGLE_H + MARGIN)
#define WINDOW_W ((NUMBER_OF_BANDS * (BAND_W + MARGIN)) + MARGIN + 2 * (BAND_W + MARGIN))
#define WINDOW_H (SLIDER_H + QKNOB_H + TOGGLE_H + LABEL_H + (4 * MARGIN))

//==============================================================================
Basic_eqAudioProcessorEditor::Basic_eqAudioProcessorEditor(Basic_eqAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(WINDOW_W, WINDOW_H);
    
    addAndMakeVisible(hpf_freq_knb);
    hpf_freq_knb.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    hpf_freq_knb.setTextBoxStyle(juce::Slider::TextBoxBelow, false, BAND_W, 30);
    hpf_freq_knb.addListener(this);
    hpf_freq_knb.setRange(20.0, 20000.0);
    hpf_freq_knb.setValue(20.0);
    
    addAndMakeVisible(hpf_q_knb);
    hpf_q_knb.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    hpf_q_knb.setTextBoxStyle(juce::Slider::TextBoxBelow, false, BAND_W, 30);
    hpf_q_knb.addListener(this);
    hpf_q_knb.setRange(0.1, 2.0);
    hpf_q_knb.setValue(0.7);
    
    addAndMakeVisible(hpf_tgl);
    hpf_tgl.setToggleState(true, juce::dontSendNotification);
    hpf_tgl.addListener(this);
    
    addAndMakeVisible(hpf_lbl);
    juce::String lbl_str = "HPF";
    hpf_lbl.setJustificationType(juce::Justification::centred);
    hpf_lbl.setText(lbl_str, juce::dontSendNotification);

    addAndMakeVisible(lpf_freq_knb);
    lpf_freq_knb.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    lpf_freq_knb.setTextBoxStyle(juce::Slider::TextBoxBelow, false, BAND_W, 30);
    lpf_freq_knb.addListener(this);
    lpf_freq_knb.setRange(20.0, 20000.0);
    lpf_freq_knb.setValue(20000.0);
    
    addAndMakeVisible(lpf_q_knb);
    lpf_q_knb.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    lpf_q_knb.setTextBoxStyle(juce::Slider::TextBoxBelow, false, BAND_W, 30);
    lpf_q_knb.addListener(this);
    lpf_q_knb.setRange(0.1, 2.0);
    lpf_q_knb.setValue(0.7);
    
    addAndMakeVisible(lpf_tgl);
    lpf_tgl.setToggleState(true, juce::dontSendNotification);
    lpf_tgl.addListener(this);
    
    addAndMakeVisible(lpf_lbl);
    lbl_str = "LPF";
    lpf_lbl.setJustificationType(juce::Justification::centred);
    lpf_lbl.setText(lbl_str, juce::dontSendNotification);
    
    for (int i = 0; i < NUMBER_OF_BANDS; i++) {
        addAndMakeVisible(band_sld[i]);
        band_sld[i].setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        band_sld[i].setTextBoxStyle(juce::Slider::TextBoxBelow, false, BAND_W, 30);
        band_sld[i].addListener(this);
        band_sld[i].setRange(-12.0, 12.0);
        band_sld[i].setValue(0.0); // TODO: default values in common header
        
        addAndMakeVisible(band_knb[i]);
        band_knb[i].setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        band_knb[i].setTextBoxStyle(juce::Slider::TextBoxBelow, false, BAND_W, 30);
        band_knb[i].addListener(this);
        band_knb[i].setRange(0.1, 2.0);
        band_knb[i].setValue(0.7);

        
        addAndMakeVisible(band_tgl[i]);
        band_tgl[i].setToggleState(true, juce::dontSendNotification);
        band_tgl[i].addListener(this);
        
        addAndMakeVisible(band_lbl[i]);
        lbl_str = "f" + juce::String(i + 1); // TODO: print cutoff freqs
        band_lbl[i].setJustificationType(juce::Justification::centred);
        band_lbl[i].setText(lbl_str, juce::dontSendNotification);
    }
    
    
}

Basic_eqAudioProcessorEditor::~Basic_eqAudioProcessorEditor()
{
}

//==============================================================================
void Basic_eqAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
}

void Basic_eqAudioProcessorEditor::resized()
{
    int x = 10;
    
    hpf_freq_knb.setBounds(x, MARGIN, BAND_W, QKNOB_H);
    hpf_q_knb.setBounds(x, QKNOB_H + MARGIN, BAND_W, QKNOB_H);
    hpf_tgl.setBounds(x, 2 * (QKNOB_H + MARGIN), TOGGLE_W, TOGGLE_H);
    hpf_lbl.setBounds(x, 2 * (QKNOB_H + MARGIN) + TOGGLE_H, LABEL_W, LABEL_H);
    x += BAND_SEPARATION;
    
    for (int i = 0; i < NUMBER_OF_BANDS; i++) {
        band_sld[i].setBounds(x, MARGIN, BAND_W, SLIDER_H);
        band_knb[i].setBounds(x, QKNOB_POS_H, BAND_W, QKNOB_H);
        band_tgl[i].setBounds(x, TOGGLE_POS_H, TOGGLE_W, TOGGLE_H);
        band_lbl[i].setBounds(x, LABEL_POS_H, LABEL_W, LABEL_H);
        x += BAND_SEPARATION;
    }
    
    lpf_freq_knb.setBounds(x, MARGIN, BAND_W, QKNOB_H);
    lpf_q_knb.setBounds(x, QKNOB_H + MARGIN, BAND_W, QKNOB_H);
    lpf_tgl.setBounds(x, 2 * (QKNOB_H + MARGIN), TOGGLE_W, TOGGLE_H);
    lpf_lbl.setBounds(x, 2 * (QKNOB_H + MARGIN) + TOGGLE_H, LABEL_W, LABEL_H);
    x += BAND_SEPARATION;
}

void Basic_eqAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    for (int i = 0; i < NUMBER_OF_BANDS; i++) {
        if (slider == &band_sld[i])
        {
            DBG("band_sld[" << i << "]="<< band_sld[i].getValue());
            audioProcessor.updateFilterGain(i, band_sld[i].getValue());
        }
        if (slider == &band_knb[i])
        {
            DBG("band_knb[" << i << "]="<< band_knb[i].getValue());
            audioProcessor.updateFilterQ(i, band_knb[i].getValue());
        }
    }
}

void Basic_eqAudioProcessorEditor::buttonClicked(juce::Button *button) {
    for (int i = 0; i < NUMBER_OF_BANDS; i++) {
        if (button == &band_tgl[i]) {
            audioProcessor.setFilterState(i, button->getToggleState());
            juce::String str = (band_tgl[i].getToggleState() == true ? "band on" : "band off");
            DBG("band_tgl[" << i << "]="<< str);
        }
    }
}
