

#include <JuceHeader.h>
#include "ConfigComponent.h"

//==============================================================================

// Konstruktor
ConfigComponent::ConfigComponent()
{
    // Initialisiere den Container für Slider und Labels
    componentContainer = std::make_unique<juce::Component>();


    const char* stringArray[] = { "Gain", "CompAllAttack", "CompAllRelease", "CompAllThreshold", "CompAllRatio" };


    // Füge mehrere Slider hinzu
    for (int i = 0; i < 5; ++i)
    {
        // Erstelle einen neuen Slider
        auto* slider = new juce::Slider();
        slider->setSliderStyle(juce::Slider::LinearHorizontal);
        slider->setRange(0, 10, 0.01); // Bereich von 0 bis 10 mit 2 Dezimalstellen
        slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        slider->setNumDecimalPlacesToDisplay(2);

        // Füge den Slider zum Array hinzu und mach ihn sichtbar
        sliders.add(slider);
        componentContainer->addAndMakeVisible(slider);

        // Erstelle ein Label für den Slider
        auto* label = new juce::Label();
        //label->setText("Slider " + juce::String(i + 1), juce::dontSendNotification);
        label->setText(stringArray[i], juce::dontSendNotification);
        label->setJustificationType(juce::Justification::centred);
        labels.add(label);
        componentContainer->addAndMakeVisible(label);
    }

    // Füge den Container in den Viewport und den Viewport in die Hauptkomponente ein
    viewport.setViewedComponent(componentContainer.get());
    viewport.setScrollBarsShown(true, false); // Zeige nur den vertikalen Scrollbalken an, keinen horizontalen
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
    //g.drawText("Settings", getLocalBounds().removeFromTop(20), juce::Justification::centredTop);
}

void ConfigComponent::resized()
{
    // Setze den Bereich des Viewports auf die Größe der Hauptkomponente
    viewport.setBounds(getLocalBounds());

    // Definiere die Größe des Containers basierend auf der Anzahl der Slider
    int boxHeight = 100;  // Gesamthöhe für die Box mit Label und Slider
    int padding = 10;     // Abstand zwischen dem Inhalt (Label/Slider) und dem Rand
    int totalHeight = (boxHeight + padding) * sliders.size(); // Berechne die Gesamthöhe für alle Boxen
    int availableWidth = getWidth() - viewport.getScrollBarThickness(); // Verhindere horizontalen Scrollbalken

    componentContainer->setSize(availableWidth, totalHeight);

    // Setze die Positionen der Slider und Labels im Container
    for (int i = 0; i < sliders.size(); ++i)
    {
        int yOffset = i * (boxHeight + padding); // Y-Offset für die Box (Label + Slider)

        // Berechne den Bereich für die Box mit Padding
        auto boxBounds = juce::Rectangle<int>(10, yOffset, availableWidth - 20, boxHeight).reduced(padding);

        // Label über dem Slider, innerhalb der Box
        labels[i]->setBounds(boxBounds.withHeight(20));

        // Slider unter dem Label, innerhalb der Box
        sliders[i]->setBounds(boxBounds.withTrimmedTop(20).reduced(padding));
    }
}

void ConfigComponent::paintOverChildren(juce::Graphics& g)
{
    g.setColour(juce::Colours::black); // Farbe für den Rand

    // Zeichne Ränder um die Slider und Labels (Boxen)
    for (int i = 0; i < sliders.size(); ++i)
    {
        // Berechne die Box, die sowohl das Label als auch den Slider umschließt
        int boxHeight = 100;   // Höhe der Box für Label und Slider
        int yOffset = i * (boxHeight + 10); // Y-Offset für die Box
        auto boxBounds = juce::Rectangle<int>(10, yOffset, getWidth() - 20 - viewport.getScrollBarThickness(), boxHeight).reduced(5);

        // Berücksichtige die Scroll-Position des Viewports
        boxBounds.translate(-viewport.getViewPositionX(), -viewport.getViewPositionY());

        // Zeichne den Rand um die Box
        g.drawRect(boxBounds, 2); // Zeichne einen 2 Pixel breiten Rand um die Box
    }
}
