#include "MainComponent.h"
#include "FirstScreen.h"
#include "SecondScreen.h"

MainComponent::MainComponent()
    : firstScreen(deviceManager)
{
    // Add buttons to switch between screens
    firstScreenButton.setButtonText("Show First Screen");
    firstScreenButton.onClick = [this] { showFirstScreen(); };
    addAndMakeVisible(firstScreenButton);

    secondScreenButton.setButtonText("Show Second Screen");
    secondScreenButton.onClick = [this] { showSecondScreen(); };
    addAndMakeVisible(secondScreenButton);

    // Add screens
    addAndMakeVisible(firstScreen);
    addChildComponent(secondScreen);  // initially hidden

    setSize(760, 360);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {}
void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {}
void MainComponent::releaseResources() {}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto area = getLocalBounds();

    firstScreenButton.setBounds(area.removeFromTop(40).removeFromLeft(getWidth() / 2));
    secondScreenButton.setBounds(area.removeFromTop(40));

    firstScreen.setBounds(area);
    secondScreen.setBounds(area);
}

void MainComponent::showFirstScreen()
{
    firstScreen.setVisible(true);
    secondScreen.setVisible(false);
}

void MainComponent::showSecondScreen()
{
    firstScreen.setVisible(false);
    secondScreen.setVisible(true);
}
