#pragma once

#include <JuceHeader.h>
#include "HelperStructs.h"

// ConfigComponent Klasse
class ConfigComponent : public juce::Component
{
public:
    ConfigComponent(ChainSettingsEQ &chSe,
                    juce::dsp::Gain<float> &gain,
                    juce::dsp::Compressor<float> &compressorAll,
                    bool &compAllBypassed,
                    CompressorBand &lowBandComp,
                    CompressorBand &midBandComp,
                    CompressorBand &highBandComp,
                    float &lowMidCrossover,
                    float &midHighCrossover);
    ~ConfigComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    //void sliderValueChanged(juce::Slider *slider) override;
    //void paintOverChildren(juce::Graphics& g) override;

private:
    void createAndAddCustomSlider(float minRange,
                                  float maxRange,
                                  float stepSize,
                                  float startValue,
                                  const juce::String suffix,
                                  int decimalPlaces,
                                  std::function<void()> lambda,
                                  const juce::String labelText);

    void createAndAddCustomCheckbox(std::function<void()> lambda,
                                    const juce::String labelText);

    juce::Viewport viewport; // Der Viewport für das Scrollen
    std::unique_ptr<juce::Component> componentContainer; // Container für die Slider und Labels

    juce::OwnedArray<juce::Slider> sliders;
    juce::OwnedArray<juce::ToggleButton> checkboxes;

    //Audioparameter
    ChainSettingsEQ &chainSettings;
    juce::dsp::Gain<float> &gain;
    juce::dsp::Compressor<float> &compressorAll;
    bool &compAllBypassed;
    CompressorBand &lowBandComp;
    CompressorBand &midBandComp;
    CompressorBand &highBandComp;
    float &lowMidCrossover;
    float &midHighCrossover;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigComponent)
};
