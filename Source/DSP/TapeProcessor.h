#pragma once

#include <JuceHeader.h>
#include "DSPUtils.h"
#include <vector>
#include <random>

// Machine speed types
enum class MachineType
{
    IPS_7_5 = 0,   // 7.5 IPS - warmest, most head bump
    IPS_15,         // 15 IPS - balanced
    IPS_30          // 30 IPS - cleanest, most extended
};

// Tape formulation types
enum class TapeType
{
    TypeI = 0,      // Ferric - classic warm, more saturation
    TypeII,         // Chrome - brighter, cleaner
    Modern          // Modern formulation - extended response
};

class TapeProcessor
{
public:
    TapeProcessor();
    ~TapeProcessor() = default;

    void prepare(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    // Main controls
    void setInputDrive(float dB);       // -12 to +12 dB
    void setSaturation(float amount);   // 0-100%
    void setWarmth(float amount);       // 0-100%
    void setHeadBump(float amount);     // 0-100%
    void setBumpFreq(float freq);       // 40-150 Hz
    void setWow(float amount);          // 0-100%
    void setFlutter(float amount);      // 0-100%
    void setHiss(float amount);         // 0-100%
    void setOutput(float dB);           // -12 to +12 dB
    void setMix(float amount);          // 0-100%
    void setAge(float amount);          // 0-100%
    void setBias(float amount);         // 0-100%

    // Type selectors
    void setMachineType(int type);
    void setTapeType(int type);

    // Metering
    float getInputLevel() const { return inputLevel.load(); }
    float getOutputLevel() const { return outputLevel.load(); }

private:
    // Processing stages
    float processSaturation(float input, int channel);
    float processHeadBump(float input, int channel);
    float processHFRolloff(float input, int channel);
    float processWowFlutter(float input, int channel);
    float processHiss();

    // Filter coefficient updates
    void updateHeadBumpFilter();
    void updateHFRolloffFilter();
    void updateWowFlutterLFO();

    // Delay line for wow/flutter
    void writeToDelayLine(float sample, int channel);
    float readFromDelayLine(float delaySamples, int channel);

    // Parameters
    float inputDrive = 0.0f;        // dB
    float saturation = 50.0f;       // 0-100
    float warmth = 50.0f;           // 0-100
    float headBump = 50.0f;         // 0-100
    float bumpFreq = 80.0f;         // Hz
    float wow = 0.0f;               // 0-100
    float flutter = 0.0f;           // 0-100
    float hiss = 0.0f;              // 0-100
    float outputGain = 0.0f;        // dB
    float mix = 100.0f;             // 0-100
    float age = 0.0f;               // 0-100
    float bias = 50.0f;             // 0-100

    MachineType machineType = MachineType::IPS_15;
    TapeType tapeType = TapeType::TypeI;

    // Derived values
    float inputGainLinear = 1.0f;
    float outputGainLinear = 1.0f;
    float saturationAmount = 0.5f;
    float warmthAmount = 0.5f;
    float headBumpAmount = 0.5f;
    float wowDepth = 0.0f;
    float flutterDepth = 0.0f;
    float hissLevel = 0.0f;
    float mixAmount = 1.0f;
    float ageAmount = 0.0f;
    float biasAmount = 0.5f;

    // Sample rate and block size
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;

    // Saturation state (hysteresis)
    float hysteresisStateL = 0.0f;
    float hysteresisStateR = 0.0f;

    // Head bump filter (biquad peak/bell)
    struct BiquadState
    {
        float x1 = 0.0f, x2 = 0.0f;
        float y1 = 0.0f, y2 = 0.0f;
    };
    BiquadState headBumpStateL, headBumpStateR;
    float headBumpB0 = 1.0f, headBumpB1 = 0.0f, headBumpB2 = 0.0f;
    float headBumpA1 = 0.0f, headBumpA2 = 0.0f;

    // HF rolloff filter (one-pole lowpass per channel)
    float hfRolloffCoeffL = 0.5f;
    float hfRolloffStateL = 0.0f;
    float hfRolloffCoeffR = 0.5f;
    float hfRolloffStateR = 0.0f;

    // Warmth filter (low shelf)
    BiquadState warmthStateL, warmthStateR;
    float warmthB0 = 1.0f, warmthB1 = 0.0f, warmthB2 = 0.0f;
    float warmthA1 = 0.0f, warmthA2 = 0.0f;

    // Wow LFO (slow, 0.5-3 Hz)
    float wowPhase = 0.0f;
    float wowRate = 1.0f;       // Hz
    float wowPhaseIncrement = 0.0f;

    // Flutter LFO (fast, 5-30 Hz)
    float flutterPhase = 0.0f;
    float flutterRate = 10.0f;  // Hz
    float flutterPhaseIncrement = 0.0f;

    // Random modulation for realistic wow/flutter
    std::mt19937 rng;
    std::uniform_real_distribution<float> randomDist;
    float wowRandomOffset = 0.0f;
    float flutterRandomOffset = 0.0f;

    // Delay line for wow/flutter pitch modulation
    static const int MAX_DELAY_SAMPLES = 2048;  // ~46ms at 44.1kHz
    std::vector<float> delayLineL;
    std::vector<float> delayLineR;
    int writeIndex = 0;
    float baseDelayMs = 10.0f;  // Center delay for modulation

    // Noise generator for hiss
    DSPUtils::NoiseGenerator noiseGen;
    float hissFilterStateL = 0.0f;
    float hissFilterStateR = 0.0f;

    // Level metering
    std::atomic<float> inputLevel { 0.0f };
    std::atomic<float> outputLevel { 0.0f };
};
