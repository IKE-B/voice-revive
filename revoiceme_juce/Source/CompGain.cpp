/*
  ==============================================================================

    CompGain.cpp
    Created: 21 Oct 2024 5:08:49pm
    Author:  Simon

  ==============================================================================
*/

#include "CompGain.h"

CompGain::CompGain()
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

CompGain::~CompGain()
{
}

//==============================================================================
/*Hinzugefügt*/
bool CompGain::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* CompGain::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}


void CompGain::getStateInformation(juce::MemoryBlock& destData)
{

}

void CompGain::setStateInformation(const void* data, int sizeInBytes)
{

}

const juce::String CompGain::getName() const
{
    return "Juce AudioProcessor for Comp Gain and EQ";
}

bool CompGain::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool CompGain::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool CompGain::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double CompGain::getTailLengthSeconds() const
{
    return 0.0;
}

int CompGain::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int CompGain::getCurrentProgram()
{
    return 0;
}

void CompGain::setCurrentProgram(int index)
{
}

const juce::String CompGain::getProgramName(int index)
{
    return {};
}

void CompGain::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================


void CompGain::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // CompAll
    prepareToPlayCompAll(sampleRate, samplesPerBlock);

    // Comp Mult Band
    prepareToPlayCompMultBand(sampleRate, samplesPerBlock);

    // Gain
    prepareToPlayGain(sampleRate, samplesPerBlock);

    // EQ
    prepareToPlayEQ(sampleRate, samplesPerBlock);
}


void CompGain::prepareToPlayCompAll(double sampleRate, int samplesPerBlock)
{
    // prepare Compressor for compressing the AudioInput
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());
    spec.sampleRate = sampleRate;

    compressorAll.prepare(spec);
}

void CompGain::prepareToPlayCompMultBand(double sampleRate, int samplesPerBlock)
{
    // prepare Compressor for compressing the AudioInput
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());
    spec.sampleRate = sampleRate;

    compressorLow.prepare(spec);
    compressorMid.prepare(spec);
    compressorHigh.prepare(spec);

    LP1.prepare(spec);
    HP1.prepare(spec);

    AP2.prepare(spec);

    LP2.prepare(spec);
    HP2.prepare(spec);

    for (auto& buffer : filterBuffers)
    {
        buffer.setSize(static_cast<int>(spec.numChannels), samplesPerBlock);
    }
}

void CompGain::prepareToPlayGain(double sampleRate, int samplesPerBlock)
{
    // prepare Compressor for compressing the AudioInput
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());
    spec.sampleRate = sampleRate;

    gain.prepare(spec);

    gain.setRampDurationSeconds(0.05);
}

void CompGain::prepareToPlayEQ(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;

    leftChain.prepare(spec);
    rightChain.prepare(spec);
}

void CompGain::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CompGain::isBusesLayoutSupported(const BusesLayout& layouts) const
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

void CompGain::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    if (compAllBypassed && compLowBypassed && compMidBypassed && compHighBypassed)
    {
        buffer.clear();
        return; // Beendet das Processing hier
    }


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

    // compressor All
    processBlockCompAll(buffer, midiMessages);

    // compressor multiBand
    processBlockCompMultBand(buffer, midiMessages);

    // gain
    applyGain(buffer, gain);

    // EQ
    processBlockEQ(buffer, midiMessages);
}

void CompGain::processBlockCompAll(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (!compAllMute)
    {
        compressorAll.process(getCompressorContext(buffer, compAllBypassed));
    }
}

juce::dsp::ProcessContextReplacing<float> CompGain::getCompressorContext(juce::AudioBuffer<float>& buffer, bool isBypass)
{
    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);

    context.isBypassed = isBypass;

    return context;
}

void CompGain::processBlockCompMultBand(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    splitBands(buffer);

    compressorLow.process(getCompressorContext(filterBuffers[0], compLowBypassed));
    compressorMid.process(getCompressorContext(filterBuffers[1], compMidBypassed));
    compressorHigh.process(getCompressorContext(filterBuffers[2], compHighBypassed));


    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();

    buffer.clear();

    auto addFilterBand = [nc = numChannels, ns = numSamples](auto& inputBuffer, const auto& source)
        {
            for (auto i = 0; i < nc; i++)
            {
                inputBuffer.addFrom(i, 0, source, i, 0, ns);
            }
        };

    bool bandsAreSoloed = compLowSolo || compMidSolo || compHighSolo;

    if (bandsAreSoloed)
    {
        if (compLowSolo)
        {
            addFilterBand(buffer, filterBuffers[0]);
        }
        else if (compMidSolo)
        {
            addFilterBand(buffer, filterBuffers[1]);
        }
        else if (compHighSolo)
        {
            addFilterBand(buffer, filterBuffers[2]);
        }
    }
    else
    {
        if (!compLowMute)
        {
            addFilterBand(buffer, filterBuffers[0]);
        }
        else if (!compMidMute)
        {
            addFilterBand(buffer, filterBuffers[1]);
        }
        else if (!compHighMute)
        {
            addFilterBand(buffer, filterBuffers[2]);
        }
    }
}

void CompGain::splitBands(const juce::AudioBuffer<float>& inputBuffer)
{
    // split the buffer in 2 filterBuffers
    for (auto& fb : filterBuffers)
    {
        fb = inputBuffer;
    }

    auto fb0Block = juce::dsp::AudioBlock<float>(filterBuffers[0]);
    auto fb1Block = juce::dsp::AudioBlock<float>(filterBuffers[1]);
    auto fb2Block = juce::dsp::AudioBlock<float>(filterBuffers[2]);

    auto fb0Ctx = juce::dsp::ProcessContextReplacing<float>(fb0Block);
    auto fb1Ctx = juce::dsp::ProcessContextReplacing<float>(fb1Block);
    auto fb2Ctx = juce::dsp::ProcessContextReplacing<float>(fb2Block);

    LP1.process(fb0Ctx);
    AP2.process(fb0Ctx);

    HP1.process(fb1Ctx);
    filterBuffers[2] = filterBuffers[1];
    LP2.process(fb1Ctx);

    HP2.process(fb2Ctx);
}

void CompGain::processBlockEQ(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::dsp::AudioBlock<float> block(buffer);

    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);
}

void CompGain::updateValues(float gainNew,
    float compAllAttack, float compAllRelease, float compAllThreshold, float compAllRatio,
    bool compAllBypassedNew, bool compAllMuteNew,
    float compLowAttack, float compLowRelease, float compLowThreshold, float compLowRatio,
    bool compLowBypassedNew, bool compLowMuteNew, bool compLowSoloNew,
    float compMidAttack, float compMidRelease, float compMidThreshold, float compMidRatio,
    bool compMidBypassedNew, bool compMidMuteNew, bool compMidSoloNew,
    float compHighAttack, float compHighRelease, float compHighThreshold, float compHighRatio,
    bool compHighBypassedNew, bool compHighMuteNew, bool compHighSoloNew,
    float lowMidCutoff, float midHighCutoff,
    float lowCutFreqNew, float highCutFreqNew, float peakFreqNew,
    float peakGainInDecibelsNew, float peakQualityNew, float lowCutSlopeNew, float highCutSlopeNew)
{
    // set the 4 different Compressors
    // compressor for all with float and bool values
    compressorAll.setAttack(compAllAttack);
    compressorAll.setRelease(compAllRelease);
    compressorAll.setThreshold(compAllThreshold);
    compressorAll.setRatio(compAllRatio);

    compAllBypassed = compAllBypassedNew;
    compAllMute = compAllMuteNew;

    // compressor for low band
    compressorLow.setAttack(compLowAttack);
    compressorLow.setRelease(compLowRelease);
    compressorLow.setThreshold(compLowThreshold);
    compressorLow.setRatio(compLowRatio);

    compLowBypassed = compLowBypassedNew;
    compLowMute = compLowMuteNew;
    compLowSolo = compLowSoloNew;

    // compressor for mid band
    // compressor for mid band
    compressorMid.setAttack(compMidAttack);
    compressorMid.setRelease(compMidRelease);
    compressorMid.setThreshold(compMidThreshold);
    compressorMid.setRatio(compMidRatio);

    compMidBypassed = compMidBypassedNew;
    compMidMute = compMidMuteNew;
    compMidSolo = compMidSoloNew;

    // compressor for high band
    compressorHigh.setAttack(compHighAttack);
    compressorHigh.setRelease(compHighRelease);
    compressorHigh.setThreshold(compHighThreshold);
    compressorHigh.setRatio(compHighRatio);

    compHighBypassed = compHighBypassedNew;
    compHighMute = compHighMuteNew;
    compHighSolo = compHighSoloNew;

    // set the 3 bands
    LP1.setCutoffFrequency(lowMidCutoff);
    HP1.setCutoffFrequency(lowMidCutoff);

    AP2.setCutoffFrequency(midHighCutoff);
    LP2.setCutoffFrequency(midHighCutoff);
    HP2.setCutoffFrequency(midHighCutoff);

    // gain
    gain.setGainDecibels(gainNew);

    // EQ
    updateFilters(lowCutFreqNew, highCutFreqNew, peakFreqNew, peakGainInDecibelsNew, peakQualityNew, lowCutSlopeNew, highCutSlopeNew);

}

ChainSettings CompGain::getChainSettings(float lowCutFreqNew, float highCutFreqNew, float peakFreqNew,
    float peakGainInDecibelsNew, float peakQualityNew, float lowCutSlopeNew, float highCutSlopeNew)
{
    ChainSettings settings;

    settings.lowCutFreq = lowCutFreqNew;
    settings.highCutFreq = highCutFreqNew;
    settings.peakFreq = peakFreqNew;
    settings.peakGainInDecibels = peakGainInDecibelsNew;
    settings.peakQuality = peakQualityNew;
    settings.lowCutSlope = lowCutSlopeNew;
    settings.highCutSlope = highCutSlopeNew;

    return settings;
}

void CompGain::updateFilters(float lowCutFreqNew, float highCutFreqNew, float peakFreqNew,
    float peakGainInDecibelsNew, float peakQualityNew, float lowCutSlopeNew, float highCutSlopeNew)
{
    ChainSettings chainSettings = getChainSettings(lowCutFreqNew, highCutFreqNew, peakFreqNew, peakGainInDecibelsNew, peakQualityNew, lowCutSlopeNew, highCutSlopeNew);

    updateLowCutFilters(chainSettings);
    updatePeakFilter(chainSettings);
    updateHighCutFilters(chainSettings);
}

void CompGain::updateHighCutFilters(const ChainSettings& chainSettings)
{
    auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.highCutFreq,
        getSampleRate(),
        2 * (chainSettings.highCutSlope + 1));

    auto& leftHighCut = leftChain.get<ChainPositions::HighCut>();
    auto& rightHighCut = rightChain.get<ChainPositions::HighCut>();

    updateCutFilter(leftHighCut, highCutCoefficients, chainSettings.highCutSlope);
    updateCutFilter(rightHighCut, highCutCoefficients, chainSettings.highCutSlope);
}

void CompGain::updateLowCutFilters(const ChainSettings& chainSettings)
{
    auto cutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.lowCutFreq,
        getSampleRate(),
        2 * (chainSettings.lowCutSlope + 1));

    auto& leftLowCut = leftChain.get<ChainPositions::LowCut>();
    auto& rightLowCut = rightChain.get<ChainPositions::LowCut>();

    updateCutFilter(leftLowCut, cutCoefficients, chainSettings.lowCutSlope);
    updateCutFilter(rightLowCut, cutCoefficients, chainSettings.lowCutSlope);
}

void CompGain::updateCoefficients(Coefficients& old, const Coefficients& replacements)
{
    *old = *replacements;
}

void CompGain::updatePeakFilter(const ChainSettings& chainSettings)
{
    auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
        chainSettings.peakFreq,
        chainSettings.peakQuality,
        juce::Decibels::decibelsToGain(chainSettings.peakGainInDecibels));

    updateCoefficients(leftChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
    updateCoefficients(rightChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
}