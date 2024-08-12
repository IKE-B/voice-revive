/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioProcessorGraphCompGainAudioProcessor::AudioProcessorGraphCompGainAudioProcessor()
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
    using namespace Params;
    const auto& params = GetParams();

    auto floatHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
        {
            param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(paramName)));
            jassert(param != nullptr);
        };
    auto choiceHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
        {
            param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(params.at(paramName)));
            jassert(param != nullptr);
        };
    auto boolHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
        {
            param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(paramName)));
            jassert(param != nullptr);
        };

    // inialize all pointers to the corresponding apvts elements
    floatHelper(gainProcess.gainParam, Names::Gain);

    // lowBandComp
    floatHelper(compProcess.lowBandComp.attack, Names::Attack_Low_Band);
    floatHelper(compProcess.lowBandComp.release, Names::Release_Low_Band);
    floatHelper(compProcess.lowBandComp.threshold, Names::Threshold_Low_Band);
    choiceHelper(compProcess.lowBandComp.ratio, Names::Ratio_Low_Band);
    boolHelper(compProcess.lowBandComp.bypassed, Names::Bypassed_Low_Band);
    boolHelper(compProcess.lowBandComp.mute, Names::Mute_Low_Band);
    boolHelper(compProcess.lowBandComp.solo, Names::Solo_Low_Band);

    //midBandComp
    floatHelper(compProcess.midBandComp.attack, Names::Attack_Mid_Band);
    floatHelper(compProcess.midBandComp.release, Names::Release_Mid_Band);
    floatHelper(compProcess.midBandComp.threshold, Names::Threshold_Mid_Band);
    choiceHelper(compProcess.midBandComp.ratio, Names::Ratio_Mid_Band);
    boolHelper(compProcess.midBandComp.bypassed, Names::Bypassed_Mid_Band);
    boolHelper(compProcess.midBandComp.mute, Names::Mute_Mid_Band);
    boolHelper(compProcess.midBandComp.solo, Names::Solo_Mid_Band);
    
    //highBandComp
    floatHelper(compProcess.highBandComp.attack, Names::Attack_High_Band);
    floatHelper(compProcess.highBandComp.release, Names::Release_High_Band);
    floatHelper(compProcess.highBandComp.threshold, Names::Threshold_High_Band);
    choiceHelper(compProcess.highBandComp.ratio, Names::Ratio_High_Band);
    boolHelper(compProcess.highBandComp.bypassed, Names::Bypassed_High_Band);
    boolHelper(compProcess.highBandComp.mute, Names::Mute_High_Band);
    boolHelper(compProcess.highBandComp.solo, Names::Solo_High_Band);


    floatHelper(compProcess.lowMidCrossover, Names::Low_Mid_Crossover_Freq);
    floatHelper(compProcess.midHighCrossover, Names::Mid_High_Crossover_Freq);

    compProcess.LP1.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    compProcess.HP1.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    compProcess.AP2.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
    compProcess.LP2.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    compProcess.HP2.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    
    //all Compressor
    floatHelper(compAllProcess.comp.attack, Names::Attack_All);
    floatHelper(compAllProcess.comp.release, Names::Release_All);
    floatHelper(compAllProcess.comp.threshold, Names::Threshold_All);
    choiceHelper(compAllProcess.comp.ratio, Names::Ratio_All);
    boolHelper(compAllProcess.comp.bypassed, Names::Bypassed_All);
    boolHelper(compAllProcess.comp.mute, Names::Mute_All);
}

AudioProcessorGraphCompGainAudioProcessor::~AudioProcessorGraphCompGainAudioProcessor()
{
}

//==============================================================================
const juce::String AudioProcessorGraphCompGainAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioProcessorGraphCompGainAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioProcessorGraphCompGainAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioProcessorGraphCompGainAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioProcessorGraphCompGainAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioProcessorGraphCompGainAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioProcessorGraphCompGainAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioProcessorGraphCompGainAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AudioProcessorGraphCompGainAudioProcessor::getProgramName (int index)
{
    return {};
}

void AudioProcessorGraphCompGainAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AudioProcessorGraphCompGainAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    gainProcess.prepareToPlay(sampleRate, samplesPerBlock);
    compProcess.prepareToPlay(sampleRate, samplesPerBlock);
    compAllProcess.prepareToPlay(sampleRate, samplesPerBlock);
}

void AudioProcessorGraphCompGainAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AudioProcessorGraphCompGainAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void AudioProcessorGraphCompGainAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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

    compProcess.processBlock(buffer, midiMessages);
    compAllProcess.processBlock(buffer, midiMessages);
    gainProcess.processBlock(buffer, midiMessages);
}

//==============================================================================
bool AudioProcessorGraphCompGainAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioProcessorGraphCompGainAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void AudioProcessorGraphCompGainAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void AudioProcessorGraphCompGainAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout AudioProcessorGraphCompGainAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout layout;

    using namespace juce;

    // define GUI Layout for Gain, minValue is -24 and maxValue is +24
    auto gainRange = NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f);
    layout.add(std::make_unique<AudioParameterFloat>("Gain", "Gain", gainRange, 0));

    // define GUI Layout for Threshold, minValue is -60 maxValue is 12 and it's linear with steps of 1, defaultValue is 0
    auto thresholdRange = NormalisableRange<float>(-60, 12, 1, 1);
    layout.add(std::make_unique<AudioParameterFloat>("Threshold Low Band", "Threshold Low Band", thresholdRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>("Threshold Mid Band", "Threshold Mid Band", thresholdRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>("Threshold High Band", "Threshold High Band", thresholdRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>("Threshold All", "Threshold All", thresholdRange, 0));

    // define GUI Layout for Attack, minValue is 5, maxValue is 500 and it's linear with steps of 1, defaultValue is 50
    auto attackReleaseRange = NormalisableRange<float>(5, 500, 1, 1);
    layout.add(std::make_unique<AudioParameterFloat>("Attack Low Band", "Attack Low Band", attackReleaseRange, 50));
    layout.add(std::make_unique<AudioParameterFloat>("Attack Mid Band", "Attack Mid Band", attackReleaseRange, 50));
    layout.add(std::make_unique<AudioParameterFloat>("Attack High Band", "Attack High Band", attackReleaseRange, 50));
    layout.add(std::make_unique<AudioParameterFloat>("Attack All", "Attack All", attackReleaseRange, 50));

    // define GUI Layout for Release with same values as for Attack
    layout.add(std::make_unique<AudioParameterFloat>("Release Low Band", "Release Low Band", attackReleaseRange, 250));
    layout.add(std::make_unique<AudioParameterFloat>("Release Mid Band", "Release Mid Band", attackReleaseRange, 250));
    layout.add(std::make_unique<AudioParameterFloat>("Release High Band", "Release High Band", attackReleaseRange, 250));
    layout.add(std::make_unique<AudioParameterFloat>("Release All", "Release All", attackReleaseRange, 250));

    // define GUI Layout for Ratio
    auto choices = std::vector<double>{ 1, 1.5, 2, 3, 4, 5, 6, 7, 8, 10, 15, 20, 50, 100 };
    juce::StringArray sa;
    for (auto choice : choices)
    {
        sa.add(juce::String(choice, 1));
    }
    layout.add(std::make_unique<AudioParameterChoice>("Ratio Low Band", "Ratio Low Band", sa, 3));
    layout.add(std::make_unique<AudioParameterChoice>("Ratio Mid Band", "Ratio Mid Band", sa, 3));
    layout.add(std::make_unique<AudioParameterChoice>("Ratio High Band", "Ratio High Band", sa, 3));
    layout.add(std::make_unique<AudioParameterChoice>("Ratio All", "Ratio All", sa, 3));

    // add Bypass
    layout.add(std::make_unique<AudioParameterBool>("Bypassed Low Band", "Bypassed Low Band", false));
    layout.add(std::make_unique<AudioParameterBool>("Bypassed Mid Band", "Bypassed Mid Band", false));
    layout.add(std::make_unique<AudioParameterBool>("Bypassed High Band", "Bypassed High Band", false));
    layout.add(std::make_unique<AudioParameterBool>("Bypassed All", "Bypassed All", false));

    // add Solo
    layout.add(std::make_unique<AudioParameterBool>("Solo Low Band", "Solo Low Band", false));
    layout.add(std::make_unique<AudioParameterBool>("Solo Mid Band", "Solo Mid Band", false));
    layout.add(std::make_unique<AudioParameterBool>("Solo High Band", "Solo High Band", false));

    // add Mute
    layout.add(std::make_unique<AudioParameterBool>("Mute Low Band", "Mute Low Band", false));
    layout.add(std::make_unique<AudioParameterBool>("Mute Mid Band", "Mute Mid Band", false));
    layout.add(std::make_unique<AudioParameterBool>("Mute High Band", "Mute High Band", false));
    layout.add(std::make_unique<AudioParameterBool>("Mute All", "Mute All", false));

    layout.add(std::make_unique<AudioParameterFloat>("Low Mid Crossover Freq", "Low Mid Crossover Freq", NormalisableRange<float>(20, 999, 1, 1), 400));
    layout.add(std::make_unique<AudioParameterFloat>("Mid High Crossover Freq", "Mid High Crossover Freq", NormalisableRange<float>(1000, 20000, 1, 1), 2000));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioProcessorGraphCompGainAudioProcessor();
}
