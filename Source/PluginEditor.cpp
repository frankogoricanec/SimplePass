#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "BinaryData.h"

SimplePassAudioProcessorEditor::SimplePassAudioProcessorEditor(SimplePassAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(400, 300);
    setResizable(false, false);

    // Rotary knob setup
    freqSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    freqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    freqSlider.setTextValueSuffix(" Hz");
    freqSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
    freqSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    styleNeomorphicSlider(freqSlider);
    addAndMakeVisible(freqSlider);

    freqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "Freq", freqSlider);

    slopeButton.setClickingTogglesState(true);
    slopeButton.setButtonText("12 dB/Oct");
    slopeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::white);
    slopeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    slopeButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    addAndMakeVisible(slopeButton);

    slopeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.apvts, "Slope", slopeButton);

    slopeButton.onClick = [this]()
    {
        int newVal = slopeButton.getToggleState() ? 1 : 0;
        audioProcessor.apvts.getParameter("Slope")->setValueNotifyingHost(static_cast<float>(newVal));
        slopeButton.setButtonText(newVal == 0 ? "12 dB/Oct" : "24 dB/Oct");

        if (newVal == 0)
        {
            slopeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::white);
            slopeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
            slopeButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
        }
        else
        {
            slopeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkblue);
            slopeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
            slopeButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        }
    };

    logoImage = juce::ImageFileFormat::loadFrom(BinaryData::logo_png, BinaryData::logo_pngSize);
}

SimplePassAudioProcessorEditor::~SimplePassAudioProcessorEditor()
{
    freqSlider.setLookAndFeel(nullptr);
}

void SimplePassAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);

    if (logoImage.isValid())
    {
        auto area = logoArea.toFloat();
        auto imageBounds = logoImage.getBounds();
        float imageAspect = static_cast<float>(imageBounds.getWidth()) / imageBounds.getHeight();
        float areaAspect = area.getWidth() / area.getHeight();

        juce::Rectangle<float> drawArea;

        if (imageAspect > areaAspect)
        {
            float scaledHeight = area.getWidth() / imageAspect;
            drawArea = { area.getX(), area.getY() + (area.getHeight() - scaledHeight) / 2,
                         area.getWidth(), scaledHeight };
        }
        else
        {
            float scaledWidth = area.getHeight() * imageAspect;
            drawArea = { area.getX() + (area.getWidth() - scaledWidth) / 2, area.getY(),
                         scaledWidth, area.getHeight() };
        }

        g.drawImage(logoImage, drawArea);
    }
}

void SimplePassAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    logoArea = area.removeFromTop(120);

    int knobSize = 100;
    int knobX = (getWidth() - knobSize) / 2;
    int knobY = logoArea.getBottom() - 30;

    freqSlider.setBounds(knobX, knobY, knobSize, knobSize);

    slopeButton.setBounds(getWidth() / 2 - 60, knobY + knobSize + 10, 120, 40);
}

void SimplePassAudioProcessorEditor::styleNeomorphicSlider(juce::Slider& s)
{
    auto color = juce::Colour::fromRGB(225, 230, 235);
    s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::lightblue.darker(0.2f));
    s.setColour(juce::Slider::backgroundColourId, color);
    s.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white.darker(0.1f));
    s.setColour(juce::Slider::thumbColourId, juce::Colours::skyblue);
    s.setLookAndFeel(&neomorphicLookAndFeel);
}

void SimplePassAudioProcessorEditor::styleNeomorphicButton(juce::TextButton& b)
{
   
}
