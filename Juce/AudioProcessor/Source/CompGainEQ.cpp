/*
  ==============================================================================

    CompGainEQ.cpp
    Created: 3 Sep 2024 4:26:59pm
    Author:  Simon

  ==============================================================================
*/

#include "CompGainEQ.h"

CompGainEQ::CompGainEQ()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
    
}

CompGainEQ::~CompGainEQ()
{
}

//==============================================================================
const juce::String CompGainEQ::getName() const
{
    return "JucePlugin Comp Gain and EQ";
}

bool CompGainEQ::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool CompGainEQ::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool CompGainEQ::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double CompGainEQ::getTailLengthSeconds() const
{
    return 0.0;
}

int CompGainEQ::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int CompGainEQ::getCurrentProgram()
{
    return 0;
}

void CompGainEQ::setCurrentProgram(int index)
{
}

const juce::String CompGainEQ::getProgramName(int index)
{
    return {};
}

void CompGainEQ::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void CompGainEQ::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // TODO:
    //gainProcess.prepareToPlay(sampleRate, samplesPerBlock);
    //compProcess.prepareToPlay(sampleRate, samplesPerBlock);
    //compAllProcess.prepareToPlay(sampleRate, samplesPerBlock);
}

void CompGainEQ::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CompGainEQ::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
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

void CompGainEQ::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // TODO
    //compProcess.processBlock(buffer, midiMessages);
    //compAllProcess.processBlock(buffer, midiMessages);
    //gainProcess.processBlock(buffer, midiMessages);
}

void CompGainEQ::updateValues(const std::unordered_map<std::string, float>& floatValues, const std::unordered_map<std::string, bool>& boolValues)
{
    // set the 4 different Compressors
    // compressor for all with float and bool values
    compressorAll.setAttack(getWithDefault(floatValues, std::string("Attack All"), 50.0f));
    compressorAll.setRelease(getWithDefault(floatValues, std::string("Release All"), 50.0f));
    compressorAll.setThreshold(getWithDefault(floatValues, std::string("Threshold All"), 50.0f));
    compressorAll.setRatio(getWithDefault(floatValues, std::string("Ratio All"), 50.0f));

    compAllBypassed = getWithDefault(boolValues, std::string("Bypassed All"), false);
    compAllMute = getWithDefault(boolValues, std::string("Mute All"), false);

    // compressor for low band
    compressorLow.setAttack(getWithDefault(floatValues, std::string("Attack Low"), 50.0f));
    compressorLow.setRelease(getWithDefault(floatValues, std::string("Release Low"), 50.0f));
    compressorLow.setThreshold(getWithDefault(floatValues, std::string("Threshold Low"), 50.0f));
    compressorLow.setRatio(getWithDefault(floatValues, std::string("Ratio Low"), 50.0f));

    compLowBypassed = getWithDefault(boolValues, std::string("Bypassed Low"), false);
    compLowMute = getWithDefault(boolValues, std::string("Mute Low"), false);
    compLowSolo = getWithDefault(boolValues, std::string("Solo Low"), false);

    // compressor for mid band
    compressorMid.setAttack(getWithDefault(floatValues, std::string("Attack Mid"), 50.0f));
    compressorMid.setRelease(getWithDefault(floatValues, std::string("Release Mid"), 50.0f));
    compressorMid.setThreshold(getWithDefault(floatValues, std::string("Threshold Mid"), 50.0f));
    compressorMid.setRatio(getWithDefault(floatValues, std::string("Ratio Mid"), 50.0f));

    compMidBypassed = getWithDefault(boolValues, std::string("Bypassed Mid"), false);
    compMidMute = getWithDefault(boolValues, std::string("Mute Mid"), false);
    compMidSolo = getWithDefault(boolValues, std::string("Solo Mid"), false);

    // compressor for high band
    compressorHigh.setAttack(getWithDefault(floatValues, std::string("Attack High"), 50.0f));
    compressorHigh.setRelease(getWithDefault(floatValues, std::string("Release High"), 50.0f));
    compressorHigh.setThreshold(getWithDefault(floatValues, std::string("Threshold High"), 50.0f));
    compressorHigh.setRatio(getWithDefault(floatValues, std::string("Ratio High"), 50.0f));

    compHighBypassed = getWithDefault(boolValues, std::string("Bypassed High"), false);
    compHighMute = getWithDefault(boolValues, std::string("Mute High"), false);
    compHighSolo = getWithDefault(boolValues, std::string("Solo High"), false);

    // set the 3 bands
    auto lowMidCutoff = getWithDefault(floatValues, std::string("lowMidCutoff"), 200.0f);
    LP1.setCutoffFrequency(lowMidCutoff);
    HP1.setCutoffFrequency(lowMidCutoff);

    auto midHighCutoff = getWithDefault(floatValues, std::string("highMidCutoff"), 2000.0f);
    AP2.setCutoffFrequency(midHighCutoff);
    LP2.setCutoffFrequency(midHighCutoff);
    HP2.setCutoffFrequency(midHighCutoff);

    // gain
    gain.setGainDecibels(getWithDefault(floatValues, std::string("Gain"), 0.0f));

}

//==============================================================================
void CompGainEQ::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream mos(destData, true);
}

void CompGainEQ::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}