#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
    : startTab {deviceManager},
    configTab {chainSettingsMember,
              gain,
              compressorAll,
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

    //EQ
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

    //EQ processing
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
    auto blockCompAll = juce::dsp::AudioBlock<float>(tempBuffer);
    auto contextCompAll = juce::dsp::ProcessContextReplacing<float>(blockCompAll);

    contextCompAll.isBypassed = compAllBypassed;

    compressorAll.process(contextCompAll);

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
}

//==============================================================================
//Gain

void MainComponent::prepareGain(int samplesPerBlock, double sampleRate) 
{
    auto* device = deviceManager.getCurrentAudioDevice();
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
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
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    tabs.setBounds(getLocalBounds());
}