/*
  ==============================================================================

    CompGainEQ.cpp
    Created: 3 Sep 2024 4:26:59pm
    Author:  Simon

  ==============================================================================
*/

#include "CompGainEQ.h"




void CompGainEQ::startModulation()
{
    if (!isInitialized) {
        DBG("Das Programm ist noch nicht bereit!");
        return;
    }

    // Hier f�hrst du den eigentlichen Start der Modulation durch
    DBG("Modulation startet jetzt...");
    isProcessing = true;
    gain.setGainDecibels(-60.0f); 

    
    const float rampDurationSeconds = 2.0f;
    gain.setRampDurationSeconds(rampDurationSeconds);

    gain.setGainDecibels(0.0f);
}


void CompGainEQ::stopModulation()
{
    
    gain.setRampDurationSeconds(0.0f); 
    gain.setGainDecibels(-60.0f); 
    compAllBypassed = true;     
    compLowBypassed = true;     
    compMidBypassed = true;    
    compHighBypassed = true;
    isProcessing = false;

    juce::JUCEApplicationBase::quit();

}






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
    // Initialize the audio device manager (2 input channels, 2 output channels)
    deviceManager.initialise(1, 1, nullptr, true, {}, nullptr);

    // Log available devices
    logAvailableDevices();

    // Start the device and check if it is working
    if (auto* currentDevice = deviceManager.getCurrentAudioDevice())
    {
        juce::String deviceName = currentDevice->getName();
        juce::Logger::writeToLog("Current audio device: " + deviceName);

        if (currentDevice->isOpen())
        {
            juce::Logger::writeToLog("Audio device is open and ready to use.");
        }
        else
        {
            juce::Logger::writeToLog("Failed to open audio device.");
        }
    }
    else
    {
        juce::Logger::writeToLog("No audio device is currently initialized.");
    }
}

CompGainEQ::~CompGainEQ()
{
}

// for testing log the Audio Devices
void CompGainEQ::logAvailableDevices()
{
    // Get the list of available device types (e.g., CoreAudio, ASIO, WASAPI, etc.)
    juce::AudioIODeviceType* deviceType = deviceManager.getCurrentDeviceTypeObject();

    if (deviceType != nullptr)
    {
        // Refresh the list of devices (needed before calling getDeviceNames)
        deviceType->scanForDevices();

        // Get the list of input and output device names
        juce::StringArray outputDevices = deviceType->getDeviceNames(false); // false = output devices
        juce::StringArray inputDevices = deviceType->getDeviceNames(true);   // true = input devices

        // Log the available output devices
        juce::Logger::writeToLog("Available Output Devices:");
        for (const auto& device : outputDevices)
        {
            juce::Logger::writeToLog("  - " + device);
        }

        // Log the available input devices
        juce::Logger::writeToLog("Available Input Devices:");
        for (const auto& device : inputDevices)
        {
            juce::Logger::writeToLog("  - " + device);
        }
    }
    else
    {
        juce::Logger::writeToLog("No audio device type found.");
    }
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
    // 
    // CompAll
    prepareToPlayCompAll(sampleRate, samplesPerBlock);

    // Comp Mult Band
    prepareToPlayCompMultBand(sampleRate, samplesPerBlock);

    // Gain
    prepareToPlayGain(sampleRate, samplesPerBlock);

    // EQ
    prepareToPlayEQ(sampleRate, samplesPerBlock);
    isInitialized = true;
    
}

void CompGainEQ::prepareToPlayCompAll(double sampleRate, int samplesPerBlock)
{
    // prepare Compressor for compressing the AudioInput
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());
    spec.sampleRate = sampleRate;

    compressorAll.prepare(spec);
}

void CompGainEQ::prepareToPlayCompMultBand(double sampleRate, int samplesPerBlock)
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

void CompGainEQ::prepareToPlayGain(double sampleRate, int samplesPerBlock)
{
    // prepare Compressor for compressing the AudioInput
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());
    spec.sampleRate = sampleRate;

    gain.prepare(spec);

    gain.setRampDurationSeconds(0.05);
}

void CompGainEQ::prepareToPlayEQ(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;

    leftChain.prepare(spec);
    rightChain.prepare(spec);
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

    if (!isProcessing || (compAllBypassed && compLowBypassed && compMidBypassed && compHighBypassed))
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

    // TODO
    // maybe we need an update:
    // updateValues(floatValues, boolValues);
    
    // compressor All
    processBlockCompAll(buffer, midiMessages);

    // compressor multiBand
    processBlockCompMultBand(buffer, midiMessages);

    // gain
    applyGain(buffer, gain);

    // EQ
    processBlockEQ(buffer, midiMessages);
}

void CompGainEQ::processBlockCompAll(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (!compAllMute)
    {
        compressorAll.process(getCompressorContext(buffer, compAllBypassed));
    }
}

juce::dsp::ProcessContextReplacing<float> CompGainEQ::getCompressorContext(juce::AudioBuffer<float>& buffer, bool isBypass)
{
    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);

    context.isBypassed = isBypass;

    return context;
}

void CompGainEQ::processBlockCompMultBand(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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

void CompGainEQ::splitBands(const juce::AudioBuffer<float>& inputBuffer)
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

void CompGainEQ::processBlockEQ(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::dsp::AudioBlock<float> block(buffer);

    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);
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

    // EQ
    updateFilters(floatValues);

}

ChainSettings CompGainEQ::getChainSettings(const std::unordered_map<std::string, float>& floatValues)
{
    ChainSettings settings;

    settings.lowCutFreq = getWithDefault(floatValues, std::string("Low Cut Freq"), 0.0f);
    settings.highCutFreq = getWithDefault(floatValues, std::string("High Cut Freq"), 0.0f);
    settings.peakFreq = getWithDefault(floatValues, std::string("Peak Freq"), 0.0f);
    settings.peakGainInDecibels = getWithDefault(floatValues, std::string("Peak Gain in Decibels"), 0.0f);
    settings.peakQuality = getWithDefault(floatValues, std::string("Peak Quality"), 0.0f);
    settings.lowCutSlope = getWithDefault(floatValues, std::string("Low Cut Slope"), 0.0f);
    settings.highCutSlope = getWithDefault(floatValues, std::string("High Cut Slope"), 0.0f);

    return settings;
}

void CompGainEQ::updateFilters(const std::unordered_map<std::string, float>& floatValues)
{
    auto chainSettings = getChainSettings(floatValues);

    updateLowCutFilters(chainSettings);
    updatePeakFilter(chainSettings);
    updateHighCutFilters(chainSettings);
}

void CompGainEQ::updateHighCutFilters(const ChainSettings& chainSettings)
{
    auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.highCutFreq,
        getSampleRate(),
        2 * (chainSettings.highCutSlope + 1));

    auto& leftHighCut = leftChain.get<ChainPositions::HighCut>();
    auto& rightHighCut = rightChain.get<ChainPositions::HighCut>();

    updateCutFilter(leftHighCut, highCutCoefficients, chainSettings.highCutSlope);
    updateCutFilter(rightHighCut, highCutCoefficients, chainSettings.highCutSlope);
}

void CompGainEQ::updateLowCutFilters(const ChainSettings& chainSettings)
{
    auto cutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.lowCutFreq,
        getSampleRate(),
        2 * (chainSettings.lowCutSlope + 1));

    auto& leftLowCut = leftChain.get<ChainPositions::LowCut>();
    auto& rightLowCut = rightChain.get<ChainPositions::LowCut>();

    updateCutFilter(leftLowCut, cutCoefficients, chainSettings.lowCutSlope);
    updateCutFilter(rightLowCut, cutCoefficients, chainSettings.lowCutSlope);
}

void CompGainEQ::updateCoefficients(Coefficients& old, const Coefficients& replacements)
{
    *old = *replacements;
}

void CompGainEQ::updatePeakFilter(const ChainSettings& chainSettings)
{
    auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
        chainSettings.peakFreq,
        chainSettings.peakQuality,
        juce::Decibels::decibelsToGain(chainSettings.peakGainInDecibels));

    updateCoefficients(leftChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
    updateCoefficients(rightChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
}