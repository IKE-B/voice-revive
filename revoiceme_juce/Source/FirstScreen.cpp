/*#include "FirstScreen.h"

FirstScreen::FirstScreen()
    : state(Stopped),
    audioSetupComp(deviceManager,
        0, 256,   // input channels
        0, 256,   // output channels
        false,    // MIDI input
        false,    // MIDI output
        false,    // stereo pairs
        false)    // hide advanced options
{
    addAndMakeVisible(&playButton);
    playButton.setButtonText("Play");
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    playButton.setEnabled(false);

    addAndMakeVisible(&stopButton);
    stopButton.setButtonText("Stop");
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    stopButton.setEnabled(false);

    addAndMakeVisible(audioSetupComp);
    addAndMakeVisible(diagnosticsBox);

    diagnosticsBox.setMultiLine(true);
    diagnosticsBox.setReturnKeyStartsNewLine(true);
    diagnosticsBox.setReadOnly(true);
    diagnosticsBox.setScrollbarsShown(true);
    diagnosticsBox.setCaretVisible(false);
    diagnosticsBox.setPopupMenuEnabled(true);
    diagnosticsBox.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x32ffffff));
    diagnosticsBox.setColour(juce::TextEditor::outlineColourId, juce::Colour(0x1c000000));
    diagnosticsBox.setColour(juce::TextEditor::shadowColourId, juce::Colour(0x16000000));

    cpuUsageLabel.setText("CPU Usage", juce::dontSendNotification);
    cpuUsageText.setJustificationType(juce::Justification::right);
    addAndMakeVisible(&cpuUsageLabel);
    addAndMakeVisible(&cpuUsageText);

    setSize(760, 360);

    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
        && !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
            [this](bool granted) { setAudioChannels(granted ? 2 : 0, 2); });
    }
    else
    {
        setAudioChannels(2, 2);
    }

    deviceManager.addChangeListener(this);
    startTimer(50); // CPU Usage Timer
}

FirstScreen::~FirstScreen()
{
    deviceManager.removeChangeListener(this);
    shutdownAudio();
}

void FirstScreen::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // Audio preparation
}

void FirstScreen::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    auto* device = deviceManager.getCurrentAudioDevice();

    if (device == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    auto activeInputChannels = device->getActiveInputChannels();
    auto activeOutputChannels = device->getActiveOutputChannels();
    auto maxInputChannels = activeInputChannels.countNumberOfSetBits();
    auto maxOutputChannels = activeOutputChannels.countNumberOfSetBits();

    for (auto channel = 0; channel < maxOutputChannels; ++channel)
    {
        if (state == Stopped)
        {
            bufferToFill.clearActiveBufferRegion();
            continue;
        }

        if (!activeOutputChannels[channel] || maxInputChannels == 0)
        {
            bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
        }
        else
        {
            auto actualInputChannel = channel % maxInputChannels;

            if (!activeInputChannels[channel])
            {
                bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
            }
            else
            {
                auto* inBuffer = bufferToFill.buffer->getReadPointer(actualInputChannel, bufferToFill.startSample);
                auto* outBuffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);

                for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
                {
                    outBuffer[sample] = inBuffer[sample];
                }
            }
        }
    }
}

void FirstScreen::releaseResources()
{
    // Release resources
}

void FirstScreen::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    auto* device = deviceManager.getCurrentAudioDevice();

    if (device != nullptr && device->isPlaying())
        changeState(Playing);
    else
        changeState(Stopped);

    dumpDeviceInfo();
}

void FirstScreen::timerCallback()
{
    auto* device = deviceManager.getCurrentAudioDevice();

    if (device != nullptr)
    {
        auto cpu = deviceManager.getCpuUsage() * 100;
        cpuUsageText.setText(juce::String(cpu, 6) + " %", juce::dontSendNotification);
    }
}

void FirstScreen::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);
}

void FirstScreen::resized()
{
    playButton.setBounds(getWidth() / 2, getHeight() - 150, 40, 20);
    stopButton.setBounds(getWidth() / 2, getHeight() - 100, 40, 20);

    auto rect = getLocalBounds();
    audioSetupComp.setBounds(rect.removeFromLeft(proportionOfWidth(0.6f)));
    rect.reduce(10, 10);

    auto topLine(rect.removeFromTop(20));
    cpuUsageLabel.setBounds(topLine.removeFromLeft(topLine.getWidth() / 2));
    cpuUsageText.setBounds(topLine);
    rect.removeFromTop(20);

    diagnosticsBox.setBounds(rect);
}

void FirstScreen::playButtonClicked()
{
    changeState(Playing);
}

void FirstScreen::stopButtonClicked()
{
    changeState(Stopped);
}

void FirstScreen::changeState(TransportState newState)
{
    if (state != newState)
    {
        state = newState;

        switch (state)
        {
        case Stopped:
            playButton.setButtonText("Play");
            stopButton.setButtonText("Stop");
            playButton.setEnabled(true);
            stopButton.setEnabled(false);
            break;

        case Playing:
            stopButton.setButtonText("Stop");
            stopButton.setEnabled(true);
            playButton.setEnabled(false);
            break;

        default:
            break;
        }
    }
}

void FirstScreen::dumpDeviceInfo()
{
    logMessage("--------------------------------------");

    auto* device = deviceManager.getCurrentAudioDevice();

    if (device != nullptr)
    {
        logMessage("Current audio device: " + device->getName().quoted());
        logMessage("Sample rate: " + juce::String(device->getCurrentSampleRate()) + " Hz");
        logMessage("Block size: " + juce::String(device->getCurrentBufferSizeSamples()) + " samples");
        logMessage("Bit depth: " + juce::String(device->getCurrentBitDepth()));
        logMessage("Input channels: " + device->getInputChannelNames().joinIntoString(", "));
        logMessage("Active input channels: " + getListOfActiveBits(device->getActiveInputChannels()));
        logMessage("Output channels: " + device->getOutputChannelNames().joinIntoString(", "));
        logMessage("Active output channels: " + getListOfActiveBits(device->getActiveOutputChannels()));
    }
    else
    {
        logMessage("No audio device open");
    }
}

void FirstScreen::logMessage(const juce::String& m)
{
    diagnosticsBox.moveCaretToEnd();
    diagnosticsBox.insertTextAtCaret(m + juce::newLine);
}

static juce::String FirstScreen::getListOfActiveBits(const juce::BigInteger& b)
{
    juce::StringArray bits;

    for (auto i = 0; i <= b.getHighestBit(); ++i)
        if (b[i])
            bits.add(juce::String(i));

    return bits.joinIntoString(", ");
}
*/