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
#define ARCH_BOX_H 60
#define ARCH_BOX_W 100
#define BAND_SEPARATION (BAND_W + MARGIN)
#define QKNOB_POS_H ((2 * MARGIN) + SLIDER_H)
#define TOGGLE_POS_H (QKNOB_POS_H + QKNOB_H + MARGIN)
#define LABEL_POS_H (TOGGLE_POS_H + TOGGLE_H + MARGIN)
#define ARCH_BOX_POS_H (LABEL_POS_H + LABEL_H + MARGIN)
#define WINDOW_W ((NUMBER_OF_BANDS * (BAND_W + MARGIN)) + MARGIN + 2 * (BAND_W + MARGIN))
#define WINDOW_H (SLIDER_H + QKNOB_H + TOGGLE_H + LABEL_H + ARCH_BOX_H + (5 * MARGIN))


#define IIR_ID 1
#define FIR_ID 2

#define LPF_TEXT ("LPF")
#define HPF_TEXT ("HPF")
#define IIR_TEXT ("IIR")
#define FIR_TEXT ("FIR")

//==============================================================================
BasicEqAudioProcessorEditor::BasicEqAudioProcessorEditor(BasicEqAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    juce::ignoreUnused (audioProcessor);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(WINDOW_W, WINDOW_H);
    
    addAndMakeVisible(hpf_freq_knb);
    hpf_freq_knb.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    hpf_freq_knb.setTextBoxStyle(juce::Slider::TextBoxBelow, false, BAND_W, 30);
    hpf_freq_knb.addListener(this);
    hpf_freq_knb.setRange(20.0, 20000.0);
    hpf_freq_knb.setValue(100.0);
    hpf_freq_knb.setSkewFactor(0.2, false);
    
    addAndMakeVisible(hpf_q_knb);
    hpf_q_knb.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    hpf_q_knb.setTextBoxStyle(juce::Slider::TextBoxBelow, false, BAND_W, 30);
    hpf_q_knb.addListener(this);
    hpf_q_knb.setRange(0.1, 2.0);
    hpf_q_knb.setValue(0.7);
    
    addAndMakeVisible(hpf_tgl);
    hpf_tgl.setToggleState(false, juce::dontSendNotification);
    hpf_tgl.addListener(this);
    
    addAndMakeVisible(hpf_lbl);
    juce::String lbl_str = HPF_TEXT;
    hpf_lbl.setJustificationType(juce::Justification::centred);
    hpf_lbl.setText(lbl_str, juce::dontSendNotification);

    addAndMakeVisible(lpf_freq_knb);
    lpf_freq_knb.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    lpf_freq_knb.setTextBoxStyle(juce::Slider::TextBoxBelow, false, BAND_W, 30);
    lpf_freq_knb.addListener(this);
    lpf_freq_knb.setRange(20.0, 20000.0);
    lpf_freq_knb.setValue(10000.0);
    lpf_freq_knb.setSkewFactor(0.2, false);
    
    addAndMakeVisible(lpf_q_knb);
    lpf_q_knb.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    lpf_q_knb.setTextBoxStyle(juce::Slider::TextBoxBelow, false, BAND_W, 30);
    lpf_q_knb.addListener(this);
    lpf_q_knb.setRange(0.1, 2.0);
    lpf_q_knb.setValue(0.7);
    
    addAndMakeVisible(lpf_tgl);
    lpf_tgl.setToggleState(false, juce::dontSendNotification);
    lpf_tgl.addListener(this);
    
    addAndMakeVisible(lpf_lbl);
    lbl_str = LPF_TEXT;
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

    // addAndMakeVisible(arch_box);
    // arch_box.addItem(IIR_TEXT, IIR_ID);
    // arch_box.addItem(FIR_TEXT, FIR_ID);
    // arch_box.setSelectedId(0);
    // arch_box.setJustificationType(juce::Justification::centred);
    // arch_box.addListener(this);
    
    
}

BasicEqAudioProcessorEditor::~BasicEqAudioProcessorEditor()
{
}

//==============================================================================
void BasicEqAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
}

void BasicEqAudioProcessorEditor::resized()
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

    arch_box.setBounds(0, ARCH_BOX_POS_H, ARCH_BOX_W, ARCH_BOX_H);
}

void BasicEqAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    for (int i = 0; i < NUMBER_OF_BANDS; i++) {
        if (slider == &band_sld[i])
        {
            DBG("band_sld[" << i << "]="<< band_sld[i].getValue());
            audioProcessor.updatePeakingGain(i, (float) band_sld[i].getValue());
        }
        if (slider == &band_knb[i])
        {
            DBG("band_knb[" << i << "]="<< band_knb[i].getValue());
            audioProcessor.updatePeakingQ(i, (float) band_knb[i].getValue());
        }
    }
    if (slider == &lpf_freq_knb)
    {
        DBG("lpf_freq_knb" << lpf_freq_knb.getValue());
        audioProcessor.updateLpfCutoff((float) lpf_freq_knb.getValue());
    }
    else if (slider == &hpf_freq_knb)
    {
        DBG("hpf_freq_knb" << hpf_freq_knb.getValue());
        audioProcessor.updateHpfCutoff((float) hpf_freq_knb.getValue());
    }
    else if (slider == &lpf_q_knb)
    {
        DBG("lpf_q_knb" << lpf_q_knb.getValue());
        audioProcessor.updateLpfQ((float) lpf_q_knb.getValue());
    }
    else if (slider == &hpf_q_knb)
    {
        DBG("hpf_q_knb" << hpf_q_knb.getValue());
        audioProcessor.updateHpfQ((float) hpf_q_knb.getValue());
    }
}

void BasicEqAudioProcessorEditor::buttonClicked(juce::Button *button) {
    juce::String str;
    
    for (int i = 0; i < NUMBER_OF_BANDS; i++) {
        if (button == &band_tgl[i]) {
            audioProcessor.setPeakingState(i, button->getToggleState());
            str = (band_tgl[i].getToggleState() == true ? "band on" : "band off");
            DBG("band_tgl[" << i << "]="<< str);
        }
    }
    
    if (button == &hpf_tgl) {
        audioProcessor.setHpfState(button->getToggleState());
        str = (hpf_tgl.getToggleState() == true ? "hpf on" : "hpf off");
        DBG("hpf_tgl" << str);
    }
    
    if (button == &lpf_tgl) {
        audioProcessor.setLpfState(button->getToggleState());
        str = (lpf_tgl.getToggleState() == true ? "lpf on" : "lpf off");
        DBG("lpf_tgl" << str);
    }
        
}

void BasicEqAudioProcessorEditor::comboBoxChanged(juce::ComboBox *comboBox) {
    if (comboBox->getSelectedId() == 1) {
        audioProcessor.updateAlgorithm(0);
        DBG("IIR");
    } else if (comboBox->getSelectedId() == 2) {
        audioProcessor.updateAlgorithm(1);
        DBG("FIR");
    }
}