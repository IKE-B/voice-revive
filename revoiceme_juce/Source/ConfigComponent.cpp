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

    /*const char* stringArray[] = { "Gain", "CompAllAttack", "CompAllRelease", "CompAllThreshold", "CompAllRatio" };
    const char* stringArray2[] = { "CompLowAttack", "CompLowRelease", "CompLowThreshold", "CompLowRatio" };
    const char* stringArray3[] = { "CompMidAttack", "CompMidRelease", "CompMidThreshold", "CompMidRatio" };
    const char* stringArray4[] = { "CompHighAttack", "CompHighRelease", "CompHighThreshold", "CompHighRatio" };
    const char* stringArray5[] = { "LowMidCutoff", "MidHighCutoff", "LowCutFreq", "HighCutFreq", "PeakFreq", "PeakGainInDecibels", "PeakQuality", "LowCutSlope", "HighCutSlope" };*/

    // F�ge die erste Gruppe von Slidern hinzu
    //for (int i = 0; i < 5; ++i)
    //{
    //    auto* sliderWithLabel = new CustomSliderWithLabel(stringArray[i]);
    //    sliders.add(sliderWithLabel);
    //    //sliderWithLabel->slider.addListener(this);
    //    componentContainer->addAndMakeVisible(sliderWithLabel);
    //}
    
    // ========================================================================================================================== //
    // Gain
    createAndAddCustomSlider(-24.0f,
                             24.0f,
                             0.5f,
                             0.0f,
                             " dB",
                             1,
                             [this, &gain] { gain.setGainDecibels(static_cast<float>(sliders[0]->getValue())); },
                             "Gain");
    // ========================================================================================================================== //
    // ---------------------------------------------------- ALL COMP ------------------------------------------------------------ //
    // CompAllAttack
    createAndAddCustomSlider(5.0f,
                             500.0f,
                             1.0f,
                             50.0f,
                             "",
                             0,
                             [this, &compressorAll] { compressorAll.setAttack(static_cast<float>(sliders[1]->getValue())); },
                             "CompAllAttack");
    // ========================================================================================================================== //
    // CompAllRelease
    createAndAddCustomSlider(5.0f,
                             500.0f,
                             1.0f,
                             250.0f,
                             "",
                             0,
                             [this, &compressorAll] { compressorAll.setRelease(static_cast<float>(sliders[2]->getValue())); },
                             "CompAllRelease");
    // ========================================================================================================================== //
    // CompAllThreshold
    createAndAddCustomSlider(-60.0f,
                             12.0f,
                             1.0f,
                             0.0f,
                             "",
                             0,
                             [this, &compressorAll] { compressorAll.setThreshold(static_cast<float>(sliders[3]->getValue())); },
                             "CompAllThreshold");
    // ========================================================================================================================== //
    // CompAllRatio
    createAndAddCustomSlider(1.0f,
                             100.0f,
                             0.5f,
                             3.0f,
                             "",
                             1,
                             [this, &compressorAll] { compressorAll.setRatio(static_cast<float>(sliders[4]->getValue())); },
                             "CompAllRatio");
    // ========================================================================================================================== //
    // ---------------------------------------------------- LOW COMP ------------------------------------------------------------ //
    // CompLowAttack
    createAndAddCustomSlider(5.0f,
                             500.0f,
                             1.0f,
                             50.0f,
                             "",
                             0,
                             [this, &lowBandComp] { lowBandComp.compressor.setAttack(static_cast<float>(sliders[5]->getValue())); },
                             "CompLowAttack");
    // ========================================================================================================================== //
    // CompLowRelease
    createAndAddCustomSlider(5.0f,
                             500.0f,
                             1.0f,
                             250.0f,
                             "",
                             0,
                             [this, &lowBandComp] { lowBandComp.compressor.setRelease(static_cast<float>(sliders[6]->getValue())); },
                             "CompLowRelease");
    // ========================================================================================================================== //
    // CompLowThreshold
    createAndAddCustomSlider(-60.0f, 
                             12.0f,
                             1.0f,
                             0.0f,
                             "",
                             0,
                             [this, &lowBandComp] { lowBandComp.compressor.setThreshold(static_cast<float>(sliders[7]->getValue())); },
                             "CompLowThreshold");
    // ========================================================================================================================== //
    // CompLowRatio
    createAndAddCustomSlider(1.0f,
                             100.0f,
                             0.5f,
                             3.0f,
                             "",
                             1,
                             [this, &lowBandComp] { lowBandComp.compressor.setRatio(static_cast<float>(sliders[8]->getValue())); },
                             "CompLowRatio");
    // ========================================================================================================================== //
    // ---------------------------------------------------- MID COMP ------------------------------------------------------------ //
    // CompMidAttack
    createAndAddCustomSlider(5.0f,
                             500.0f,
                             1.0f,
                             50.0f,
                             "",
                             0,
                             [this, &midBandComp] { midBandComp.compressor.setAttack(static_cast<float>(sliders[9]->getValue())); },
                             "CompMidAttack");
    // ========================================================================================================================== //
    // CompMidRelease
    createAndAddCustomSlider(5.0f,
                             500.0f,
                             1.0f,
                             250.0f,
                             "",
                             0,
                             [this, &midBandComp] { midBandComp.compressor.setRelease(static_cast<float>(sliders[10]->getValue())); },
                             "CompMidRelease");
    // ========================================================================================================================== //
    // CompMidThreshold
    createAndAddCustomSlider(-60.0f,
                             12.0f,
                             1.0f,
                             0.0f,
                             "",
                             0,
                             [this, &midBandComp] { midBandComp.compressor.setThreshold(static_cast<float>(sliders[11]->getValue())); },
                             "CompMidThreshold");
    // ========================================================================================================================== //
    // CompMidRatio
    createAndAddCustomSlider(1.0f,
                             100.0f,
                             0.5f,
                             3.0f,
                             "",
                             1,
                             [this, &midBandComp] { midBandComp.compressor.setRatio(static_cast<float>(sliders[12]->getValue())); },
                             "CompMidRatio");
    // ========================================================================================================================== //
    // ---------------------------------------------------- HIGH COMP ----------------------------------------------------------- //
    // CompHighAttack
    createAndAddCustomSlider(5.0f,
                             500.0f,
                             1.0f,
                             50.0f,
                             "",
                             0,
                             [this, &highBandComp] { highBandComp.compressor.setAttack(static_cast<float>(sliders[13]->getValue())); },
                             "CompHighAttack");
    // ========================================================================================================================== //
    // CompHighRelease
    createAndAddCustomSlider(5.0f,
                             500.0f,
                             1.0f,
                             250.0f,
                             "",
                             0,
                             [this, &highBandComp] { highBandComp.compressor.setRelease(static_cast<float>(sliders[14]->getValue())); },
                             "CompHighRelease");
    // ========================================================================================================================== //
    // CompHighThreshold
    createAndAddCustomSlider(-60.0f,
                             12.0f,
                             1.0f,
                             0.0f,
                             "",
                             0,
                             [this, &highBandComp] { highBandComp.compressor.setThreshold(static_cast<float>(sliders[15]->getValue())); },
                             "CompHighThreshold");
    // ========================================================================================================================== //
    // CompHighRatio
    createAndAddCustomSlider(1.0f,
                             100.0f,
                             0.5f,
                             3.0f,
                             "",
                             1,
                             [this, &highBandComp] { highBandComp.compressor.setRatio(static_cast<float>(sliders[16]->getValue())); },
                             "CompHighRatio");
    // ========================================================================================================================== //
    // ---------------------------------------------------- CROSSOVERS ---------------------------------------------------------- //
    // lowmidcrossover
    createAndAddCustomSlider(20.0f,
                             999.0f,
                             1.0f,
                             400.0f,
                             " hz",
                             0,
                             [this, &lowMidCrossover] { lowMidCrossover = static_cast<float>(sliders[17]->getValue()); },
                             "LowMidCrossover");
    // ========================================================================================================================== //
    // midhighcrossover
    createAndAddCustomSlider(1000.0f,
                             20000.0f,
                             1.0f,
                             2000.0f,
                             " hz",
                             0,
                             [this, &midHighCrossover] { midHighCrossover = static_cast<float>(sliders[18]->getValue()); },
                             "MidHighCrossover");
    // ========================================================================================================================== //
    // ---------------------------------------------------- eq settings --------------------------------------------------------- //
    // lowCutFreq
    createAndAddCustomSlider(20.0f,
                             20000.0f,
                             1.0f,
                             20.0f,
                             " hz",
                             0,
                             [this] { chainSettings.lowCutFreq = static_cast<float>(sliders[19]->getValue()); },
                             "lowCutFreq");
    // ========================================================================================================================== //
    // HighCutFreq
    createAndAddCustomSlider(20.0f,
                             20000.0f,
                             1.0f,
                             20000.0f,
                             " hz",
                             0,
                             [this] { chainSettings.highCutFreq = static_cast<float>(sliders[20]->getValue()); },
                             "HighCutFreq");
    // ========================================================================================================================== //
    // PeakFreq
    createAndAddCustomSlider(20.0f,
                             20000.0f,
                             1.0f,
                             750.0f,
                             " hz",
                             0,
                             [this] { chainSettings.peakFreq = static_cast<float>(sliders[21]->getValue()); },
                             "PeakFreq");
    // ========================================================================================================================== //
    // PeakGainIndecibels
    createAndAddCustomSlider(-24.0f,
                             24.0f,
                             1.0f,
                             0.0f,
                             " db",
                             1,
                             [this] { chainSettings.peakGainInDecibels = static_cast<float>(sliders[22]->getValue()); },
                             "PeakGainIndecibels");
    // ========================================================================================================================== //
    // PeakQuality
    createAndAddCustomSlider(0.1f,
                             10.0f,
                             0.05f,
                             1.0f,
                             "",
                             2,
                             [this] { chainSettings.peakQuality = static_cast<float>(sliders[23]->getValue()); },
                             "PeakQuality");
    // ========================================================================================================================== //
    // LowCutSlope
    createAndAddCustomSlider(12.0f,
                             48.0f,
                             12.0f,
                             12.0f,
                             " db/Oct",
                             0,
                             [this] { chainSettings.lowCutSlope = static_cast<SlopeEQ>(sliders[24]->getValue() / 12 - 1); },
                             "LowCutSlope");
    // ========================================================================================================================== //
    // HighCutSlope
    createAndAddCustomSlider(12.0f,
                             48.0f,
                             12.0f,
                             12.0f,
                             " db/Oct",
                             0,
                             [this] { chainSettings.highCutSlope = static_cast<SlopeEQ>(sliders[25]->getValue() / 12 - 1); },
                             "HighCutSlope");
    // ========================================================================================================================== //

    //auto* gainSlider2 = sliders.getFirst();
    ////gainSlider2->slider.setRange(0, 100, 0.1);
    //gainSlider2->slider.onValueChange = [this, &gain] { gain.setGainDecibels(sliders.getFirst()->slider.getValue()); };

    //// Erste Gruppe von Checkboxen
    //auto* checkbox1 = new CustomCheckbox("CompAllBypass");
    //checkboxes.add(checkbox1);
    //componentContainer->addAndMakeVisible(checkbox1);

    //auto* checkbox2 = new CustomCheckbox("CompAllMute");
    //checkboxes.add(checkbox2);
    //componentContainer->addAndMakeVisible(checkbox2);

    //// F�ge die zweite Gruppe von Slidern hinzu
    //for (int i = 0; i < 4; ++i)
    //{
    //    auto* sliderWithLabel = new CustomSliderWithLabel(stringArray2[i]);
    //    sliders.add(sliderWithLabel);
    //    //sliderWithLabel->slider.addListener(this);
    //    componentContainer->addAndMakeVisible(sliderWithLabel);
    //}

    //// Zweite Gruppe von Checkboxen
    //auto* checkbox3 = new CustomCheckbox("CompGroupBypass");
    //checkboxes.add(checkbox3);
    //componentContainer->addAndMakeVisible(checkbox3);

    //auto* checkbox4 = new CustomCheckbox("CompGroupMute");
    //checkboxes.add(checkbox4);
    //componentContainer->addAndMakeVisible(checkbox4);

    //auto* checkbox5 = new CustomCheckbox("CompGroupSolo");
    //checkboxes.add(checkbox5);
    //componentContainer->addAndMakeVisible(checkbox5);

    //// Dritte Gruppe von Slidern
    //for (int i = 0; i < 4; ++i)
    //{
    //    auto* sliderWithLabel = new CustomSliderWithLabel(stringArray3[i]);
    //    sliders.add(sliderWithLabel);
    //    //sliderWithLabel->slider.addListener(this);
    //    componentContainer->addAndMakeVisible(sliderWithLabel);
    //}

    //// Dritte Gruppe von Checkboxen
    //auto* checkbox6 = new CustomCheckbox("CompMidBypassed");
    //checkboxes.add(checkbox6);
    //componentContainer->addAndMakeVisible(checkbox6);

    //auto* checkbox7 = new CustomCheckbox("CompMidMute");
    //checkboxes.add(checkbox7);
    //componentContainer->addAndMakeVisible(checkbox7);

    //auto* checkbox8 = new CustomCheckbox("CompMidSolo");
    //checkboxes.add(checkbox8);
    //componentContainer->addAndMakeVisible(checkbox8);

    //// Vierte Gruppe von Slidern
    //for (int i = 0; i < 4; ++i)
    //{
    //    auto* sliderWithLabel = new CustomSliderWithLabel(stringArray4[i]);
    //    sliders.add(sliderWithLabel);
    //    //sliderWithLabel->slider.addListener(this);
    //    componentContainer->addAndMakeVisible(sliderWithLabel);
    //}

    //// Vierte Gruppe von Checkboxen
    //auto* checkbox9 = new CustomCheckbox("CompHighBypassed");
    //checkboxes.add(checkbox9);
    //componentContainer->addAndMakeVisible(checkbox9);

    //auto* checkbox10 = new CustomCheckbox("CompHighMute");
    //checkboxes.add(checkbox10);
    //componentContainer->addAndMakeVisible(checkbox10);

    //auto* checkbox11 = new CustomCheckbox("CompHighSolo");
    //checkboxes.add(checkbox11);
    //componentContainer->addAndMakeVisible(checkbox11);

    //// F�nfte Gruppe von Slidern (9 Slider)
    //for (int i = 0; i < 9; ++i)
    //{
    //    auto* sliderWithLabel = new CustomSliderWithLabel(stringArray5[i]);
    //    sliders.add(sliderWithLabel);
    //    //sliderWithLabel->slider.addListener(this);
    //    componentContainer->addAndMakeVisible(sliderWithLabel);
    //}

    //// F�ge den Container in den Viewport und den Viewport in die Hauptkomponente ein
    addAndMakeVisible(viewport);
    viewport.setSize(getLocalBounds().getWidth(), getLocalBounds().getHeight());
    viewport.setViewedComponent(componentContainer.get(), false);
    viewport.setScrollBarsShown(true, false); // Zeige nur den vertikalen Scrollbalken an
    componentContainer->setSize(getWidth() - 50, 2000);
    
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
    g.drawRect (getLocalBounds(), 4);   // draw an outline around the component

    //g.setColour (juce::Colours::white);
    //g.setFont (juce::FontOptions (14.0f));
    //g.drawText ("ConfigComponent", getLocalBounds(),
                //juce::Justification::centred, true);   // draw some placeholder text
}

void ConfigComponent::resized()
{
    auto rect = getLocalBounds();
    rect.removeFromTop(10);
    viewport.setBounds(rect);
    rect.removeFromRight(viewport.getScrollBarThickness() + 40);

    
    //int availableWidth = getWidth() - viewport.getScrollBarThickness();
    //componentContainer->setSize(availableWidth - 50, 2000);
    componentContainer->setBounds(rect.expanded(0, 1000).withZeroOrigin());
    int compHeight = 20;
    auto sliderRect = componentContainer->getBounds();
    auto yOffset = sliderRect.getY() + 30;

    for (auto *slider : sliders)
    {
        slider->setBounds(20, yOffset, sliderRect.getWidth(), compHeight);
        yOffset += compHeight*2 + 10;
    }

    //viewport.setBounds(getLocalBounds());
//    viewport.setBounds(getLocalBounds());
//
//    int boxHeight = 100;      // Gesamth�he f�r die Box mit Label und Slider
//    int checkboxHeight = 50;  // H�he f�r die Checkboxen (inkl. Rahmen)
//    int padding = 10;         // Abstand zwischen den Komponenten
//    int extraPaddingForSliders = 30;  // Zus�tzlicher Abstand f�r Slider unter der Checkbox
//
//    // Berechnung der H�he f�r jede Gruppe
//    int totalHeightForFirstSliders = (boxHeight + padding) * 5;
//    int totalHeightForCheckboxes1 = 2 * (checkboxHeight + padding); // Erste Checkbox-Gruppe
//    int totalHeightForSecondSliders = (boxHeight + padding) * 4;
//    int totalHeightForCheckboxes2 = 3 * (checkboxHeight + padding); // Zweite Checkbox-Gruppe
//    int totalHeightForThirdSliders = (boxHeight + padding) * 4;
//    int totalHeightForCheckboxes3 = 3 * (checkboxHeight + padding); // Dritte Checkbox-Gruppe
//    int totalHeightForFourthSliders = (boxHeight + padding) * 4;
//    int totalHeightForCheckboxes4 = 3 * (checkboxHeight + padding); // Vierte Checkbox-Gruppe
//    int totalHeightForFifthSliders = (boxHeight + padding) * 9;     // F�nfte Slider-Gruppe
//
//    int totalHeight = totalHeightForFirstSliders + totalHeightForCheckboxes1 +
//        totalHeightForSecondSliders + totalHeightForCheckboxes2 +
//        totalHeightForThirdSliders + totalHeightForCheckboxes3 +
//        totalHeightForFourthSliders + totalHeightForCheckboxes4 +
//        totalHeightForFifthSliders + padding;
//
//    int availableWidth = getWidth() - viewport.getScrollBarThickness();
//
//    componentContainer->setSize(availableWidth, totalHeight);
//
//    // Erste Slider-Gruppe
//    int yOffset = 0;
//    for (int i = 0; i < 5; ++i)
//    {
//        sliders[i]->setBounds(10, yOffset, availableWidth - 20, boxHeight);
//        yOffset += boxHeight + padding;
//    }
//
//    // Erste Checkbox-Gruppe
//    for (int i = 0; i < 2; ++i)
//    {
//        checkboxes[i]->setBounds(10, yOffset, availableWidth - 20, checkboxHeight);
//        yOffset += checkboxHeight + padding;
//    }
//
//    // Zweite Slider-Gruppe
//    for (int i = 5; i < 9; ++i)
//    {
//        sliders[i]->setBounds(10, yOffset, availableWidth - 20, boxHeight);
//        yOffset += boxHeight + padding;
//    }
//
//    // Zweite Checkbox-Gruppe
//    for (int i = 2; i < 5; ++i)
//    {
//        checkboxes[i]->setBounds(10, yOffset, availableWidth - 20, checkboxHeight);
//        yOffset += checkboxHeight + padding;
//    }
//
//    // Dritte Slider-Gruppe
//    for (int i = 9; i < 13; ++i)
//    {
//        sliders[i]->setBounds(10, yOffset, availableWidth - 20, boxHeight);
//        yOffset += boxHeight + padding;
//    }
//
//    // Dritte Checkbox-Gruppe
//    for (int i = 5; i < 8; ++i)
//    {
//        checkboxes[i]->setBounds(10, yOffset, availableWidth - 20, checkboxHeight);
//        yOffset += checkboxHeight + padding;
//    }
//
//    // Vierte Slider-Gruppe
//    for (int i = 13; i < 17; ++i)
//    {
//        sliders[i]->setBounds(10, yOffset, availableWidth - 20, boxHeight);
//        yOffset += boxHeight + padding;
//    }
//
//    // Vierte Checkbox-Gruppe
//    for (int i = 8; i < 11; ++i)
//    {
//        checkboxes[i]->setBounds(10, yOffset, availableWidth - 20, checkboxHeight);
//        yOffset += checkboxHeight + padding;
//    }
//
//    // F�nfte Slider-Gruppe
//    for (int i = 17; i < 26; ++i)
//    {
//        sliders[i]->setBounds(10, yOffset, availableWidth - 20, boxHeight);
//        yOffset += boxHeight + padding;
//    }
//}
//
//void ConfigComponent::onGainSliderChange(float dbs)
//{
//    gain.setGainDecibels(dbs);
}

void ConfigComponent::createAndAddCustomSlider(float minRange, float maxRange, float stepSize, float startValue, const juce::String suffix, int decimalPlaces, std::function<void()> lambda, const juce::String labelText)
{
    float fontSize = 20.0f;

    // Slider
    auto *slider = new juce::Slider();
    slider->setSliderStyle(juce::Slider::LinearHorizontal);
    slider->setRange(minRange, maxRange, stepSize);
    slider->setValue(startValue);
    slider->setTextValueSuffix(suffix);
    slider->setTextBoxStyle(juce::Slider::TextBoxLeft, false, slider->getTextBoxWidth(), slider->getTextBoxHeight());
    slider->setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
    slider->setNumDecimalPlacesToDisplay(decimalPlaces);
    sliders.add(slider);
    slider->onValueChange = lambda;
    componentContainer->addAndMakeVisible(slider);
    // Label
    auto *label = new juce::Label();
    label->setText(labelText, juce::dontSendNotification);
    label->setColour(juce::Label::textColourId, juce::Colours::black);
    label->setFont(juce::Font(fontSize, juce::Font::bold));
    label->attachToComponent(slider, false);
}
