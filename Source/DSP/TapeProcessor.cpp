#include "TapeProcessor.h"
#include <cmath>

TapeProcessor::TapeProcessor()
    : rng(std::random_device{}()),
      randomDist(-1.0f, 1.0f)
{
}

void TapeProcessor::prepare(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;

    // Initialize delay lines
    int delaySize = static_cast<int>(sampleRate * 0.05);  // 50ms max
    delayLineL.resize(delaySize, 0.0f);
    delayLineR.resize(delaySize, 0.0f);
    writeIndex = 0;

    // Update all filter coefficients
    updateHeadBumpFilter();
    updateHFRolloffFilter();
    updateWowFlutterLFO();

    reset();
}

void TapeProcessor::reset()
{
    // Reset saturation state
    hysteresisStateL = 0.0f;
    hysteresisStateR = 0.0f;

    // Reset head bump filters
    headBumpStateL = {};
    headBumpStateR = {};

    // Reset HF rolloff filters
    hfRolloffStateL = 0.0f;
    hfRolloffStateR = 0.0f;

    // Reset warmth filters
    warmthStateL = {};
    warmthStateR = {};

    // Reset delay lines
    std::fill(delayLineL.begin(), delayLineL.end(), 0.0f);
    std::fill(delayLineR.begin(), delayLineR.end(), 0.0f);
    writeIndex = 0;

    // Reset LFO phases
    wowPhase = 0.0f;
    flutterPhase = 0.0f;

    // Reset hiss filter
    hissFilterStateL = 0.0f;
    hissFilterStateR = 0.0f;
}

void TapeProcessor::setInputDrive(float dB)
{
    inputDrive = std::clamp(dB, -12.0f, 12.0f);
    inputGainLinear = DSPUtils::decibelsToLinear(inputDrive);
}

void TapeProcessor::setSaturation(float amount)
{
    saturation = std::clamp(amount, 0.0f, 100.0f);
    saturationAmount = saturation / 100.0f;
}

void TapeProcessor::setWarmth(float amount)
{
    warmth = std::clamp(amount, 0.0f, 100.0f);
    warmthAmount = warmth / 100.0f;
    updateHFRolloffFilter();
}

void TapeProcessor::setHeadBump(float amount)
{
    headBump = std::clamp(amount, 0.0f, 100.0f);
    headBumpAmount = headBump / 100.0f;
    updateHeadBumpFilter();
}

void TapeProcessor::setBumpFreq(float freq)
{
    bumpFreq = std::clamp(freq, 40.0f, 150.0f);
    updateHeadBumpFilter();
}

void TapeProcessor::setWow(float amount)
{
    wow = std::clamp(amount, 0.0f, 100.0f);
    wowDepth = (wow / 100.0f) * 3.0f;  // Max 3ms pitch deviation
    updateWowFlutterLFO();
}

void TapeProcessor::setFlutter(float amount)
{
    flutter = std::clamp(amount, 0.0f, 100.0f);
    flutterDepth = (flutter / 100.0f) * 0.5f;  // Max 0.5ms pitch deviation
    updateWowFlutterLFO();
}

void TapeProcessor::setHiss(float amount)
{
    hiss = std::clamp(amount, 0.0f, 100.0f);
    // Map to -60dB to -20dB noise floor
    float hissDb = DSPUtils::mapRange(hiss, 0.0f, 100.0f, -80.0f, -30.0f);
    hissLevel = DSPUtils::decibelsToLinear(hissDb);
}

void TapeProcessor::setOutput(float dB)
{
    outputGain = std::clamp(dB, -12.0f, 12.0f);
    outputGainLinear = DSPUtils::decibelsToLinear(outputGain);
}

void TapeProcessor::setMix(float amount)
{
    mix = std::clamp(amount, 0.0f, 100.0f);
    mixAmount = mix / 100.0f;
}

void TapeProcessor::setAge(float amount)
{
    age = std::clamp(amount, 0.0f, 100.0f);
    ageAmount = age / 100.0f;
    updateHFRolloffFilter();
    updateWowFlutterLFO();
}

void TapeProcessor::setBias(float amount)
{
    bias = std::clamp(amount, 0.0f, 100.0f);
    biasAmount = bias / 100.0f;
}

void TapeProcessor::setMachineType(int type)
{
    machineType = static_cast<MachineType>(std::clamp(type, 0, 2));
    updateHeadBumpFilter();
    updateHFRolloffFilter();
}

void TapeProcessor::setTapeType(int type)
{
    tapeType = static_cast<TapeType>(std::clamp(type, 0, 2));
    updateHeadBumpFilter();
    updateHFRolloffFilter();
}

void TapeProcessor::updateHeadBumpFilter()
{
    // Head bump frequency varies with tape speed
    float speedMultiplier = 1.0f;
    switch (machineType)
    {
        case MachineType::IPS_7_5:  speedMultiplier = 0.7f; break;   // Lower bump
        case MachineType::IPS_15:   speedMultiplier = 1.0f; break;   // Reference
        case MachineType::IPS_30:   speedMultiplier = 1.5f; break;   // Higher bump
    }

    float centerFreq = bumpFreq * speedMultiplier;
    centerFreq = std::clamp(centerFreq, 30.0f, 200.0f);

    // Gain amount varies with tape type
    float typeGain = 1.0f;
    switch (tapeType)
    {
        case TapeType::TypeI:   typeGain = 1.2f; break;   // More pronounced
        case TapeType::TypeII:  typeGain = 0.9f; break;   // Subtler
        case TapeType::Modern:  typeGain = 0.7f; break;   // Minimal
    }

    // Calculate peak filter coefficients (bell/parametric EQ)
    float gainDb = headBumpAmount * 6.0f * typeGain;  // Max +6dB boost
    float Q = 1.5f;  // Moderate Q for smooth bump

    float A = std::pow(10.0f, gainDb / 40.0f);
    float omega = 2.0f * juce::MathConstants<float>::pi * centerFreq / static_cast<float>(currentSampleRate);
    float sinOmega = std::sin(omega);
    float cosOmega = std::cos(omega);
    float alpha = sinOmega / (2.0f * Q);

    float b0 = 1.0f + alpha * A;
    float b1 = -2.0f * cosOmega;
    float b2 = 1.0f - alpha * A;
    float a0 = 1.0f + alpha / A;
    float a1 = -2.0f * cosOmega;
    float a2 = 1.0f - alpha / A;

    // Normalize coefficients
    headBumpB0 = b0 / a0;
    headBumpB1 = b1 / a0;
    headBumpB2 = b2 / a0;
    headBumpA1 = a1 / a0;
    headBumpA2 = a2 / a0;
}

void TapeProcessor::updateHFRolloffFilter()
{
    // HF cutoff varies with tape speed and type
    float baseCutoff = 15000.0f;

    switch (machineType)
    {
        case MachineType::IPS_7_5:  baseCutoff = 10000.0f; break;  // Darker
        case MachineType::IPS_15:   baseCutoff = 15000.0f; break;  // Reference
        case MachineType::IPS_30:   baseCutoff = 18000.0f; break;  // Brighter
    }

    switch (tapeType)
    {
        case TapeType::TypeI:   baseCutoff *= 0.85f; break;  // Warmer
        case TapeType::TypeII:  baseCutoff *= 1.0f;  break;  // Reference
        case TapeType::Modern:  baseCutoff *= 1.1f;  break;  // Extended
    }

    // Warmth and age reduce HF
    float warmthCut = 1.0f - (warmthAmount * 0.4f);  // Up to 40% reduction
    float ageCut = 1.0f - (ageAmount * 0.3f);        // Up to 30% reduction

    float finalCutoff = baseCutoff * warmthCut * ageCut;
    finalCutoff = std::clamp(finalCutoff, 2000.0f, 20000.0f);

    // One-pole lowpass coefficient
    float omega = 2.0f * juce::MathConstants<float>::pi * finalCutoff / static_cast<float>(currentSampleRate);
    hfRolloffCoeffL = omega / (1.0f + omega);
    hfRolloffCoeffR = hfRolloffCoeffL;
}

void TapeProcessor::updateWowFlutterLFO()
{
    // Wow rate: 0.5-3 Hz (slow pitch variation)
    // Age increases wow
    float ageWowBoost = 1.0f + ageAmount * 0.5f;
    wowRate = 0.5f + randomDist(rng) * 0.5f;  // Slight randomness
    wowPhaseIncrement = wowRate / static_cast<float>(currentSampleRate);

    // Flutter rate: 5-30 Hz (fast pitch variation)
    float ageFlutterBoost = 1.0f + ageAmount * 0.3f;
    flutterRate = 10.0f + randomDist(rng) * 5.0f;  // Slight randomness
    flutterPhaseIncrement = flutterRate / static_cast<float>(currentSampleRate);

    // Random offsets for natural feel
    wowRandomOffset = randomDist(rng) * 0.2f;
    flutterRandomOffset = randomDist(rng) * 0.1f;
}

void TapeProcessor::writeToDelayLine(float sample, int channel)
{
    if (channel == 0)
        delayLineL[writeIndex] = sample;
    else
        delayLineR[writeIndex] = sample;
}

float TapeProcessor::readFromDelayLine(float delaySamples, int channel)
{
    const auto& delayLine = (channel == 0) ? delayLineL : delayLineR;
    int size = static_cast<int>(delayLine.size());

    // Fractional delay with linear interpolation
    float readPos = static_cast<float>(writeIndex) - delaySamples;
    while (readPos < 0.0f)
        readPos += static_cast<float>(size);

    int index0 = static_cast<int>(readPos) % size;
    int index1 = (index0 + 1) % size;
    float frac = readPos - std::floor(readPos);

    return delayLine[index0] * (1.0f - frac) + delayLine[index1] * frac;
}

float TapeProcessor::processSaturation(float input, int channel)
{
    float& hysteresisState = (channel == 0) ? hysteresisStateL : hysteresisStateR;

    // Bias affects the saturation curve
    float biasOffset = (biasAmount - 0.5f) * 0.1f;  // -0.05 to +0.05
    float biasedInput = input + biasOffset;

    // Different saturation characteristics per tape type
    float drive = 1.0f + saturationAmount * 4.0f;  // 1 to 5x drive

    switch (tapeType)
    {
        case TapeType::TypeI:
        {
            // Ferric: warmer, more saturation, even harmonics
            drive *= 1.3f;
            float saturated = DSPUtils::hysteresis(biasedInput * drive, hysteresisState, saturationAmount);
            return saturated * 0.8f;  // Compensate for drive
        }

        case TapeType::TypeII:
        {
            // Chrome: cleaner, less distortion
            drive *= 0.9f;
            float saturated = std::tanh(biasedInput * drive);
            // Update hysteresis state for continuity
            hysteresisState = hysteresisState * 0.9f + saturated * 0.1f;
            return saturated;
        }

        case TapeType::Modern:
        {
            // Modern: cleanest, most headroom
            drive *= 0.7f;
            float saturated = biasedInput * drive;
            // Very gentle soft clipping
            if (std::abs(saturated) > 0.7f)
            {
                float sign = (saturated > 0.0f) ? 1.0f : -1.0f;
                saturated = sign * (0.7f + std::tanh((std::abs(saturated) - 0.7f) * 2.0f) * 0.3f);
            }
            hysteresisState = hysteresisState * 0.95f + saturated * 0.05f;
            return saturated;
        }

        default:
            return std::tanh(biasedInput * drive);
    }
}

float TapeProcessor::processHeadBump(float input, int channel)
{
    BiquadState& state = (channel == 0) ? headBumpStateL : headBumpStateR;

    // Biquad filter processing
    float output = headBumpB0 * input + headBumpB1 * state.x1 + headBumpB2 * state.x2
                 - headBumpA1 * state.y1 - headBumpA2 * state.y2;

    // Update state
    state.x2 = state.x1;
    state.x1 = input;
    state.y2 = state.y1;
    state.y1 = output;

    return output;
}

float TapeProcessor::processHFRolloff(float input, int channel)
{
    if (channel == 0)
    {
        hfRolloffStateL += hfRolloffCoeffL * (input - hfRolloffStateL);
        return hfRolloffStateL;
    }
    else
    {
        hfRolloffStateR += hfRolloffCoeffR * (input - hfRolloffStateR);
        return hfRolloffStateR;
    }
}

float TapeProcessor::processWowFlutter(float input, int channel)
{
    // Write input to delay line
    writeToDelayLine(input, channel);

    // Calculate wow modulation (slow sine with randomness)
    float wowMod = std::sin(wowPhase * 2.0f * juce::MathConstants<float>::pi);
    wowMod += wowRandomOffset * std::sin(wowPhase * 1.7f * juce::MathConstants<float>::pi);  // Irregular
    wowMod *= wowDepth;

    // Calculate flutter modulation (fast with randomness)
    float flutterMod = std::sin(flutterPhase * 2.0f * juce::MathConstants<float>::pi);
    flutterMod += flutterRandomOffset * std::sin(flutterPhase * 2.3f * juce::MathConstants<float>::pi);
    flutterMod *= flutterDepth;

    // Age increases the effect
    float ageBoost = 1.0f + ageAmount * 0.5f;
    float totalModulation = (wowMod + flutterMod) * ageBoost;

    // Convert modulation to delay time (base delay + modulation)
    float delaySamples = (baseDelayMs + totalModulation) * static_cast<float>(currentSampleRate) / 1000.0f;
    delaySamples = std::clamp(delaySamples, 1.0f, static_cast<float>(delayLineL.size() - 2));

    return readFromDelayLine(delaySamples, channel);
}

float TapeProcessor::processHiss()
{
    // Generate noise
    float noise = noiseGen.nextSample();

    // Shape the noise spectrum (tape hiss has specific character)
    // Filter to emphasize mid-high frequencies
    float shaped = noise * 0.7f;

    return shaped * hissLevel;
}

void TapeProcessor::process(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    if (numChannels == 0 || numSamples == 0)
        return;

    // Measure input level
    float inLevel = 0.0f;
    for (int ch = 0; ch < numChannels; ++ch)
        inLevel = std::max(inLevel, buffer.getMagnitude(ch, 0, numSamples));
    inputLevel.store(inLevel);

    // Process each sample
    for (int i = 0; i < numSamples; ++i)
    {
        // Advance LFOs once per sample
        wowPhase += wowPhaseIncrement;
        if (wowPhase >= 1.0f) wowPhase -= 1.0f;

        flutterPhase += flutterPhaseIncrement;
        if (flutterPhase >= 1.0f) flutterPhase -= 1.0f;

        // Occasionally update random offsets for natural variation
        if (i % 1000 == 0)
        {
            wowRandomOffset = wowRandomOffset * 0.99f + randomDist(rng) * 0.01f;
            flutterRandomOffset = flutterRandomOffset * 0.99f + randomDist(rng) * 0.01f;
        }

        // Generate hiss (same for both channels, slight decorrelation applied later)
        float hissL = processHiss();
        float hissR = hissL * 0.9f + processHiss() * 0.1f;  // Slight stereo difference

        for (int ch = 0; ch < numChannels; ++ch)
        {
            float* channelData = buffer.getWritePointer(ch);
            float dry = channelData[i];

            // Apply input drive
            float sample = dry * inputGainLinear;

            // Signal chain: Saturation -> Head Bump -> HF Rolloff -> Wow/Flutter -> Hiss

            // 1. Tape saturation (with hysteresis)
            if (saturationAmount > 0.0f)
                sample = processSaturation(sample, ch);

            // 2. Head bump (low frequency boost)
            if (headBumpAmount > 0.0f)
                sample = processHeadBump(sample, ch);

            // 3. HF rolloff
            sample = processHFRolloff(sample, ch);

            // 4. Wow & Flutter (pitch modulation)
            if (wowDepth > 0.0f || flutterDepth > 0.0f)
                sample = processWowFlutter(sample, ch);

            // 5. Add tape hiss
            if (hissLevel > 0.0f)
                sample += (ch == 0) ? hissL : hissR;

            // Apply output gain
            sample *= outputGainLinear;

            // Mix dry/wet
            channelData[i] = dry * (1.0f - mixAmount) + sample * mixAmount;
        }

        // Advance delay line write index
        writeIndex = (writeIndex + 1) % static_cast<int>(delayLineL.size());
    }

    // Measure output level
    float outLevel = 0.0f;
    for (int ch = 0; ch < numChannels; ++ch)
        outLevel = std::max(outLevel, buffer.getMagnitude(ch, 0, numSamples));
    outputLevel.store(outLevel);
}
