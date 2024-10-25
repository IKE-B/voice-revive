#include <JuceHeader.h>
#include "ConfigComponent.h"

//==============================================================================
// Konstruktor
ConfigComponent::ConfigComponent(ChainSettingsEQ &chSe,
                                 juce::dsp::Gain<float> &gain,
                                 juce::dsp::Compressor<float> &compressorAll,
                                 bool &compAllMute,
                                 bool &compAllBypassed,
                                 CompressorBand &lowBandComp,
                                 CompressorBand &midBandComp,
                                 CompressorBand &highBandComp,
                                 float &lowMidCrossover,
                                 float &midHighCrossover)
    : chainSettings {chSe},
    gain {gain},
    compressorAll {compressorAll},
    compAllMute {compAllMute},
    compAllBypassed {compAllBypassed},
    lowBandComp {lowBandComp},
    midBandComp {midBandComp},
    highBandComp {highBandComp},
    lowMidCrossover {lowMidCrossover},
    midHighCrossover {midHighCrossover}
{
    // Initialisiere den Container f�r Slider und Labels
    componentContainer = std::make_unique<juce::Component>();

    const char* stringArray[] = { "Gain", "CompAllAttack", "CompAllRelease", "CompAllThreshold", "CompAllRatio" };
    const char* stringArray2[] = { "CompLowAttack", "CompLowRelease", "CompLowThreshold", "CompLowRatio" };
    const char* stringArray3[] = { "CompMidAttack", "CompMidRelease", "CompMidThreshold", "CompMidRatio" };
    const char* stringArray4[] = { "CompHighAttack", "CompHighRelease", "CompHighThreshold", "CompHighRatio" };
    const char* stringArray5[] = { "LowMidCutoff", "MidHighCutoff", "LowCutFreq", "HighCutFreq", "PeakFreq", "PeakGainInDecibels", "PeakQuality", "LowCutSlope", "HighCutSlope" };

    // F�ge die erste Gruppe von Slidern hinzu
    for (int i = 0; i < 5; ++i)
    {
        auto* sliderWithLabel = new CustomSliderWithLabel(stringArray[i]);
        sliders.add(sliderWithLabel);
        //sliderWithLabel->slider.addListener(this);
        componentContainer->addAndMakeVisible(sliderWithLabel);
    }

    // Erste Gruppe von Checkboxen
    auto* checkbox1 = new CustomCheckbox("CompAllBypass");
    checkboxes.add(checkbox1);
    componentContainer->addAndMakeVisible(checkbox1);

    auto* checkbox2 = new CustomCheckbox("CompAllMute");
    checkboxes.add(checkbox2);
    componentContainer->addAndMakeVisible(checkbox2);

    // F�ge die zweite Gruppe von Slidern hinzu
    for (int i = 0; i < 4; ++i)
    {
        auto* sliderWithLabel = new CustomSliderWithLabel(stringArray2[i]);
        sliders.add(sliderWithLabel);
        //sliderWithLabel->slider.addListener(this);
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
        //sliderWithLabel->slider.addListener(this);
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
        //sliderWithLabel->slider.addListener(this);
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

    // F�nfte Gruppe von Slidern (9 Slider)
    for (int i = 0; i < 9; ++i)
    {
        auto* sliderWithLabel = new CustomSliderWithLabel(stringArray5[i]);
        sliders.add(sliderWithLabel);
        //sliderWithLabel->slider.addListener(this);
        componentContainer->addAndMakeVisible(sliderWithLabel);
    }

    // F�ge den Container in den Viewport und den Viewport in die Hauptkomponente ein
    viewport.setViewedComponent(componentContainer.get());
    viewport.setScrollBarsShown(true, false); // Zeige nur den vertikalen Scrollbalken an
    addAndMakeVisible(viewport);
}

ConfigComponent::~ConfigComponent()
{
}

void ConfigComponent::paint(juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 2);   // draw an outline around the component

    //g.setColour (juce::Colours::white);
    //g.setFont (juce::FontOptions (14.0f));
    //g.drawText ("ConfigComponent", getLocalBounds(),
                //juce::Justification::centred, true);   // draw some placeholder text
}

void ConfigComponent::resized()
{
    viewport.setBounds(getLocalBounds());

    int boxHeight = 100;      // Gesamth�he f�r die Box mit Label und Slider
    int checkboxHeight = 50;  // H�he f�r die Checkboxen (inkl. Rahmen)
    int padding = 10;         // Abstand zwischen den Komponenten
    int extraPaddingForSliders = 30;  // Zus�tzlicher Abstand f�r Slider unter der Checkbox

    // Berechnung der H�he f�r jede Gruppe
    int totalHeightForFirstSliders = (boxHeight + padding) * 5;
    int totalHeightForCheckboxes1 = 2 * (checkboxHeight + padding); // Erste Checkbox-Gruppe
    int totalHeightForSecondSliders = (boxHeight + padding) * 4;
    int totalHeightForCheckboxes2 = 3 * (checkboxHeight + padding); // Zweite Checkbox-Gruppe
    int totalHeightForThirdSliders = (boxHeight + padding) * 4;
    int totalHeightForCheckboxes3 = 3 * (checkboxHeight + padding); // Dritte Checkbox-Gruppe
    int totalHeightForFourthSliders = (boxHeight + padding) * 4;
    int totalHeightForCheckboxes4 = 3 * (checkboxHeight + padding); // Vierte Checkbox-Gruppe
    int totalHeightForFifthSliders = (boxHeight + padding) * 9;     // F�nfte Slider-Gruppe

    int totalHeight = totalHeightForFirstSliders + totalHeightForCheckboxes1 +
        totalHeightForSecondSliders + totalHeightForCheckboxes2 +
        totalHeightForThirdSliders + totalHeightForCheckboxes3 +
        totalHeightForFourthSliders + totalHeightForCheckboxes4 +
        totalHeightForFifthSliders + padding;

    int availableWidth = getWidth() - viewport.getScrollBarThickness();

    componentContainer->setSize(availableWidth, totalHeight);

    // Erste Slider-Gruppe
    int yOffset = 0;
    for (int i = 0; i < 5; ++i)
    {
        sliders[i]->setBounds(10, yOffset, availableWidth - 20, boxHeight);
        yOffset += boxHeight + padding;
    }

    // Erste Checkbox-Gruppe
    for (int i = 0; i < 2; ++i)
    {
        checkboxes[i]->setBounds(10, yOffset, availableWidth - 20, checkboxHeight);
        yOffset += checkboxHeight + padding;
    }

    // Zweite Slider-Gruppe
    for (int i = 5; i < 9; ++i)
    {
        sliders[i]->setBounds(10, yOffset, availableWidth - 20, boxHeight);
        yOffset += boxHeight + padding;
    }

    // Zweite Checkbox-Gruppe
    for (int i = 2; i < 5; ++i)
    {
        checkboxes[i]->setBounds(10, yOffset, availableWidth - 20, checkboxHeight);
        yOffset += checkboxHeight + padding;
    }

    // Dritte Slider-Gruppe
    for (int i = 9; i < 13; ++i)
    {
        sliders[i]->setBounds(10, yOffset, availableWidth - 20, boxHeight);
        yOffset += boxHeight + padding;
    }

    // Dritte Checkbox-Gruppe
    for (int i = 5; i < 8; ++i)
    {
        checkboxes[i]->setBounds(10, yOffset, availableWidth - 20, checkboxHeight);
        yOffset += checkboxHeight + padding;
    }

    // Vierte Slider-Gruppe
    for (int i = 13; i < 17; ++i)
    {
        sliders[i]->setBounds(10, yOffset, availableWidth - 20, boxHeight);
        yOffset += boxHeight + padding;
    }

    // Vierte Checkbox-Gruppe
    for (int i = 8; i < 11; ++i)
    {
        checkboxes[i]->setBounds(10, yOffset, availableWidth - 20, checkboxHeight);
        yOffset += checkboxHeight + padding;
    }

    // F�nfte Slider-Gruppe
    for (int i = 17; i < 26; ++i)
    {
        sliders[i]->setBounds(10, yOffset, availableWidth - 20, boxHeight);
        yOffset += boxHeight + padding;
    }
}
