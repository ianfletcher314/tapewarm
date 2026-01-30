#pragma once

#include <JuceHeader.h>
#include "DSP/TapeProcessor.h"

class TapeWarmAudioProcessor : public juce::AudioProcessor
{
public:
    TapeWarmAudioProcessor();
    ~TapeWarmAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    // Level metering
    float getInputLevel() const { return tapeProcessor.getInputLevel(); }
    float getOutputLevel() const { return tapeProcessor.getOutputLevel(); }

private:
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // DSP
    TapeProcessor tapeProcessor;

    // Parameter pointers for fast access
    std::atomic<float>* inputDrive = nullptr;
    std::atomic<float>* saturation = nullptr;
    std::atomic<float>* warmth = nullptr;
    std::atomic<float>* headBump = nullptr;
    std::atomic<float>* bumpFreq = nullptr;
    std::atomic<float>* wow = nullptr;
    std::atomic<float>* flutter = nullptr;
    std::atomic<float>* hiss = nullptr;
    std::atomic<float>* output = nullptr;
    std::atomic<float>* mix = nullptr;
    std::atomic<float>* age = nullptr;
    std::atomic<float>* bias = nullptr;
    std::atomic<float>* machineType = nullptr;
    std::atomic<float>* tapeType = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TapeWarmAudioProcessor)
};
