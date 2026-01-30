#include "PluginProcessor.h"
#include "PluginEditor.h"

TapeWarmAudioProcessor::TapeWarmAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
       apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // Get parameter pointers
    inputDrive = apvts.getRawParameterValue("inputDrive");
    saturation = apvts.getRawParameterValue("saturation");
    warmth = apvts.getRawParameterValue("warmth");
    headBump = apvts.getRawParameterValue("headBump");
    bumpFreq = apvts.getRawParameterValue("bumpFreq");
    wow = apvts.getRawParameterValue("wow");
    flutter = apvts.getRawParameterValue("flutter");
    hiss = apvts.getRawParameterValue("hiss");
    output = apvts.getRawParameterValue("output");
    mix = apvts.getRawParameterValue("mix");
    age = apvts.getRawParameterValue("age");
    bias = apvts.getRawParameterValue("bias");
    machineType = apvts.getRawParameterValue("machineType");
    tapeType = apvts.getRawParameterValue("tapeType");
}

TapeWarmAudioProcessor::~TapeWarmAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout TapeWarmAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Input Drive: -12 to +12 dB
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("inputDrive", 1), "Input Drive",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    // Saturation: 0-100%
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("saturation", 1), "Saturation",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Warmth: 0-100%
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("warmth", 1), "Warmth",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Head Bump: 0-100%
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("headBump", 1), "Head Bump",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Bump Frequency: 40-150 Hz
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("bumpFreq", 1), "Bump Freq",
        juce::NormalisableRange<float>(40.0f, 150.0f, 1.0f), 80.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    // Wow: 0-100%
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("wow", 1), "Wow",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Flutter: 0-100%
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("flutter", 1), "Flutter",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Hiss: 0-100%
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("hiss", 1), "Hiss",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Output: -12 to +12 dB
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("output", 1), "Output",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    // Mix: 0-100%
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("mix", 1), "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Age: 0-100%
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("age", 1), "Age",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Bias: 0-100%
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("bias", 1), "Bias",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Machine Type: 7.5 IPS, 15 IPS, 30 IPS
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("machineType", 1), "Machine",
        juce::StringArray{ "7.5 IPS", "15 IPS", "30 IPS" }, 1));

    // Tape Type: Type I, Type II, Modern
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("tapeType", 1), "Tape",
        juce::StringArray{ "Type I (Ferric)", "Type II (Chrome)", "Modern" }, 0));

    return { params.begin(), params.end() };
}

const juce::String TapeWarmAudioProcessor::getName() const { return JucePlugin_Name; }
bool TapeWarmAudioProcessor::acceptsMidi() const { return false; }
bool TapeWarmAudioProcessor::producesMidi() const { return false; }
bool TapeWarmAudioProcessor::isMidiEffect() const { return false; }
double TapeWarmAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int TapeWarmAudioProcessor::getNumPrograms() { return 1; }
int TapeWarmAudioProcessor::getCurrentProgram() { return 0; }
void TapeWarmAudioProcessor::setCurrentProgram(int index) { juce::ignoreUnused(index); }
const juce::String TapeWarmAudioProcessor::getProgramName(int index) { juce::ignoreUnused(index); return {}; }
void TapeWarmAudioProcessor::changeProgramName(int index, const juce::String& newName) { juce::ignoreUnused(index, newName); }

void TapeWarmAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    tapeProcessor.prepare(sampleRate, samplesPerBlock);
}

void TapeWarmAudioProcessor::releaseResources()
{
    tapeProcessor.reset();
}

bool TapeWarmAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    return true;
}

void TapeWarmAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Update tape processor parameters
    tapeProcessor.setInputDrive(inputDrive->load());
    tapeProcessor.setSaturation(saturation->load());
    tapeProcessor.setWarmth(warmth->load());
    tapeProcessor.setHeadBump(headBump->load());
    tapeProcessor.setBumpFreq(bumpFreq->load());
    tapeProcessor.setWow(wow->load());
    tapeProcessor.setFlutter(flutter->load());
    tapeProcessor.setHiss(hiss->load());
    tapeProcessor.setOutput(output->load());
    tapeProcessor.setMix(mix->load());
    tapeProcessor.setAge(age->load());
    tapeProcessor.setBias(bias->load());
    tapeProcessor.setMachineType(static_cast<int>(machineType->load()));
    tapeProcessor.setTapeType(static_cast<int>(tapeType->load()));

    // Process audio
    tapeProcessor.process(buffer);
}

void TapeWarmAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void TapeWarmAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor* TapeWarmAudioProcessor::createEditor()
{
    return new TapeWarmAudioProcessorEditor(*this);
}

bool TapeWarmAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TapeWarmAudioProcessor();
}
