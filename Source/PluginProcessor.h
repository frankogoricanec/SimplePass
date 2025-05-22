#pragma once

#include <JuceHeader.h>

enum Slope
{
    Slope_12,
    Slope_24
};

struct ChainSettings
{
    float freq{0};
    Slope slope{Slope_12};
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

class SimplePassAudioProcessor : public juce::AudioProcessor
{
public:
    SimplePassAudioProcessor();
    ~SimplePassAudioProcessor() override;

    void prepareToPlay(double, int) override;
    void releaseResources() override;

    #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    #endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int) override;
    const juce::String getProgramName(int) override;
    void changeProgramName(int, const juce::String&) override;

    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;
    

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };

private:
    using Filter = juce::dsp::IIR::Filter<float>;
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter>;
    CutFilter leftChain, rightChain;

    enum ChainPositions { Cut };

    using Coefficients = Filter::CoefficientsPtr;
    static void updateCoefficients(Coefficients& old, const Coefficients& replacements);

    template<int Index, typename ChainType, typename CoefficientType>
    void update(ChainType& chain, const CoefficientType& coefficients)
    {
        updateCoefficients(chain.template get<Index>().coefficients, coefficients[Index]);
        chain.template setBypassed<Index>(false);
    }

    template<typename ChainType, typename CoefficientType>
    void updateCutFilter(ChainType& chain, const CoefficientType& coeffs, const Slope& slope)
    {
        chain.template setBypassed<0>(true);
        chain.template setBypassed<1>(true);

        switch (slope) {
            case Slope_24: update<1>(chain, coeffs);
            case Slope_12: update<0>(chain, coeffs);
        }
    }

    void updateFilters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimplePassAudioProcessor)
};
