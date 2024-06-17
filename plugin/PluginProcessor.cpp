/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicEqAudioProcessor::BasicEqAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
}

BasicEqAudioProcessor::~BasicEqAudioProcessor()
{
}

//==============================================================================
const juce::String BasicEqAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BasicEqAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BasicEqAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BasicEqAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BasicEqAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BasicEqAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BasicEqAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BasicEqAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String BasicEqAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void BasicEqAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void BasicEqAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);

    iirPeakFilter[0] = IIR((float) sampleRate, CUTOFF_1, 0, true);
    iirPeakFilter[1] = IIR((float) sampleRate, CUTOFF_2, 0, true);
    iirPeakFilter[2] = IIR((float) sampleRate, CUTOFF_3, 0, true);
    iirPeakFilter[3] = IIR((float) sampleRate, CUTOFF_4, 0, true);
    iirPeakFilter[4] = IIR((float) sampleRate, CUTOFF_5, 0, true);
    iirPeakFilter[5] = IIR((float) sampleRate, CUTOFF_6, 0, true);
    iirPeakFilter[6] = IIR((float) sampleRate, CUTOFF_7, 0, true);
    iirPeakFilter[7] = IIR((float) sampleRate, CUTOFF_8, 0, true);

    hpFilter = IIR((float) sampleRate, 100.0, 1, false);
    lpFilter = IIR((float) sampleRate, 10000.0, 2, false);
}

void BasicEqAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool BasicEqAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void BasicEqAudioProcessor::updatePeakingGain(int band, float g) {
    iirPeakFilter[band].iirParams.g = g;
    iirPeakFilter[band].computePeakingCoeffs();
}

void BasicEqAudioProcessor::updatePeakingCutoff(int band, float f) {
    iirPeakFilter[band].iirParams.f = f;
    iirPeakFilter[band].computePeakingCoeffs();
}

void BasicEqAudioProcessor::updatePeakingQ(int band, float q) {
    iirPeakFilter[band].iirParams.q = q;
    iirPeakFilter[band].computePeakingCoeffs();
}

void BasicEqAudioProcessor::setPeakingState(int band, bool b) {
    iirPeakFilter[band].iirParams.state = b;
}

void BasicEqAudioProcessor::setHpfState(bool b) {
    hpFilter.iirParams.state = b;
}

void BasicEqAudioProcessor::setLpfState(bool b) {
    lpFilter.iirParams.state = b;
}

void BasicEqAudioProcessor::updateLpfCutoff(float f) {
    lpFilter.iirParams.f = f;
    lpFilter.computeLowPassCoeffs();
}

void BasicEqAudioProcessor::updateHpfCutoff(float f) {
    hpFilter.iirParams.f = f;
    hpFilter.computeHighPassCoeffs();
}

void BasicEqAudioProcessor::updateLpfQ(float q) {
    lpFilter.iirParams.q = q;
    lpFilter.computeLowPassCoeffs();
}

void BasicEqAudioProcessor::updateHpfQ(float q) {
    hpFilter.iirParams.q = q;
    hpFilter.computeHighPassCoeffs();
}

void BasicEqAudioProcessor::updateAlgorithm(int algo) {

    float sampleRateAux = (float) this->getSampleRate();
    switch (algo) {
        case 0: // IIR
            iirPeakFilter[0] = IIR((float) sampleRateAux, CUTOFF_1, 0, true);
            iirPeakFilter[1] = IIR((float) sampleRateAux, CUTOFF_2, 0, true);
            iirPeakFilter[2] = IIR((float) sampleRateAux, CUTOFF_3, 0, true);
            iirPeakFilter[3] = IIR((float) sampleRateAux, CUTOFF_4, 0, true);
            iirPeakFilter[4] = IIR((float) sampleRateAux, CUTOFF_5, 0, true);
            iirPeakFilter[5] = IIR((float) sampleRateAux, CUTOFF_6, 0, true);
            iirPeakFilter[6] = IIR((float) sampleRateAux, CUTOFF_7, 0, true);
            iirPeakFilter[7] = IIR((float) sampleRateAux, CUTOFF_8, 0, true);
            hpFilter = IIR((float) sampleRateAux, 100.0, 1, false);
            lpFilter = IIR((float) sampleRateAux, 10000.0, 2, false);
        break;
        default:
        break;
    
    }
}

void BasicEqAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
    
    for (int i = 0; i < NUMBER_OF_BANDS; i++) {
        iirPeakFilter[i].runFilter(&buffer);
    }

    lpFilter.runFilter(&buffer);
    hpFilter.runFilter(&buffer);

}

//==============================================================================
bool BasicEqAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BasicEqAudioProcessor::createEditor()
{
    return new BasicEqAudioProcessorEditor (*this);
}

//==============================================================================
void BasicEqAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::ignoreUnused (destData);
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void BasicEqAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    juce::ignoreUnused (data, sizeInBytes);
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BasicEqAudioProcessor();
}
