#include <JuceHeader.h>
#include "ConfigComponent.h"

//==============================================================================
// Konstruktor
ConfigComponent::ConfigComponent()
{
    // Initialisiere den Container für Slider und Labels
    componentContainer = std::make_unique<juce::Component>();

    const char* stringArray[] = { "Gain", "CompAllAttack", "CompAllRelease", "CompAllThreshold", "CompAllRatio" };
    const char* stringArray2[] = { "CompLowAttack", "CompLowRelease", "CompLowThreshold", "CompLowRatio" };
    const char* stringArray3[] = { "CompMidAttack", "CompMidRelease", "CompMidThreshold", "CompMidRatio" };
    const char* stringArray4[] = { "CompHighAttack", "CompHighRelease", "CompHighThreshold", "CompHighRatio" };
    const char* stringArray5[] = { "LowMidCutoff", "MidHighCutoff", "LowCutFreq", "HighCutFreq", "PeakFreq", "PeakGainInDecibels", "PeakQuality", "LowCutSlope", "HighCutSlope" };

    // Füge die erste Gruppe von Slidern hinzu
    for (int i = 0; i < 5; ++i)
    {
        auto* sliderWithLabel = new CustomSliderWithLabel(stringArray[i]);
        sliders.add(sliderWithLabel);
        componentContainer->addAndMakeVisible(sliderWithLabel);
    }

    // Erste Gruppe von Checkboxen
    auto* checkbox1 = new CustomCheckbox("CompAllBypass");
    checkboxes.add(checkbox1);
    componentContainer->addAndMakeVisible(checkbox1);

    auto* checkbox2 = new CustomCheckbox("CompAllMute");
    checkboxes.add(checkbox2);
    componentContainer->addAndMakeVisible(checkbox2);

    // Füge die zweite Gruppe von Slidern hinzu
    for (int i = 0; i < 4; ++i)
    {
        auto* sliderWithLabel = new CustomSliderWithLabel(stringArray2[i]);
        sliders.add(sliderWithLabel);
        componentContainer->addAndMakeVisible(sliderWithLabel);
    }

    // Zweite Gruppe von Checkboxen
    auto* checkbox3 = new CustomCheckbox("CompGroupBypass");
    checkboxes.add(checkbox3);
    componentContainer->addAndMakeVisible(checkbox3);

    auto* checkbox4 = new CustomCheckbox("CompGroupMute");
    checkboxes.add(checkbox4);
    componentContainer->addAndMakeVisible(checkbox4);

    auto* checkbox5 = new CustomCheckbox("CompGroupSolo");
    checkboxes.add(checkbox5);
    componentContainer->addAndMakeVisible(checkbox5);

    // Dritte Gruppe von Slidern
    for (int i = 0; i < 4; ++i)
    {
        auto* sliderWithLabel = new CustomSliderWithLabel(stringArray3[i]);
        sliders.add(sliderWithLabel);
        componentContainer->addAndMakeVisible(sliderWithLabel);
    }

    // Dritte Gruppe von Checkboxen
    auto* checkbox6 = new CustomCheckbox("CompMidBypassed");
    checkboxes.add(checkbox6);
    componentContainer->addAndMakeVisible(checkbox6);

    auto* checkbox7 = new CustomCheckbox("CompMidMute");
    checkboxes.add(checkbox7);
    componentContainer->addAndMakeVisible(checkbox7);

    auto* checkbox8 = new CustomCheckbox("CompMidSolo");
    checkboxes.add(checkbox8);
    componentContainer->addAndMakeVisible(checkbox8);

    // Vierte Gruppe von Slidern
    for (int i = 0; i < 4; ++i)
    {
        auto* sliderWithLabel = new CustomSliderWithLabel(stringArray4[i]);
        sliders.add(sliderWithLabel);
        componentContainer->addAndMakeVisible(sliderWithLabel);
    }

    // Vierte Gruppe von Checkboxen
    auto* checkbox9 = new CustomCheckbox("CompHighBypassed");
    checkboxes.add(checkbox9);
    componentContainer->addAndMakeVisible(checkbox9);

    auto* checkbox10 = new CustomCheckbox("CompHighMute");
    checkboxes.add(checkbox10);
    componentContainer->addAndMakeVisible(checkbox10);

    auto* checkbox11 = new CustomCheckbox("CompHighSolo");
    checkboxes.add(checkbox11);
    componentContainer->addAndMakeVisible(checkbox11);

    // Fünfte Gruppe von Slidern (9 Slider)
    for (int i = 0; i < 9; ++i)
    {
        auto* sliderWithLabel = new CustomSliderWithLabel(stringArray5[i]);
        sliders.add(sliderWithLabel);
        componentContainer->addAndMakeVisible(sliderWithLabel);
    }

    // Füge den Container in den Viewport und den Viewport in die Hauptkomponente ein
    viewport.setViewedComponent(componentContainer.get());
    viewport.setScrollBarsShown(true, false); // Zeige nur den vertikalen Scrollbalken an
    addAndMakeVisible(viewport);
}

ConfigComponent::~ConfigComponent()
{
}

void ConfigComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));  // Hintergrund füllen
    g.setColour(juce::Colours::white);
    g.setFont(14.0f);
}

void ConfigComponent::resized()
{
    viewport.setBounds(getLocalBounds());

    int boxHeight = 100;  // Gesamthöhe für die Box mit Label und Slider
    int checkboxHeight = 50; // Höhe für die Checkboxen (inkl. Rahmen)
    int padding = 10;     // Abstand zwischen den Komponenten
    int extraPaddingForSliders = 30;  // Zusätzlicher Abstand für Slider unter der Checkbox

    int totalHeightForFirstSliders = (boxHeight + padding) * 5;
    int totalHeightForCheckboxes1 = 2 * (checkboxHeight + padding); // Erste Checkbox-Gruppe
    int totalHeightForSecondSliders = (boxHeight + padding) * 4;
    int totalHeightForCheckboxes2 = 3 * (checkboxHeight + padding); // Zweite Checkbox-Gruppe
    int totalHeightForThirdSliders = (boxHeight + padding) * 4;
    int totalHeightForCheckboxes3 = 3 * (checkboxHeight + padding); // Dritte Checkbox-Gruppe
    int totalHeightForFourthSliders = (boxHeight + padding) * 4;
    int totalHeightForFifthSliders = (boxHeight + padding) * 9;

    int totalHeight = totalHeightForFirstSliders + totalHeightForCheckboxes1 + totalHeightForSecondSliders +
        totalHeightForCheckboxes2 + totalHeightForThirdSliders + totalHeightForCheckboxes3 +
        totalHeightForFourthSliders + totalHeightForFifthSliders;

    int availableWidth = getWidth() - viewport.getScrollBarThickness();

    componentContainer->setSize(availableWidth, totalHeight);

    // Positioniere die erste Gruppe von Slidern
    for (int i = 0; i < 5; ++i)
    {
        int yOffset = i * (boxHeight + padding);
        sliders[i]->setBounds(10, yOffset, availableWidth - 20, boxHeight);
    }

    // Positioniere die erste Gruppe von Checkboxen
    int checkboxOffset1 = totalHeightForFirstSliders + padding;
    checkboxes[0]->setBounds(10, checkboxOffset1, availableWidth - 20, checkboxHeight);
    checkboxes[1]->setBounds(10, checkboxOffset1 + checkboxHeight + padding, availableWidth - 20, checkboxHeight);

    // Zweite Slider-Gruppe
    int secondSliderOffset = checkboxOffset1 + checkboxHeight + extraPaddingForSliders;
    for (int i = 0; i < 4; ++i)
    {
        int yOffset = secondSliderOffset + i * (boxHeight + padding);
        sliders[i + 5]->setBounds(10, yOffset, availableWidth - 20, boxHeight);
    }

    // Zweite Gruppe von Checkboxen
    int checkboxOffset2 = secondSliderOffset + (boxHeight + padding) * 4 + padding;
    checkboxes[2]->setBounds(10, checkboxOffset2, availableWidth - 20, checkboxHeight);
    checkboxes[3]->setBounds(10, checkboxOffset2 + checkboxHeight + padding, availableWidth - 20, checkboxHeight);
    checkboxes[4]->setBounds(10, checkboxOffset2 + 2 * (checkboxHeight + padding), availableWidth - 20, checkboxHeight);

    // Dritte Slider-Gruppe
    int thirdSliderOffset = checkboxOffset2 + 3 * (checkboxHeight + padding) + extraPaddingForSliders;
    for (int i = 0; i < 4; ++i)
    {
        int yOffset = thirdSliderOffset + i * (boxHeight + padding);
        sliders[i + 9]->setBounds(10, yOffset, availableWidth - 20, boxHeight);
    }

    // Dritte Gruppe von Checkboxen
    int checkboxOffset3 = thirdSliderOffset + (boxHeight + padding) * 4 + padding;
    checkboxes[5]->setBounds(10, checkboxOffset3, availableWidth - 20, checkboxHeight);
    checkboxes[6]->setBounds(10, checkboxOffset3 + checkboxHeight + padding, availableWidth - 20, checkboxHeight);
    checkboxes[7]->setBounds(10, checkboxOffset3 + 2 * (checkboxHeight + padding), availableWidth - 20, checkboxHeight);

    // Vierte Gruppe von Slidern
    int fourthSliderOffset = checkboxOffset3 + 3 * (checkboxHeight + padding) + extraPaddingForSliders;
    for (int i = 0; i < 4; ++i)
    {
        int yOffset = fourthSliderOffset + i * (boxHeight + padding);
        sliders[i + 13]->setBounds(10, yOffset, availableWidth - 20, boxHeight);
    }

    // Vierte Gruppe von Checkboxen
    int checkboxOffset4 = fourthSliderOffset + (boxHeight + padding) * 4 + padding;
    checkboxes[8]->setBounds(10, checkboxOffset4, availableWidth - 20, checkboxHeight);
    checkboxes[9]->setBounds(10, checkboxOffset4 + checkboxHeight + padding, availableWidth - 20, checkboxHeight);
    checkboxes[10]->setBounds(10, checkboxOffset4 + 2 * (checkboxHeight + padding), availableWidth - 20, checkboxHeight);

    // Fünfte Gruppe von Slidern (9 Slider)
    int fifthSliderOffset = checkboxOffset4 + 3 * (checkboxHeight + padding) + extraPaddingForSliders;
    for (int i = 0; i < 9; ++i)
    {
        int yOffset = fifthSliderOffset + i * (boxHeight + padding);
        sliders[i + 17]->setBounds(10, yOffset, availableWidth - 20, boxHeight);
    }
}

void ConfigComponent::paintOverChildren(juce::Graphics& g)
{
    // Dies zeichnet die Ränder um die Kinderkomponenten
}
