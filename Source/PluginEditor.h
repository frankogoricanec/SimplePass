#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class SimplePassAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    SimplePassAudioProcessorEditor(SimplePassAudioProcessor&);
    ~SimplePassAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    juce::LookAndFeel_V4 neomorphicLookAndFeel;

private:
    SimplePassAudioProcessor& audioProcessor;

    juce::Slider freqSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqAttachment;

    juce::TextButton slopeButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> slopeAttachment;
    std::function<void()> toggleSlope;

    juce::Image logoImage;
    juce::Rectangle<int> logoArea;

    void styleNeomorphicSlider(juce::Slider&);
    void styleNeomorphicButton(juce::TextButton&);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimplePassAudioProcessorEditor)
};
