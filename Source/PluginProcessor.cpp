#include "PluginProcessor.h"
#include "PluginEditor.h"

SimplePassAudioProcessor::SimplePassAudioProcessor()
    : AudioProcessor(
        #ifndef JucePlugin_PreferredChannelConfigurations
        BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
        #endif
    )
{
}

SimplePassAudioProcessor::~SimplePassAudioProcessor() {}

const juce::String SimplePassAudioProcessor::getName() const { return "SimplePass"; }

bool SimplePassAudioProcessor::acceptsMidi() const { return false; }
bool SimplePassAudioProcessor::producesMidi() const { return false; }
bool SimplePassAudioProcessor::isMidiEffect() const { return false; }
double SimplePassAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int SimplePassAudioProcessor::getNumPrograms() { return 1; }
int SimplePassAudioProcessor::getCurrentProgram() { return 0; }
void SimplePassAudioProcessor::setCurrentProgram(int) {}
const juce::String SimplePassAudioProcessor::getProgramName(int) { return {}; }
void SimplePassAudioProcessor::changeProgramName(int, const juce::String&) {}

void SimplePassAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec{ sampleRate, static_cast<juce::uint32>(samplesPerBlock), 1 };
    leftChain.prepare(spec);
    rightChain.prepare(spec);
    updateFilters();
}

void SimplePassAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimplePassAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    #if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    #endif
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}
#endif

void SimplePassAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    updateFilters();

    juce::dsp::AudioBlock<float> block(buffer);
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);
}

bool SimplePassAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* SimplePassAudioProcessor::createEditor()
{
    //return new juce::GenericAudioProcessorEditor(*this);
    //USE CUSTOM GUI
    return new SimplePassAudioProcessorEditor(*this);
}


void SimplePassAudioProcessor::getStateInformation(juce::MemoryBlock&) {}
void SimplePassAudioProcessor::setStateInformation(const void*, int) {}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings settings;
    settings.freq = apvts.getRawParameterValue("Freq")->load();
    settings.slope = static_cast<Slope>(apvts.getRawParameterValue("Slope")->load());
    return settings;
}

void SimplePassAudioProcessor::updateCoefficients(Coefficients& old, const Coefficients& replacements)
{
    *old = *replacements;
}

void SimplePassAudioProcessor::updateFilters()
{
    auto settings = getChainSettings(apvts);
    auto coeffs = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(settings.freq, getSampleRate(), (settings.slope + 1) * 2);
    updateCutFilter(leftChain, coeffs, settings.slope);
    updateCutFilter(rightChain, coeffs, settings.slope);
}

juce::AudioProcessorValueTreeState::ParameterLayout SimplePassAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("Freq", 1), "Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f), 20000.f));

    juce::StringArray slopeChoices{ "12 dB/Oct", "24 dB/Oct" };
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("Slope", 1), "Slope", slopeChoices, 0));

    return layout;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimplePassAudioProcessor();
}
