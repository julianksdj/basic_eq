/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Basic_eqAudioProcessor::Basic_eqAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

Basic_eqAudioProcessor::~Basic_eqAudioProcessor()
{
}

//==============================================================================
const juce::String Basic_eqAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Basic_eqAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Basic_eqAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Basic_eqAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Basic_eqAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Basic_eqAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Basic_eqAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Basic_eqAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Basic_eqAudioProcessor::getProgramName (int index)
{
    return {};
}

void Basic_eqAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Basic_eqAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    parametricFilter[0] = Filter(sampleRate, CUTOFF_1, 0, true);
    parametricFilter[1] = Filter(sampleRate, CUTOFF_2, 0, true);
    parametricFilter[2] = Filter(sampleRate, CUTOFF_3, 0, true);
    parametricFilter[3] = Filter(sampleRate, CUTOFF_4, 0, true);
    parametricFilter[4] = Filter(sampleRate, CUTOFF_5, 0, true);
    parametricFilter[5] = Filter(sampleRate, CUTOFF_6, 0, true);
    parametricFilter[6] = Filter(sampleRate, CUTOFF_7, 0, true);
    parametricFilter[7] = Filter(sampleRate, CUTOFF_8, 0, true);

    hpFilter = Filter(sampleRate, 100.0, 1, false);
    lpFilter = Filter(sampleRate, 10000.0, 2, false);
}

void Basic_eqAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Basic_eqAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Basic_eqAudioProcessor::updatePeakingGain(int band, float g) {
    parametricFilter[band].biquadFilterParams.g = g;
    parametricFilter[band].computePeakingCoeffs();
}

void Basic_eqAudioProcessor::updatePeakingCutoff(int band, float f) {
    parametricFilter[band].biquadFilterParams.f = f;
    parametricFilter[band].computePeakingCoeffs();
}

void Basic_eqAudioProcessor::updatePeakingQ(int band, float q) {
    parametricFilter[band].biquadFilterParams.q = q;
    parametricFilter[band].computePeakingCoeffs();
}

void Basic_eqAudioProcessor::setPeakingState(int band, bool b) {
    parametricFilter[band].biquadFilterParams.state = b;
}

void Basic_eqAudioProcessor::setHpfState(bool b) {
    hpFilter.biquadFilterParams.state = b;
}

void Basic_eqAudioProcessor::setLpfState(bool b) {
    lpFilter.biquadFilterParams.state = b;
}

void Basic_eqAudioProcessor::updateLpfCutoff(float f) {
    lpFilter.biquadFilterParams.f = f;
    lpFilter.computeLowPassCoeffs();
}

void Basic_eqAudioProcessor::updateHpfCutoff(float f) {
    hpFilter.biquadFilterParams.f = f;
    hpFilter.computeHighPassCoeffs();
}

void Basic_eqAudioProcessor::updateLpfQ(float q) {
    lpFilter.biquadFilterParams.q = q;
    lpFilter.computeLowPassCoeffs();
}

void Basic_eqAudioProcessor::updateHpfQ(float q) {
    hpFilter.biquadFilterParams.q = q;
    hpFilter.computeHighPassCoeffs();
}

void Basic_eqAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
    
    for (int i = 0; i < NUMBER_OF_BANDS; i++) {
        parametricFilter[i].runBiquadFilter(&buffer);
    }
    lpFilter.runBiquadFilter(&buffer);
    hpFilter.runBiquadFilter(&buffer);

}

//==============================================================================
bool Basic_eqAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Basic_eqAudioProcessor::createEditor()
{
    return new Basic_eqAudioProcessorEditor (*this);
}

//==============================================================================
void Basic_eqAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Basic_eqAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Basic_eqAudioProcessor();
}
