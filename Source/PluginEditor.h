#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
// Custom LookAndFeel for vintage tape machine style knobs
//==============================================================================
class TapeWarmLookAndFeel : public juce::LookAndFeel_V4
{
public:
    TapeWarmLookAndFeel() {}

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float, float,
                          juce::Slider&) override;
};

//==============================================================================
// VU Meter component with analog-style ballistics
//==============================================================================
class VUMeter : public juce::Component
{
public:
    void setLevel(float newLevel);
    void paint(juce::Graphics& g) override;

private:
    float level = 0.0f;
    float peakLevel = 0.0f;
    int peakHoldCounter = 0;
};

//==============================================================================
// Tape reel animation component
//==============================================================================
class TapeReel : public juce::Component, public juce::Timer
{
public:
    TapeReel();
    ~TapeReel() override { stopTimer(); }

    void paint(juce::Graphics& g) override;
    void timerCallback() override;
    void setSpinning(bool shouldSpin);

private:
    float rotation = 0.0f;
    float rotationSpeed = 0.02f;
    bool spinning = true;
};

//==============================================================================
// Main editor class
//==============================================================================
class TapeWarmAudioProcessorEditor : public juce::AudioProcessorEditor,
                                      public juce::Timer
{
public:
    TapeWarmAudioProcessorEditor(TapeWarmAudioProcessor&);
    ~TapeWarmAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    TapeWarmAudioProcessor& audioProcessor;

    // Custom look and feel
    TapeWarmLookAndFeel lookAndFeel;

    // Background image
    juce::Image backgroundImage;

    // Tape reels
    TapeReel leftReel, rightReel;

    // VU Meters
    VUMeter inputMeter, outputMeter;
    float smoothedInputLevel = 0.0f;
    float smoothedOutputLevel = 0.0f;

    // Machine and tape type selectors
    juce::ComboBox machineTypeBox;
    juce::ComboBox tapeTypeBox;
    juce::Label machineLabel, tapeLabel;

    // Main knobs - Row 1
    juce::Slider inputDriveKnob;
    juce::Slider saturationKnob;
    juce::Slider warmthKnob;
    juce::Slider headBumpKnob;

    // Main knobs - Row 2
    juce::Slider wowKnob;
    juce::Slider flutterKnob;
    juce::Slider hissKnob;
    juce::Slider outputKnob;

    // Labels for row 1
    juce::Label inputDriveLabel;
    juce::Label saturationLabel;
    juce::Label warmthLabel;
    juce::Label headBumpLabel;

    // Labels for row 2
    juce::Label wowLabel;
    juce::Label flutterLabel;
    juce::Label hissLabel;
    juce::Label outputLabel;

    // Secondary controls
    juce::Slider bumpFreqSlider;
    juce::Slider ageSlider;
    juce::Slider mixSlider;
    juce::Slider biasSlider;

    juce::Label bumpFreqLabel;
    juce::Label ageLabel;
    juce::Label mixLabel;
    juce::Label biasLabel;

    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputDriveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> saturationAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> warmthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> headBumpAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bumpFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> wowAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> flutterAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hissAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ageAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> biasAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> machineTypeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> tapeTypeAttachment;

    void setupKnob(juce::Slider& slider, juce::Label& label, const juce::String& text);
    void setupSecondarySlider(juce::Slider& slider, juce::Label& label, const juce::String& text);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TapeWarmAudioProcessorEditor)
};
