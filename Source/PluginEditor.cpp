#include "PluginProcessor.h"
#include "PluginEditor.h"

// Vintage tape machine color palette
namespace TapeColors {
    const juce::Colour background   = juce::Colour(0xff2a2520);  // Dark warm brown
    const juce::Colour faceplate    = juce::Colour(0xff3d3630);  // Lighter brown
    const juce::Colour cream        = juce::Colour(0xfff5f0e6);  // Vintage cream
    const juce::Colour gold         = juce::Colour(0xffc9a227);  // Gold accents
    const juce::Colour reelHub      = juce::Colour(0xff8b7355);  // Reel hub brown
    const juce::Colour reelTape     = juce::Colour(0xff1a1510);  // Tape color (dark)
    const juce::Colour vuGreen      = juce::Colour(0xff4a7c59);  // VU meter green
    const juce::Colour vuYellow     = juce::Colour(0xffc9a227);  // VU meter yellow
    const juce::Colour vuRed        = juce::Colour(0xffc44536);  // VU meter red
    const juce::Colour knobBody     = juce::Colour(0xff4a4440);  // Knob body
    const juce::Colour knobPointer  = juce::Colour(0xfff5f0e6);  // Knob pointer
    const juce::Colour labelText    = juce::Colour(0xfff5f0e6);  // Label text
    const juce::Colour metalLight   = juce::Colour(0xff606060);  // Metal highlight
    const juce::Colour metalDark    = juce::Colour(0xff252525);  // Metal shadow
}

//==============================================================================
// TapeWarmLookAndFeel implementation
//==============================================================================
void TapeWarmLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                            float sliderPosProportional, float, float,
                                            juce::Slider&)
{
    auto bounds = juce::Rectangle<float>((float)x, (float)y, (float)width, (float)height).reduced(4.0f);
    float cx = bounds.getCentreX();
    float cy = bounds.getCentreY();
    float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f - 2.0f;

    // Outer shadow
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.fillEllipse(cx - radius + 2, cy - radius + 2, radius * 2.0f, radius * 2.0f);

    // Knob body gradient (3D effect)
    juce::ColourGradient bodyGrad(TapeColors::knobBody.brighter(0.2f), cx - radius * 0.5f, cy - radius * 0.5f,
                                   TapeColors::knobBody.darker(0.3f), cx + radius * 0.5f, cy + radius * 0.5f, true);
    g.setGradientFill(bodyGrad);
    g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

    // Knurled edge pattern
    g.setColour(TapeColors::knobBody.darker(0.2f));
    int numKnurls = 32;
    for (int i = 0; i < numKnurls; ++i)
    {
        float angle = i * juce::MathConstants<float>::twoPi / numKnurls;
        float x1 = cx + (radius - 1.0f) * std::cos(angle);
        float y1 = cy + (radius - 1.0f) * std::sin(angle);
        float x2 = cx + (radius - 4.0f) * std::cos(angle);
        float y2 = cy + (radius - 4.0f) * std::sin(angle);
        g.drawLine(x1, y1, x2, y2, 1.0f);
    }

    // Inner cap with shine
    float capRadius = radius * 0.55f;
    juce::ColourGradient capGrad(TapeColors::knobBody.brighter(0.15f), cx - capRadius * 0.3f, cy - capRadius * 0.3f,
                                  TapeColors::knobBody.darker(0.1f), cx + capRadius * 0.3f, cy + capRadius * 0.3f, true);
    g.setGradientFill(capGrad);
    g.fillEllipse(cx - capRadius, cy - capRadius, capRadius * 2.0f, capRadius * 2.0f);

    // Pointer line (7 o'clock to 5 o'clock range)
    float indicatorAngle = juce::jmap(sliderPosProportional, 0.0f, 1.0f,
                                       juce::MathConstants<float>::pi * 1.25f,
                                       juce::MathConstants<float>::pi * 2.75f);

    float pointerLength = radius * 0.75f;
    float ix1 = cx + (radius * 0.15f) * std::cos(indicatorAngle);
    float iy1 = cy + (radius * 0.15f) * std::sin(indicatorAngle);
    float ix2 = cx + pointerLength * std::cos(indicatorAngle);
    float iy2 = cy + pointerLength * std::sin(indicatorAngle);

    g.setColour(TapeColors::knobPointer);
    g.drawLine(ix1, iy1, ix2, iy2, 2.5f);

    // Center dot
    g.setColour(TapeColors::gold);
    g.fillEllipse(cx - 3, cy - 3, 6, 6);
}

//==============================================================================
// VUMeter implementation
//==============================================================================
void VUMeter::setLevel(float newLevel)
{
    level = newLevel;

    // Peak hold
    if (newLevel > peakLevel)
    {
        peakLevel = newLevel;
        peakHoldCounter = 30;  // Hold for ~1 second at 30fps
    }
    else if (peakHoldCounter > 0)
    {
        peakHoldCounter--;
    }
    else
    {
        peakLevel *= 0.95f;  // Decay peak
    }

    repaint();
}

void VUMeter::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2);

    // Background with vintage look
    g.setColour(TapeColors::cream.darker(0.8f));
    g.fillRoundedRectangle(bounds, 4.0f);

    // Inner recess
    auto innerBounds = bounds.reduced(3);
    g.setColour(juce::Colour(0xff0a0a08));
    g.fillRoundedRectangle(innerBounds, 3.0f);

    // Calculate level (convert to dB, then normalize)
    float db = juce::Decibels::gainToDecibels(level, -60.0f);
    float normalized = juce::jmap(db, -40.0f, 3.0f, 0.0f, 1.0f);
    normalized = juce::jlimit(0.0f, 1.0f, normalized);

    // Level bar with gradient
    float barWidth = innerBounds.getWidth() * normalized;
    auto barBounds = innerBounds.removeFromLeft(barWidth).reduced(2);

    if (barBounds.getWidth() > 0)
    {
        // Green to yellow to red gradient
        juce::Colour barColour;
        if (normalized < 0.6f)
            barColour = TapeColors::vuGreen;
        else if (normalized < 0.85f)
            barColour = TapeColors::vuYellow;
        else
            barColour = TapeColors::vuRed;

        g.setColour(barColour);
        g.fillRoundedRectangle(barBounds, 2.0f);
    }

    // Peak indicator
    float peakDb = juce::Decibels::gainToDecibels(peakLevel, -60.0f);
    float peakNormalized = juce::jmap(peakDb, -40.0f, 3.0f, 0.0f, 1.0f);
    peakNormalized = juce::jlimit(0.0f, 1.0f, peakNormalized);

    if (peakNormalized > 0.01f)
    {
        float peakX = bounds.getX() + 5 + (bounds.getWidth() - 10) * peakNormalized;
        g.setColour(TapeColors::cream);
        g.fillRect(peakX - 1, bounds.getY() + 4, 2.0f, bounds.getHeight() - 8);
    }

    // VU scale markings
    g.setColour(TapeColors::cream.withAlpha(0.5f));
    g.setFont(juce::FontOptions(8.0f));

    // -20, -10, -3, 0, +3 dB marks
    float marks[] = { -20.0f, -10.0f, -3.0f, 0.0f, 3.0f };
    for (float mark : marks)
    {
        float markNorm = juce::jmap(mark, -40.0f, 3.0f, 0.0f, 1.0f);
        float markX = bounds.getX() + 5 + (bounds.getWidth() - 10) * markNorm;
        g.drawLine(markX, bounds.getY() + 2, markX, bounds.getY() + 6, 1.0f);
    }

    // Outer frame
    g.setColour(TapeColors::gold.withAlpha(0.5f));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
}

//==============================================================================
// TapeReel implementation
//==============================================================================
TapeReel::TapeReel()
{
    startTimerHz(30);
}

void TapeReel::setSpinning(bool shouldSpin)
{
    spinning = shouldSpin;
}

void TapeReel::timerCallback()
{
    if (spinning)
    {
        rotation += rotationSpeed;
        if (rotation > juce::MathConstants<float>::twoPi)
            rotation -= juce::MathConstants<float>::twoPi;
        repaint();
    }
}

void TapeReel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(4);
    float cx = bounds.getCentreX();
    float cy = bounds.getCentreY();
    float outerRadius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;

    // Tape (outer ring) - varies with imaginary tape amount
    float tapeRadius = outerRadius;
    g.setColour(TapeColors::reelTape);
    g.fillEllipse(cx - tapeRadius, cy - tapeRadius, tapeRadius * 2.0f, tapeRadius * 2.0f);

    // Reel hub
    float hubRadius = outerRadius * 0.45f;

    // Hub base with metallic gradient
    juce::ColourGradient hubGrad(TapeColors::reelHub.brighter(0.3f), cx - hubRadius * 0.3f, cy - hubRadius * 0.3f,
                                  TapeColors::reelHub.darker(0.2f), cx + hubRadius * 0.3f, cy + hubRadius * 0.3f, true);
    g.setGradientFill(hubGrad);
    g.fillEllipse(cx - hubRadius, cy - hubRadius, hubRadius * 2.0f, hubRadius * 2.0f);

    // Hub spokes (3 spokes, rotating)
    g.setColour(TapeColors::reelHub.darker(0.4f));
    for (int i = 0; i < 3; ++i)
    {
        float angle = rotation + i * juce::MathConstants<float>::twoPi / 3.0f;

        // Spoke shape (trapezoid-ish)
        juce::Path spoke;
        float innerR = hubRadius * 0.25f;
        float outerR = hubRadius * 0.9f;
        float width1 = hubRadius * 0.15f;
        float width2 = hubRadius * 0.25f;

        float cos1 = std::cos(angle);
        float sin1 = std::sin(angle);

        // Inner points
        spoke.startNewSubPath(cx + innerR * cos1 - width1 * sin1, cy + innerR * sin1 + width1 * cos1);
        spoke.lineTo(cx + innerR * cos1 + width1 * sin1, cy + innerR * sin1 - width1 * cos1);
        // Outer points
        spoke.lineTo(cx + outerR * cos1 + width2 * sin1, cy + outerR * sin1 - width2 * cos1);
        spoke.lineTo(cx + outerR * cos1 - width2 * sin1, cy + outerR * sin1 + width2 * cos1);
        spoke.closeSubPath();

        g.fillPath(spoke);
    }

    // Center hole
    float holeRadius = hubRadius * 0.2f;
    g.setColour(TapeColors::background);
    g.fillEllipse(cx - holeRadius, cy - holeRadius, holeRadius * 2.0f, holeRadius * 2.0f);

    // Highlight reflection
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    juce::Path highlight;
    highlight.addArc(cx - hubRadius * 0.7f, cy - hubRadius * 0.7f,
                     hubRadius * 1.4f, hubRadius * 1.4f,
                     -0.5f, 0.8f, true);
    g.strokePath(highlight, juce::PathStrokeType(2.0f));
}

//==============================================================================
// TapeWarmAudioProcessorEditor implementation
//==============================================================================
TapeWarmAudioProcessorEditor::TapeWarmAudioProcessorEditor(TapeWarmAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set the custom look and feel
    setLookAndFeel(&lookAndFeel);

    // Add tape reels
    addAndMakeVisible(leftReel);
    addAndMakeVisible(rightReel);

    // Add VU meters
    addAndMakeVisible(inputMeter);
    addAndMakeVisible(outputMeter);

    // Machine type selector
    machineTypeBox.addItem("7.5 IPS", 1);
    machineTypeBox.addItem("15 IPS", 2);
    machineTypeBox.addItem("30 IPS", 3);
    machineTypeBox.setColour(juce::ComboBox::backgroundColourId, TapeColors::faceplate);
    machineTypeBox.setColour(juce::ComboBox::textColourId, TapeColors::cream);
    machineTypeBox.setColour(juce::ComboBox::outlineColourId, TapeColors::gold.withAlpha(0.5f));
    addAndMakeVisible(machineTypeBox);

    machineLabel.setText("MACHINE", juce::dontSendNotification);
    machineLabel.setColour(juce::Label::textColourId, TapeColors::cream);
    machineLabel.setFont(juce::FontOptions(10.0f).withStyle("Bold"));
    machineLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(machineLabel);

    // Tape type selector
    tapeTypeBox.addItem("Type I (Ferric)", 1);
    tapeTypeBox.addItem("Type II (Chrome)", 2);
    tapeTypeBox.addItem("Modern", 3);
    tapeTypeBox.setColour(juce::ComboBox::backgroundColourId, TapeColors::faceplate);
    tapeTypeBox.setColour(juce::ComboBox::textColourId, TapeColors::cream);
    tapeTypeBox.setColour(juce::ComboBox::outlineColourId, TapeColors::gold.withAlpha(0.5f));
    addAndMakeVisible(tapeTypeBox);

    tapeLabel.setText("TAPE", juce::dontSendNotification);
    tapeLabel.setColour(juce::Label::textColourId, TapeColors::cream);
    tapeLabel.setFont(juce::FontOptions(10.0f).withStyle("Bold"));
    tapeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(tapeLabel);

    // Setup main knobs - Row 1
    setupKnob(inputDriveKnob, inputDriveLabel, "INPUT");
    setupKnob(saturationKnob, saturationLabel, "SATURATION");
    setupKnob(warmthKnob, warmthLabel, "WARMTH");
    setupKnob(headBumpKnob, headBumpLabel, "HEAD BUMP");

    // Setup main knobs - Row 2
    setupKnob(wowKnob, wowLabel, "WOW");
    setupKnob(flutterKnob, flutterLabel, "FLUTTER");
    setupKnob(hissKnob, hissLabel, "HISS");
    setupKnob(outputKnob, outputLabel, "OUTPUT");

    // Setup secondary sliders
    setupSecondarySlider(bumpFreqSlider, bumpFreqLabel, "BUMP FREQ");
    setupSecondarySlider(ageSlider, ageLabel, "AGE");
    setupSecondarySlider(mixSlider, mixLabel, "MIX");
    setupSecondarySlider(biasSlider, biasLabel, "BIAS");

    // Create parameter attachments
    auto& apvts = audioProcessor.getAPVTS();

    inputDriveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "inputDrive", inputDriveKnob);
    saturationAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "saturation", saturationKnob);
    warmthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "warmth", warmthKnob);
    headBumpAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "headBump", headBumpKnob);
    bumpFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "bumpFreq", bumpFreqSlider);
    wowAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "wow", wowKnob);
    flutterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "flutter", flutterKnob);
    hissAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "hiss", hissKnob);
    outputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "output", outputKnob);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "mix", mixSlider);
    ageAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "age", ageSlider);
    biasAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "bias", biasSlider);
    machineTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "machineType", machineTypeBox);
    tapeTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "tapeType", tapeTypeBox);

    setSize(600, 500);
    startTimerHz(30);
}

TapeWarmAudioProcessorEditor::~TapeWarmAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void TapeWarmAudioProcessorEditor::setupKnob(juce::Slider& slider, juce::Label& label, const juce::String& text)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 14);
    slider.setColour(juce::Slider::textBoxTextColourId, TapeColors::cream);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setColour(juce::Label::textColourId, TapeColors::cream);
    label.setFont(juce::FontOptions(10.0f).withStyle("Bold"));
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}

void TapeWarmAudioProcessorEditor::setupSecondarySlider(juce::Slider& slider, juce::Label& label, const juce::String& text)
{
    slider.setSliderStyle(juce::Slider::LinearHorizontal);
    slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 16);
    slider.setColour(juce::Slider::textBoxTextColourId, TapeColors::cream);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    slider.setColour(juce::Slider::trackColourId, TapeColors::gold);
    slider.setColour(juce::Slider::thumbColourId, TapeColors::cream);
    slider.setColour(juce::Slider::backgroundColourId, TapeColors::faceplate.darker(0.3f));
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setColour(juce::Label::textColourId, TapeColors::cream.withAlpha(0.8f));
    label.setFont(juce::FontOptions(9.0f));
    label.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(label);
}

void TapeWarmAudioProcessorEditor::timerCallback()
{
    // Smooth the levels for display
    float targetIn = audioProcessor.getInputLevel();
    float targetOut = audioProcessor.getOutputLevel();

    smoothedInputLevel = smoothedInputLevel * 0.85f + targetIn * 0.15f;
    smoothedOutputLevel = smoothedOutputLevel * 0.85f + targetOut * 0.15f;

    inputMeter.setLevel(smoothedInputLevel);
    outputMeter.setLevel(smoothedOutputLevel);
}

void TapeWarmAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Main background
    g.fillAll(TapeColors::background);

    // Faceplate area
    auto faceplateArea = getLocalBounds().reduced(10);
    g.setColour(TapeColors::faceplate);
    g.fillRoundedRectangle(faceplateArea.toFloat(), 8.0f);

    // Brushed metal effect
    g.setColour(juce::Colours::white.withAlpha(0.02f));
    for (int i = 0; i < faceplateArea.getHeight(); i += 2)
    {
        g.drawHorizontalLine(faceplateArea.getY() + i, (float)faceplateArea.getX(), (float)faceplateArea.getRight());
    }

    // Bevel effect
    g.setColour(TapeColors::metalLight.withAlpha(0.3f));
    g.drawRoundedRectangle(faceplateArea.toFloat(), 8.0f, 1.0f);
    g.setColour(TapeColors::metalDark.withAlpha(0.5f));
    g.drawRoundedRectangle(faceplateArea.reduced(1).toFloat(), 7.0f, 1.0f);

    // Title
    g.setColour(TapeColors::gold);
    g.setFont(juce::FontOptions(28.0f).withStyle("Bold"));
    g.drawText("TAPEWARM", 0, 20, getWidth(), 35, juce::Justification::centred);

    // Subtitle
    g.setColour(TapeColors::cream.withAlpha(0.7f));
    g.setFont(juce::FontOptions(11.0f));
    g.drawText("ANALOG TAPE EMULATION", 0, 48, getWidth(), 16, juce::Justification::centred);

    // VU meter labels
    g.setColour(TapeColors::cream);
    g.setFont(juce::FontOptions(9.0f).withStyle("Bold"));
    g.drawText("IN", 175, 72, 30, 14, juce::Justification::centred);
    g.drawText("OUT", 395, 72, 30, 14, juce::Justification::centred);

    // Decorative screws in corners
    auto drawScrew = [&g](float x, float y) {
        g.setColour(TapeColors::metalDark);
        g.fillEllipse(x - 6, y - 6, 12, 12);
        g.setColour(TapeColors::metalLight);
        g.fillEllipse(x - 5, y - 5, 10, 10);
        g.setColour(TapeColors::metalDark);
        g.drawLine(x - 3, y, x + 3, y, 1.5f);
        g.drawLine(x, y - 3, x, y + 3, 1.5f);
    };

    drawScrew(25, 25);
    drawScrew(getWidth() - 25.0f, 25);
    drawScrew(25, getHeight() - 25.0f);
    drawScrew(getWidth() - 25.0f, getHeight() - 25.0f);

    // Divider line above secondary controls
    g.setColour(TapeColors::gold.withAlpha(0.3f));
    g.drawHorizontalLine(395, 30, getWidth() - 30.0f);
}

void TapeWarmAudioProcessorEditor::resized()
{
    // Tape reels - top section
    int reelSize = 80;
    int reelY = 70;
    leftReel.setBounds(50, reelY, reelSize, reelSize);
    rightReel.setBounds(getWidth() - 50 - reelSize, reelY, reelSize, reelSize);

    // VU Meters between reels
    int meterWidth = 150;
    int meterHeight = 22;
    inputMeter.setBounds(145, 86, meterWidth, meterHeight);
    outputMeter.setBounds(305, 86, meterWidth, meterHeight);

    // Type selectors
    int selectorWidth = 130;
    machineLabel.setBounds(145, 115, selectorWidth, 14);
    machineTypeBox.setBounds(145, 130, selectorWidth, 24);

    tapeLabel.setBounds(305, 115, selectorWidth, 14);
    tapeTypeBox.setBounds(305, 130, selectorWidth, 24);

    // Main knobs - Row 1
    int knobSize = 70;
    int knobY1 = 170;
    int knobSpacing = 140;
    int startX = 35;

    inputDriveLabel.setBounds(startX, knobY1, knobSize, 14);
    inputDriveKnob.setBounds(startX, knobY1 + 14, knobSize, knobSize);

    saturationLabel.setBounds(startX + knobSpacing, knobY1, knobSize, 14);
    saturationKnob.setBounds(startX + knobSpacing, knobY1 + 14, knobSize, knobSize);

    warmthLabel.setBounds(startX + knobSpacing * 2, knobY1, knobSize, 14);
    warmthKnob.setBounds(startX + knobSpacing * 2, knobY1 + 14, knobSize, knobSize);

    headBumpLabel.setBounds(startX + knobSpacing * 3, knobY1, knobSize, 14);
    headBumpKnob.setBounds(startX + knobSpacing * 3, knobY1 + 14, knobSize, knobSize);

    // Main knobs - Row 2
    int knobY2 = 280;

    wowLabel.setBounds(startX, knobY2, knobSize, 14);
    wowKnob.setBounds(startX, knobY2 + 14, knobSize, knobSize);

    flutterLabel.setBounds(startX + knobSpacing, knobY2, knobSize, 14);
    flutterKnob.setBounds(startX + knobSpacing, knobY2 + 14, knobSize, knobSize);

    hissLabel.setBounds(startX + knobSpacing * 2, knobY2, knobSize, 14);
    hissKnob.setBounds(startX + knobSpacing * 2, knobY2 + 14, knobSize, knobSize);

    outputLabel.setBounds(startX + knobSpacing * 3, knobY2, knobSize, 14);
    outputKnob.setBounds(startX + knobSpacing * 3, knobY2 + 14, knobSize, knobSize);

    // Secondary controls - bottom row
    int sliderY = 410;
    int sliderWidth = 100;
    int sliderHeight = 20;
    int labelWidth = 60;
    int sliderSpacing = 140;

    bumpFreqLabel.setBounds(30, sliderY, labelWidth, 16);
    bumpFreqSlider.setBounds(30 + labelWidth, sliderY, sliderWidth, sliderHeight);

    ageLabel.setBounds(30 + sliderSpacing, sliderY, labelWidth, 16);
    ageSlider.setBounds(30 + sliderSpacing + labelWidth, sliderY, sliderWidth, sliderHeight);

    mixLabel.setBounds(30 + sliderSpacing * 2, sliderY, labelWidth, 16);
    mixSlider.setBounds(30 + sliderSpacing * 2 + labelWidth, sliderY, sliderWidth, sliderHeight);

    biasLabel.setBounds(30 + sliderSpacing * 3, sliderY, labelWidth, 16);
    biasSlider.setBounds(30 + sliderSpacing * 3 + labelWidth, sliderY, sliderWidth, sliderHeight);
}
