#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
    : startTab {deviceManager},
    configTab {chainSettingsMember,
              gain,
              compressorAll,
              compAllMute,
              compAllBypassed,
              lowBandComp,
              midBandComp,
              highBandComp,
              lowMidCrossover,
              midHighCrossover},
    tabs {juce::TabbedButtonBar::TabsAtTop}
{
    tabs.addTab("Startseite",
                juce::Colours::lightgrey,
                &startTab,
                true);
    tabs.addTab("Audiogeraet", 
                juce::Colours::grey,
                new juce::AudioDeviceSelectorComponent(deviceManager,
                                                       0,     // minimum input channels
                                                       256,   // maximum input channels
                                                       0,     // minimum output channels
                                                       256,   // maximum output channels
                                                       false, // ability to select midi inputs
                                                       false, // ability to select midi output device
                                                       false, // treat channels as stereo pairs
                                                       false), // hide advanced options)
                true);
    tabs.addTab("Konfiguration",
                juce::Colours::lightgrey,
                &configTab,
                true);

    addAndMakeVisible(tabs);

    // Make sure you set the size of the component after
    // you add any child components.
    setSize (640, 480);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels(2, 2);
    }
    gain.setGainDecibels(0.f);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()

    prepareEQ(samplesPerBlockExpected, sampleRate);

    //Gain
    prepareGain(samplesPerBlockExpected, sampleRate);

    //compAll
    prepareCompAll(samplesPerBlockExpected, sampleRate);

    //compMult
    prepareCompMult(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock() 
    auto *device = deviceManager.getCurrentAudioDevice();
    auto activeInputChannels = device->getActiveInputChannels();
    auto activeOutputChannels = device->getActiveOutputChannels();
    auto maxInputChannels = activeInputChannels.countNumberOfSetBits();
    auto maxOutputChannels = activeOutputChannels.countNumberOfSetBits();

    // Create a temporary AudioBuffer to consolidate input channels
    juce::AudioBuffer<float> tempBuffer(maxInputChannels, bufferToFill.numSamples);

    // Copy input channels to the tempBuffer
    for (int channel = 0; channel < maxInputChannels; ++channel)
    {
        if (activeInputChannels[channel])
        {
            // Copy data from input to tempBuffer
            tempBuffer.copyFrom(channel, 0, *bufferToFill.buffer, channel, bufferToFill.startSample, bufferToFill.numSamples);
        }
        else
        {
            // If no input, clear this channel
            tempBuffer.clear(channel, 0, bufferToFill.numSamples);
        }
    }

    //EQ-processBlock logic:
    /*juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = device->getActiveInputChannels().getHighestBit() + 1;
    auto totalNumOutputChannels = device->getActiveOutputChannels().getHighestBit() + 1;

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        tempBuffer.clear(i, 0, tempBuffer.getNumSamples());*/

    updateFilters();

    juce::dsp::AudioBlock<float> block(tempBuffer);

    auto leftBlock = block.getSingleChannelBlock(0);
    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    leftChain.process(leftContext);

    if (tempBuffer.getNumChannels() > 1) // Handle stereo
    {
        auto rightBlock = block.getSingleChannelBlock(1);
        juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);
        rightChain.process(rightContext);
    }

    //CompressorAll
    if (!compAllMute)
    {
        auto block = juce::dsp::AudioBlock<float>(tempBuffer);
        auto context = juce::dsp::ProcessContextReplacing<float>(block);

        context.isBypassed = compAllMute;

        compressorAll.process(context);
    }

    //Multiband Compressor
    splitBands(tempBuffer);

    for (size_t i = 0; i < filterBuffers.size(); i++)
    {
        compressors[i].process(filterBuffers[i]);
    }

    auto numSamples = tempBuffer.getNumSamples();
    auto numChannels = tempBuffer.getNumChannels();

    tempBuffer.clear();

    auto addFilterBand = [nc = numChannels, ns = numSamples](auto& inputBuffer, const auto& source)
        {
            for (auto i = 0; i < nc; i++)
            {
                inputBuffer.addFrom(i, 0, source, i, 0, ns);
            }
        };

    bool bandsAreSoloed = false;
    for (auto& comp : compressors)
    {
        if (comp.solo)
        {
            bandsAreSoloed = true;
            break;
        }
    }

    if (bandsAreSoloed)
    {
        for (size_t i = 0; i < compressors.size(); i++)
        {
            auto& comp = compressors[i];
            if (comp.solo)
            {
                addFilterBand(tempBuffer, filterBuffers[i]);
            }
        }
    }
    else
    {
        for (size_t i = 0; i < compressors.size(); i++)
        {
            auto& comp = compressors[i];
            if (!comp.mute)
            {
                addFilterBand(tempBuffer, filterBuffers[i]);
            }
        }
    }

    //Gain
    applyGain(tempBuffer, gain);

    // Now copy the processed data back to the output buffer
    for (int channel = 0; channel < maxOutputChannels; ++channel)
    {
        if ((!activeOutputChannels[channel]) || maxInputChannels == 0)
        {
            bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
        }
        else
        {
            auto actualInputChannel = channel % maxInputChannels;

            if (activeInputChannels[actualInputChannel])
            {
                // Copy processed data back to the output buffer
                bufferToFill.buffer->copyFrom(channel, bufferToFill.startSample, tempBuffer, actualInputChannel, 0, bufferToFill.numSamples);
            }
            else
            {
                // If no input, clear the output channel
                bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
            }
        }
    }
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

// update all values with the UI values
void MainComponent::updateState() 
{
    gain.setGainDecibels(5.f);

    // Solo, Mute and Bypass also needs to be set for compressors
    compressorAll.setAttack(50.0f);
    compressorAll.setRelease(50.0f);
    compressorAll.setThreshold(0.0f);
    compressorAll.setRatio(3.0f);

    //update MultBandCompressor

    lowMidCrossover = 400.0f;
    LP1.setCutoffFrequency(lowMidCrossover);
    HP1.setCutoffFrequency(lowMidCrossover);

    midHighCrossover = 2000.0f;
    AP2.setCutoffFrequency(midHighCrossover);
    LP2.setCutoffFrequency(midHighCrossover);
    HP2.setCutoffFrequency(midHighCrossover);
}

//==============================================================================
//Gain

void MainComponent::prepareGain(int samplesPerBlock, double sampleRate) 
{
    auto* device = deviceManager.getCurrentAudioDevice();
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    //Attention: includes also inactive output channels, maybe has to be adjusted to 1
    spec.numChannels = static_cast<juce::uint32>(device->getActiveOutputChannels().countNumberOfSetBits());
    spec.sampleRate = sampleRate;

    gain.prepare(spec);

    gain.setRampDurationSeconds(0.05);
}

//==============================================================================
//EQ

void MainComponent::prepareEQ(int samplesPerBlock, double sampleRate)
{
    juce::dsp::ProcessSpec spec;

    spec.maximumBlockSize = samplesPerBlock;
    //maybe adjust to be dynamically
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;

    leftChain.prepare(spec);
    rightChain.prepare(spec);

    updateFilters();
}

void MainComponent::updatePeakFilter(const ChainSettingsEQ &chainSettings)
{
    auto *device = deviceManager.getCurrentAudioDevice();
    auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(device->getCurrentSampleRate(), chainSettings.peakFreq,
                                                                                chainSettings.peakQuality, juce::Decibels::decibelsToGain(chainSettings.peakGainInDecibels));

    updateCoefficients(leftChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
    updateCoefficients(rightChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
}

void MainComponent::updateCoefficients(Coefficients &old, const Coefficients &replacements)
{
    *old = *replacements;
}

void MainComponent::updateLowCutFilters(const ChainSettingsEQ &chainSettings)
{
    auto *device = deviceManager.getCurrentAudioDevice();

    auto lowCutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(
        chainSettings.lowCutFreq, device->getCurrentSampleRate(),
        2 * (chainSettings.lowCutSlope + 1));

    auto &leftLowCut = leftChain.get<ChainPositions::LowCut>();
    auto &rightLowCut = rightChain.get<ChainPositions::LowCut>();

    updateCutFilter(leftLowCut, lowCutCoefficients, chainSettings.lowCutSlope);
    updateCutFilter(rightLowCut, lowCutCoefficients, chainSettings.lowCutSlope);
}

void MainComponent::updateHighCutFilters(const ChainSettingsEQ &chainSettings)
{
    auto *device = deviceManager.getCurrentAudioDevice();

    auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
        chainSettings.highCutFreq, device->getCurrentSampleRate(),
        2 * (chainSettings.highCutSlope + 1));

    auto &leftHighCut = leftChain.get<ChainPositions::HighCut>();
    auto &rightHighCut = rightChain.get<ChainPositions::HighCut>();
   
    updateCutFilter(leftHighCut, highCutCoefficients, chainSettings.highCutSlope);
    updateCutFilter(rightHighCut, highCutCoefficients, chainSettings.highCutSlope);
}

void MainComponent::updateFilters()
{
    auto chainSettings = chainSettingsMember;

    updateLowCutFilters(chainSettings);
    updatePeakFilter(chainSettings);
    updateHighCutFilters(chainSettings);
}

//==============================================================================
//CompAll

void MainComponent::prepareCompAll(int samplesPerBlock, double sampleRate)
{
    auto* device = deviceManager.getCurrentAudioDevice();
    
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(device->getActiveOutputChannels().countNumberOfSetBits());
    spec.sampleRate = sampleRate;

    compressorAll.prepare(spec);
}

//==============================================================================
//MultComp

void MainComponent::prepareCompMult(int samplesPerBlock, double sampleRate)
{
    auto* device = deviceManager.getCurrentAudioDevice();

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(device->getActiveOutputChannels().getHighestBit() +1);
    spec.sampleRate = sampleRate;

    for (auto& comp : compressors)
    {
        comp.prepare(spec);
    }

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

void MainComponent::splitBands(const juce::AudioBuffer<float>& inputBuffer)
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

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    tabs.setBounds(getLocalBounds());

    //auto rect = getLocalBounds();

    /*audioSetupComp.setBounds(rect.removeFromLeft(proportionOfWidth(0.6f)));
    rect.reduce(10, 10);*/

    /*auto topLine(rect.removeFromTop(20));
    cpuUsageLabel.setBounds(topLine.removeFromLeft(topLine.getWidth() / 2));
    cpuUsageText.setBounds(topLine);
    rect.removeFromTop(20);*/

    //diagnosticsBox.setBounds(rect);
}

static juce::String getListOfActiveBits(const juce::BigInteger &b)
{
    juce::StringArray bits;

    for (auto i = 0; i <= b.getHighestBit(); ++i)
        if (b[i])
            bits.add(juce::String(i));

    return bits.joinIntoString(", ");
}

juce::AudioProcessorValueTreeState::ParameterLayout MainComponent::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

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
