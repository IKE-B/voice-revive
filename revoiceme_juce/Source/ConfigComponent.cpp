#include <JuceHeader.h>
#include "ConfigComponent.h"

//==============================================================================
// Konstruktor
ConfigComponent::ConfigComponent(ChainSettingsEQ &chSe,
                                 juce::dsp::Gain<float> &gain,
                                 juce::dsp::Compressor<float> &compressorAll,
                                 bool &compAllBypassed,
                                 CompressorBand &lowBandComp,
                                 CompressorBand &midBandComp,
                                 CompressorBand &highBandComp,
                                 float &lowMidCrossover,
                                 float &midHighCrossover)
    : chainSettings {chSe},
    gain {gain},
    compressorAll {compressorAll},
    compAllBypassed {compAllBypassed},
    lowBandComp {lowBandComp},
    midBandComp {midBandComp},
    highBandComp {highBandComp},
    lowMidCrossover {lowMidCrossover},
    midHighCrossover {midHighCrossover}
{
    // Initialisiere den Container f�r Slider und Labels
    componentContainer = std::make_unique<juce::Component>();
    
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
    // CompAllBypassed
    createAndAddCustomCheckbox([this, &compAllBypassed] { compAllBypassed = checkboxes[0]->getToggleState(); },
        "CompAllBypassed");
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
    // CompLowBypassed
    createAndAddCustomCheckbox([this, &lowBandComp] { lowBandComp.bypassed = checkboxes[1]->getToggleState(); },
        "CompLowBypassed");
    // ========================================================================================================================== //
    // CompLowMute
    createAndAddCustomCheckbox([this, &lowBandComp] { lowBandComp.mute = checkboxes[2]->getToggleState(); },
        "CompLowMute");
    // // ========================================================================================================================== //
    // CompLowSolo
    createAndAddCustomCheckbox([this, &lowBandComp] { lowBandComp.solo = checkboxes[3]->getToggleState(); },
        "CompLowSolo");
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
    // CompMidBypassed
    createAndAddCustomCheckbox([this, &midBandComp] { midBandComp.bypassed = checkboxes[4]->getToggleState(); },
        "CompMidBypassed");
    // ========================================================================================================================== //
    // CompMidMute
    createAndAddCustomCheckbox([this, &midBandComp] { midBandComp.mute = checkboxes[5]->getToggleState(); },
        "CompMidMute");
    // // ========================================================================================================================== //
    // CompMidSolo
    createAndAddCustomCheckbox([this, &midBandComp] { midBandComp.solo = checkboxes[6]->getToggleState(); },
        "CompMidSolo");
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
    // CompHighBypassed
    createAndAddCustomCheckbox([this, &highBandComp] { highBandComp.bypassed = checkboxes[7]->getToggleState(); },
        "CompHighBypassed");
    // ========================================================================================================================== //
    // CompHighMute
    createAndAddCustomCheckbox([this, &highBandComp] { highBandComp.mute = checkboxes[8]->getToggleState(); },
        "CompHighMute");
    // // ========================================================================================================================== //
    // CompHighSolo
    createAndAddCustomCheckbox([this, &highBandComp] { highBandComp.solo = checkboxes[9]->getToggleState(); },
        "CompHighSolo");
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
    // Füge den Container in den Viewport und den Viewport in die Hauptkomponente ein
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
    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 4);   // draw an outline around the component
}

void ConfigComponent::resized()
{
    auto rect = getLocalBounds();
    rect.removeFromTop(10);
    viewport.setBounds(rect);
    rect.removeFromRight(viewport.getScrollBarThickness() + 40);

    componentContainer->setBounds(rect.expanded(0, 1000).withZeroOrigin());
    int compHeight = 20;
    auto sliderRect = componentContainer->getBounds();
    auto yOffset = sliderRect.getY() + 30;

    for (auto *slider : sliders)
    {
        slider->setBounds(20, yOffset, sliderRect.getWidth(), compHeight);
        yOffset += compHeight*2 + 10;
    }

    for (auto* button : checkboxes)
    {
        button->setBounds(20, yOffset, sliderRect.getWidth(), compHeight);
        yOffset += compHeight * 2 + 10;
    }
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

void ConfigComponent::createAndAddCustomCheckbox(std::function<void()> lambda, const juce::String labelText)
{
    //float fontSize = 20.0f;

    // Button
    auto* button = new juce::ToggleButton(labelText);
    button->onClick = lambda;
    button->setColour(juce::ToggleButton::textColourId, juce::Colours::black);
    button->setColour(juce::ToggleButton::tickColourId, juce::Colours::black);
    checkboxes.add(button);
    componentContainer->addAndMakeVisible(button);
}
