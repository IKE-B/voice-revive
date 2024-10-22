/*
#pragma once

#include <JuceHeader.h>

class FirstScreen : public juce::Component,
    private juce::ChangeListener,
    private juce::Timer
{
public:
    FirstScreen(juce::AudioDeviceManager& deviceManagerRef)
        : deviceManager(deviceManagerRef), state(Stopped),
        audioSetupComp(deviceManagerRef, 0, 256, 0, 256, false, false, false, false)
    {
        // Play Button
        playButton.setButtonText("Play");
        playButton.onClick = [this] { playButtonClicked(); };
        playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
        playButton.setEnabled(false);
        addAndMakeVisible(playButton);

        // Stop Button
        stopButton.setButtonText("Stop");
        stopButton.onClick = [this] { stopButtonClicked(); };
        stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
        stopButton.setEnabled(false);
        addAndMakeVisible(stopButton);

        // Audio Setup Component
        addAndMakeVisible(audioSetupComp);

        // Diagnostics Box
        diagnosticsBox.setMultiLine(true);
        diagnosticsBox.setReturnKeyStartsNewLine(true);
        diagnosticsBox.setReadOnly(true);
        diagnosticsBox.setScrollbarsShown(true);
        diagnosticsBox.setCaretVisible(false);
        diagnosticsBox.setPopupMenuEnabled(true);
        diagnosticsBox.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x32ffffff));
        diagnosticsBox.setColour(juce::TextEditor::outlineColourId, juce::Colour(0x1c000000));
        diagnosticsBox.setColour(juce::TextEditor::shadowColourId, juce::Colour(0x16000000));
        addAndMakeVisible(diagnosticsBox);

        // CPU Usage
        cpuUsageLabel.setText("CPU Usage", juce::dontSendNotification);
        cpuUsageText.setJustificationType(juce::Justification::right);
        addAndMakeVisible(cpuUsageLabel);
        addAndMakeVisible(cpuUsageText);

        // Set up listeners
        deviceManager.addChangeListener(this);
        startTimer(50);  // Start timer for CPU usage updates
    }

    ~FirstScreen()
    {
        deviceManager.removeChangeListener(this);
    }

    void resized() override
    {
        

        auto area = getLocalBounds();  // Gesamter Bereich des Screens

        // Audio-Setup-Komponente nimmt die linke Hälfte des Bildschirms ein
        audioSetupComp.setBounds(area.removeFromLeft(proportionOfWidth(0.6f)));

        // Reduziere den verbleibenden Bereich
        area.reduce(10, 10);

        // Platziere die CPU-Anzeige oben
        auto topLine(area.removeFromTop(20));
        cpuUsageLabel.setBounds(topLine.removeFromLeft(topLine.getWidth() / 2));
        cpuUsageText.setBounds(topLine);

        // Diagnostics Box unter der CPU-Anzeige
        diagnosticsBox.setBounds(area.removeFromTop(100));

        // Platzierung der Buttons unten im verbleibenden Bereich
        auto buttonArea = area.removeFromBottom(50);  // Bereich für die Buttons
        playButton.setBounds(buttonArea.removeFromLeft(buttonArea.getWidth() / 2).reduced(10));
        stopButton.setBounds(buttonArea.reduced(10));
    }

    void changeListenerCallback(juce::ChangeBroadcaster* source) override
    {
        if (deviceManager.getCurrentAudioDevice()->isPlaying())
            changeState(Playing);
        else
            changeState(Stopped);

        dumpDeviceInfo();
    }

    void timerCallback() override
    {
        auto cpu = deviceManager.getCpuUsage() * 100;
        cpuUsageText.setText(juce::String(cpu, 6) + " %", juce::dontSendNotification);
    }

private:
    // Audio setup
    juce::AudioDeviceManager& deviceManager;
    juce::AudioDeviceSelectorComponent audioSetupComp;

    // Buttons
    juce::TextButton playButton;
    juce::TextButton stopButton;

    // Diagnostics and CPU usage
    juce::TextEditor diagnosticsBox;
    juce::Label cpuUsageLabel;
    juce::Label cpuUsageText;

    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Stopping
    };

    TransportState state;

    void playButtonClicked()
    {
        changeState(Playing);
    }

    void stopButtonClicked()
    {
        changeState(Stopped);
    }

    void changeState(TransportState newState)
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

    void dumpDeviceInfo()
    {
        diagnosticsBox.moveCaretToEnd();
        diagnosticsBox.insertTextAtCaret("--------------------------------------\n");

        if (auto* device = deviceManager.getCurrentAudioDevice())
        {
            diagnosticsBox.insertTextAtCaret("Current audio device: " + device->getName().quoted() + "\n");
            diagnosticsBox.insertTextAtCaret("Sample rate: " + juce::String(device->getCurrentSampleRate()) + " Hz\n");
            diagnosticsBox.insertTextAtCaret("Block size: " + juce::String(device->getCurrentBufferSizeSamples()) + " samples\n");
            diagnosticsBox.insertTextAtCaret("Bit depth: " + juce::String(device->getCurrentBitDepth()) + "\n");
            diagnosticsBox.insertTextAtCaret("Input channel names: " + device->getInputChannelNames().joinIntoString(", ") + "\n");
            diagnosticsBox.insertTextAtCaret("Active input channels: " + getListOfActiveBits(device->getActiveInputChannels()) + "\n");
            diagnosticsBox.insertTextAtCaret("Output channel names: " + device->getOutputChannelNames().joinIntoString(", ") + "\n");
            diagnosticsBox.insertTextAtCaret("Active output channels: " + getListOfActiveBits(device->getActiveOutputChannels()) + "\n");
        }
        else
        {
            diagnosticsBox.insertTextAtCaret("No audio device open\n");
        }
    }


    static juce::String getListOfActiveBits(const juce::BigInteger& b)
    {
        juce::StringArray bits;

        for (auto i = 0; i <= b.getHighestBit(); ++i)
            if (b[i])
                bits.add(juce::String(i));

        return bits.joinIntoString(", ");
    }
};
*/




#pragma once

#include <JuceHeader.h>

class FirstScreen : public juce::Component,
    private juce::ChangeListener,
    private juce::Timer,
    public juce::AudioIODeviceCallback  // Hinzufügen der Callback-Schnittstelle
{
public:
    FirstScreen(juce::AudioDeviceManager& deviceManagerRef)
        : deviceManager(deviceManagerRef), state(Stopped),
        audioSetupComp(deviceManagerRef, 0, 256, 0, 256, false, false, false, false)
    {
        // Play Button
        playButton.setButtonText("Play");
        playButton.onClick = [this] { playButtonClicked(); };
        playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
        playButton.setEnabled(false);
        addAndMakeVisible(playButton);

        // Stop Button
        stopButton.setButtonText("Stop");
        stopButton.onClick = [this] { stopButtonClicked(); };
        stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
        stopButton.setEnabled(false);
        addAndMakeVisible(stopButton);

        // Audio Setup Component
        addAndMakeVisible(audioSetupComp);

        // Diagnostics Box
        diagnosticsBox.setMultiLine(true);
        diagnosticsBox.setReturnKeyStartsNewLine(true);
        diagnosticsBox.setReadOnly(true);
        diagnosticsBox.setScrollbarsShown(true);
        diagnosticsBox.setCaretVisible(false);
        diagnosticsBox.setPopupMenuEnabled(true);
        diagnosticsBox.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x32ffffff));
        diagnosticsBox.setColour(juce::TextEditor::outlineColourId, juce::Colour(0x1c000000));
        diagnosticsBox.setColour(juce::TextEditor::shadowColourId, juce::Colour(0x16000000));
        addAndMakeVisible(diagnosticsBox);

        // CPU Usage
        cpuUsageLabel.setText("CPU Usage", juce::dontSendNotification);
        cpuUsageText.setJustificationType(juce::Justification::right);
        addAndMakeVisible(cpuUsageLabel);
        addAndMakeVisible(cpuUsageText);

        // Set up listeners
        deviceManager.addChangeListener(this);
        deviceManager.addAudioCallback(this);  // Callback hinzufügen
        startTimer(50);  // Start timer for CPU usage updates
    }

    ~FirstScreen()
    {
        deviceManager.removeChangeListener(this);
        deviceManager.removeAudioCallback(this);  // Entfernen des Callbacks
    }

    void audioDeviceIOCallback(const float* const* inputChannelData, int numInputChannels,
        float* const* outputChannelData, int numOutputChannels, int numSamples)
    {
        for (int channel = 0; channel < numOutputChannels; ++channel)
        {
            if (channel < numInputChannels)
            {
                // Kopiere den Eingang direkt auf den Ausgang
                std::memcpy(outputChannelData[channel], inputChannelData[channel], sizeof(float) * numSamples);
            }
            else
            {
                // Fülle nicht verwendete Ausgangskanäle mit Stille
                std::fill_n(outputChannelData[channel], numSamples, 0.0f);
            }
        }
    }


    void audioDeviceAboutToStart(juce::AudioIODevice* device) override
    {
        // Vorbereitung, bevor das Audio-Gerät startet
    }

    void audioDeviceStopped() override
    {
        // Cleanup, wenn das Audio-Gerät stoppt
    }

    void resized() override
    {
        auto area = getLocalBounds();  // Gesamter Bereich des Screens

        // Audio-Setup-Komponente nimmt die linke Hälfte des Bildschirms ein
        audioSetupComp.setBounds(area.removeFromLeft(proportionOfWidth(0.6f)));

        // Reduziere den verbleibenden Bereich
        area.reduce(10, 10);

        // Platziere die CPU-Anzeige oben
        auto topLine(area.removeFromTop(20));
        cpuUsageLabel.setBounds(topLine.removeFromLeft(topLine.getWidth() / 2));
        cpuUsageText.setBounds(topLine);

        // Diagnostics Box unter der CPU-Anzeige
        diagnosticsBox.setBounds(area.removeFromTop(100));

        // Platzierung der Buttons unten im verbleibenden Bereich
        auto buttonArea = area.removeFromBottom(50);  // Bereich für die Buttons
        playButton.setBounds(buttonArea.removeFromLeft(buttonArea.getWidth() / 2).reduced(10));
        stopButton.setBounds(buttonArea.reduced(10));
    }

    void changeListenerCallback(juce::ChangeBroadcaster* source) override
    {
        if (deviceManager.getCurrentAudioDevice()->isPlaying())
            changeState(Playing);
        else
            changeState(Stopped);

        dumpDeviceInfo();
    }

    void timerCallback() override
    {
        auto cpu = deviceManager.getCpuUsage() * 100;
        cpuUsageText.setText(juce::String(cpu, 6) + " %", juce::dontSendNotification);
    }

private:
    // Audio setup
    juce::AudioDeviceManager& deviceManager;
    juce::AudioDeviceSelectorComponent audioSetupComp;

    // Buttons
    juce::TextButton playButton;
    juce::TextButton stopButton;

    // Diagnostics and CPU usage
    juce::TextEditor diagnosticsBox;
    juce::Label cpuUsageLabel;
    juce::Label cpuUsageText;

    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Stopping
    };

    TransportState state;

    void playButtonClicked()
    {
        changeState(Playing);
    }

    void stopButtonClicked()
    {
        changeState(Stopped);
    }

    void changeState(TransportState newState)
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

    void dumpDeviceInfo()
    {
        diagnosticsBox.moveCaretToEnd();
        diagnosticsBox.insertTextAtCaret("--------------------------------------\n");

        if (auto* device = deviceManager.getCurrentAudioDevice())
        {
            diagnosticsBox.insertTextAtCaret("Current audio device: " + device->getName().quoted() + "\n");
            diagnosticsBox.insertTextAtCaret("Sample rate: " + juce::String(device->getCurrentSampleRate()) + " Hz\n");
            diagnosticsBox.insertTextAtCaret("Block size: " + juce::String(device->getCurrentBufferSizeSamples()) + " samples\n");
            diagnosticsBox.insertTextAtCaret("Bit depth: " + juce::String(device->getCurrentBitDepth()) + "\n");
            diagnosticsBox.insertTextAtCaret("Input channel names: " + device->getInputChannelNames().joinIntoString(", ") + "\n");
            diagnosticsBox.insertTextAtCaret("Active input channels: " + getListOfActiveBits(device->getActiveInputChannels()) + "\n");
            diagnosticsBox.insertTextAtCaret("Output channel names: " + device->getOutputChannelNames().joinIntoString(", ") + "\n");
            diagnosticsBox.insertTextAtCaret("Active output channels: " + getListOfActiveBits(device->getActiveOutputChannels()) + "\n");
        }
        else
        {
            diagnosticsBox.insertTextAtCaret("No audio device open\n");
        }
    }

    static juce::String getListOfActiveBits(const juce::BigInteger& b)
    {
        juce::StringArray bits;

        for (auto i = 0; i <= b.getHighestBit(); ++i)
            if (b[i])
                bits.add(juce::String(i));

        return bits.joinIntoString(", ");
    }
};

