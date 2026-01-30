#pragma once

#include <cmath>
#include <random>

namespace DSPUtils
{
    inline float linearToDecibels(float linear)
    {
        return linear > 0.0f ? 20.0f * std::log10(linear) : -100.0f;
    }

    inline float decibelsToLinear(float dB)
    {
        return std::pow(10.0f, dB / 20.0f);
    }

    inline float mapRange(float value, float inMin, float inMax, float outMin, float outMax)
    {
        return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
    }

    // Soft saturation using tanh
    inline float softClip(float sample)
    {
        return std::tanh(sample);
    }

    // Tape-style soft clipping with even harmonics
    inline float tapeSaturate(float sample, float drive)
    {
        // Asymmetric soft clipping for even harmonics
        float x = sample * drive;
        float y = std::tanh(x);

        // Add slight asymmetry for even harmonics
        float asymmetry = 0.1f * drive;
        if (x > 0.0f)
            y *= (1.0f + asymmetry * 0.5f);
        else
            y *= (1.0f - asymmetry * 0.3f);

        return y;
    }

    // Hysteresis approximation for tape saturation
    inline float hysteresis(float input, float& state, float saturation)
    {
        // Simplified hysteresis model
        float diff = input - state;
        float drive = 1.0f + saturation * 3.0f;

        // Apply soft saturation to the difference
        float saturatedDiff = std::tanh(diff * drive) / drive;

        // Update state with some lag (magnetic memory)
        float lagCoeff = 0.3f + saturation * 0.4f;
        state += saturatedDiff * lagCoeff;

        return state;
    }

    inline float hardClip(float sample, float threshold = 1.0f)
    {
        return std::clamp(sample, -threshold, threshold);
    }

    // Calculate one-pole filter coefficient for given time constant
    inline float calculateCoefficient(double sampleRate, float timeMs)
    {
        if (timeMs <= 0.0f) return 1.0f;
        return 1.0f - std::exp(-1.0f / (static_cast<float>(sampleRate) * timeMs * 0.001f));
    }

    // One-pole lowpass filter
    inline float onePoleLP(float input, float& state, float coeff)
    {
        state += coeff * (input - state);
        return state;
    }

    // Simple white noise generator
    class NoiseGenerator
    {
    public:
        NoiseGenerator() : gen(std::random_device{}()), dist(-1.0f, 1.0f) {}

        float nextSample()
        {
            return dist(gen);
        }

    private:
        std::mt19937 gen;
        std::uniform_real_distribution<float> dist;
    };
}
