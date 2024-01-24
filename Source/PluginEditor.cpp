/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define MARGIN 10
#define BAND_W 40
#define SLIDER_H 300
#define LABEL_W BAND_W
#define LABEL_H 40
#define BAND_SEPARATION (BAND_W + MARGIN)
#define LABEL_POS_H ((2 * MARGIN) + SLIDER_H)
#define WINDOW_W ((NUMBER_OF_BANDS * (BAND_W + MARGIN)) + MARGIN)
#define WINDOW_H (SLIDER_H + LABEL_H + (2 * MARGIN))

//==============================================================================
Basic_eqAudioProcessorEditor::Basic_eqAudioProcessorEditor(Basic_eqAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(WINDOW_W, WINDOW_H);

    for (int i = 0; i < NUMBER_OF_BANDS; i++) {
        addAndMakeVisible(band_sld[i]);
        band_sld[i].setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        band_sld[i].setTextBoxStyle(juce::Slider::TextBoxBelow, false, BAND_W, 30);
        band_sld[i].addListener(this);
        band_sld[i].setRange(-48.0, 10.0);
        
        addAndMakeVisible(band_lbl[i]);
        juce::String lbl_str = "f" + juce::String(i + 1);
        band_lbl[i].setText(lbl_str, juce::dontSendNotification);
        band_lbl[i].setJustificationType(juce::Justification::centred);
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
    int y = 10;
    for (int i = 0; i < NUMBER_OF_BANDS; i++) {
        band_sld[i].setBounds(x, MARGIN, BAND_W, SLIDER_H);
        band_lbl[i].setBounds(x, LABEL_POS_H, LABEL_W, LABEL_H);
        x += BAND_SEPARATION;
    }
}

void Basic_eqAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    for (int i = 0; i < NUMBER_OF_BANDS; i++) {
        if(slider == &band_sld[i])
        {
            DBG("band_sld[" << i << "]="<< band_sld[i].getValue());
        }
    }
}
