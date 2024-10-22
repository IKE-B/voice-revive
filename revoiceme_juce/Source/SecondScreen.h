#pragma once
#include <JuceHeader.h>

//==============================================================================
// SecondScreen
//==============================================================================

class SecondScreen : public juce::Component
{
public:
    SecondScreen()
    {
        // Hier könnten zukünftige UI-Elemente hinzugefügt werden
    }

    ~SecondScreen() override = default;

    void paint(juce::Graphics& g) override
    {
        // Hintergrundfarbe setzen
        g.fillAll(juce::Colours::lightgrey);

        // Text anzeigen
        g.setColour(juce::Colours::black);
        g.setFont(20.0f);
        g.drawText("This is the Second Screen", getLocalBounds(),
            juce::Justification::centred, true);
    }

    void resized() override
    {
        // Falls in der Zukunft UI-Elemente hinzugefügt werden, kann hier das Layout definiert werden.
    }
};
