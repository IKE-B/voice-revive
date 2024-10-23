#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
    : startTab(new StartComponent(deviceManager)), configTab(new ConfigComponent()), tabs(juce::TabbedButtonBar::TabsAtTop)
{
    tabs.addTab("Startseite",
                juce::Colours::grey,
                startTab,
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
                juce::Colours::grey,
                configTab,
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

    juce::dsp::ProcessSpec spec;

    spec.maximumBlockSize = samplesPerBlockExpected;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;

    leftChain.prepare(spec);
    rightChain.prepare(spec);

    updateFilters();
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    //bufferToFill.clearActiveBufferRegion();

    auto *device = deviceManager.getCurrentAudioDevice();
    auto activeInputChannels = device->getActiveInputChannels();
    auto activeOutputChannels = device->getActiveOutputChannels();
    auto maxInputChannels = activeInputChannels.countNumberOfSetBits();
    auto maxOutputChannels = activeOutputChannels.countNumberOfSetBits();
    //myAudioProcessor.totalInputChannels = maxInputChannels;
    //myAudioProcessor.totalOutputChannels = maxOutputChannels;

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

    // Call the processBlock function with the tempBuffer and the empty MIDI buffer
    //myAudioProcessor.processBlock(tempBuffer, emptyMidiBuffer);

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

ChainSettingsEQ getChainSettingsEQ(float lowCutFreqNew, float highCutFreqNew, float peakFreqNew,
                                   float peakGainInDecibelsNew, float peakQualityNew, SlopeEQ lowCutSlopeNew, SlopeEQ highCutSlopeNew)
{
    ChainSettingsEQ settings;


    // TODO
    // lowCutFreq und highCutFreq aktuell noch vertauscht!!
    settings.lowCutFreq = lowCutFreqNew;
    settings.highCutFreq = highCutFreqNew;
    settings.peakFreq = peakFreqNew;
    settings.peakGainInDecibels = peakGainInDecibelsNew;
    settings.peakQuality = peakQualityNew;
    settings.lowCutSlope = lowCutSlopeNew;
    settings.highCutSlope = highCutSlopeNew;

    return settings;
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

    auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(
        chainSettings.highCutFreq, device->getCurrentSampleRate(),
        2 * (chainSettings.highCutSlope + 1));

    auto &leftHighCut = leftChain.get<ChainPositions::HighCut>();
    auto &rightHighCut = rightChain.get<ChainPositions::HighCut>();
   
    updateCutFilter(leftHighCut, highCutCoefficients, chainSettings.highCutSlope);
    updateCutFilter(rightHighCut, highCutCoefficients, chainSettings.highCutSlope);
}

void MainComponent::updateFilters()
{
    auto chainSettings = getChainSettingsEQ(20.0f, 20000.0f, 750.0f, 0.0f, 1.0f, SlopeEQ::SlopeEQ_12, SlopeEQ::SlopeEQ_48);

    updateLowCutFilters(chainSettings);
    updatePeakFilter(chainSettings);
    updateHighCutFilters(chainSettings);
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

