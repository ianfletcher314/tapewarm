#include "PluginProcessor.h"
#include "PluginEditor.h"

// Muted metallic color palette (matching PDLBRD style)
namespace TapeColors {
    const juce::Colour background   = juce::Colour(0xff1a1a1a);  // Dark background
    const juce::Colour faceplate    = juce::Colour(0xff3a3535);  // Muted brownish metal
    const juce::Colour cream        = juce::Colour(0xffdddddd);  // Light gray text
    const juce::Colour gold         = juce::Colour(0xffc9a227);  // Gold accents
    const juce::Colour reelHub      = juce::Colour(0xff6a6055);  // Reel hub brown
    const juce::Colour reelTape     = juce::Colour(0xff1a1510);  // Tape color (dark)
    const juce::Colour vuGreen      = juce::Colour(0xff22c55e);  // VU meter green (matching PDLBRD)
    const juce::Colour vuYellow     = juce::Colour(0xffeab308);  // VU meter yellow
    const juce::Colour vuRed        = juce::Colour(0xffef4444);  // VU meter red
    const juce::Colour knobBody     = juce::Colour(0xff4a4440);  // Knob body
    const juce::Colour knobPointer  = juce::Colour(0xffeeeeee);  // Knob pointer
    const juce::Colour labelText    = juce::Colour(0xffeeeeee);  // Label text
    const juce::Colour metalLight   = juce::Colour(0xff505050);  // Metal highlight
    const juce::Colour metalDark    = juce::Colour(0xff202020);  // Metal shadow
    const juce::Colour screw        = juce::Colour(0xff505050);  // Screw color
}

//==============================================================================
// Helper functions for drawing components (matching PDLBRD style)
//==============================================================================
static void drawScrew(juce::Graphics& g, float x, float y, float size)
{
    // Outer ring
    g.setColour(TapeColors::screw.darker(0.3f));
    g.fillEllipse(x - size/2, y - size/2, size, size);

    // Inner hex pattern (simplified as star)
    g.setColour(TapeColors::screw.brighter(0.2f));
    g.fillEllipse(x - size/3, y - size/3, size * 0.66f, size * 0.66f);

    // Hex slot
    juce::Path hex;
    float r = size * 0.22f;
    for (int i = 0; i < 6; ++i)
    {
        float angle = i * juce::MathConstants<float>::pi / 3.0f - juce::MathConstants<float>::pi / 6.0f;
        float px = x + r * std::cos(angle);
        float py = y + r * std::sin(angle);
        if (i == 0)
            hex.startNewSubPath(px, py);
        else
            hex.lineTo(px, py);
    }
    hex.closeSubPath();
    g.setColour(TapeColors::screw.darker(0.5f));
    g.fillPath(hex);
}

static void drawBrushedMetalTexture(juce::Graphics& g, juce::Rectangle<float> bounds, juce::Colour baseColour)
{
    // Base color
    g.setColour(baseColour);
    g.fillRoundedRectangle(bounds, 12.0f);

    // Subtle diagonal brush strokes
    g.setColour(juce::Colours::white.withAlpha(0.03f));
    for (float i = -bounds.getHeight(); i < bounds.getWidth() + bounds.getHeight(); i += 3.0f)
    {
        g.drawLine(bounds.getX() + i, bounds.getY(),
                   bounds.getX() + i + bounds.getHeight(), bounds.getBottom(), 0.5f);
    }

    // Top highlight
    juce::ColourGradient topHighlight(baseColour.brighter(0.15f), bounds.getX(), bounds.getY(),
                                       baseColour, bounds.getX(), bounds.getY() + 30.0f, false);
    g.setGradientFill(topHighlight);
    g.fillRoundedRectangle(bounds.getX(), bounds.getY(), bounds.getWidth(), 30.0f, 12.0f);
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

    // Pointer line (7 o'clock to 5 o'clock range, rotating clockwise)
    // 7 o'clock = 210 degrees = 7*pi/6 radians from positive x-axis
    // 5 o'clock = 330 degrees = 11*pi/6 radians, but we go through 360 degrees
    // So we map from 7*pi/6 (~3.67) to 11*pi/6 + 2*pi (~11.52) for clockwise rotation
    const float startAngle = juce::MathConstants<float>::pi * 0.75f;   // 135 degrees (7 o'clock in screen coords where y increases downward)
    const float endAngle = juce::MathConstants<float>::pi * 2.25f;     // 405 degrees (5 o'clock, going clockwise through bottom)
    float indicatorAngle = juce::jmap(sliderPosProportional, 0.0f, 1.0f, startAngle, endAngle);

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
    auto bounds = getLocalBounds().toFloat();

    // Background (recessed look)
    g.setColour(juce::Colour(0xff151515));
    g.fillRoundedRectangle(bounds, 4.0f);

    // Inner border for depth
    g.setColour(juce::Colour(0xff0a0a0a));
    g.fillRoundedRectangle(bounds.reduced(2), 3.0f);

    // Calculate level (convert to dB, then normalize)
    float db = juce::Decibels::gainToDecibels(level, -60.0f);
    float normalized = juce::jmap(db, -60.0f, 0.0f, 0.0f, 1.0f);
    normalized = juce::jlimit(0.0f, 1.0f, normalized);

    // Segmented LED meter style (like PDLBRD)
    const int numSegments = 16;
    float segmentWidth = (bounds.getWidth() - 8) / numSegments;
    float segmentHeight = bounds.getHeight() - 8;
    float segmentGap = 2.0f;

    int litSegments = (int)(normalized * numSegments);

    for (int i = 0; i < numSegments; ++i)
    {
        float segX = bounds.getX() + 4 + i * segmentWidth;

        // Determine segment color
        juce::Colour segColour;
        if (i < 10)
            segColour = TapeColors::vuGreen;
        else if (i < 13)
            segColour = TapeColors::vuYellow;
        else
            segColour = TapeColors::vuRed;

        bool isLit = (i < litSegments);

        if (isLit)
        {
            g.setColour(segColour);
        }
        else
        {
            g.setColour(segColour.withAlpha(0.15f));  // Dim unlit segments
        }

        g.fillRoundedRectangle(segX, bounds.getY() + 4, segmentWidth - segmentGap, segmentHeight, 2.0f);
    }

    // Peak indicator
    float peakDb = juce::Decibels::gainToDecibels(peakLevel, -60.0f);
    float peakNormalized = juce::jmap(peakDb, -60.0f, 0.0f, 0.0f, 1.0f);
    peakNormalized = juce::jlimit(0.0f, 1.0f, peakNormalized);

    if (peakNormalized > 0.01f)
    {
        int peakSegment = (int)(peakNormalized * numSegments);
        peakSegment = juce::jlimit(0, numSegments - 1, peakSegment);
        float peakX = bounds.getX() + 4 + peakSegment * segmentWidth;
        g.setColour(TapeColors::cream);
        g.fillRect(peakX + segmentWidth/2 - 1, bounds.getY() + 2, 2.0f, bounds.getHeight() - 4);
    }

    // Outer frame
    g.setColour(juce::Colour(0xff333333));
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
    auto bounds = getLocalBounds().toFloat().reduced(2);
    float cx = bounds.getCentreX();
    float cy = bounds.getCentreY();
    float outerRadius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;

    // Shadow under reel
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.fillEllipse(cx - outerRadius + 3, cy - outerRadius + 3, outerRadius * 2.0f, outerRadius * 2.0f);

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

    // Load background image
    juce::File imageFile("/Users/ianfletcher/tapewarm/Source/background.png");
    if (imageFile.existsAsFile())
        backgroundImage = juce::ImageFileFormat::loadFrom(imageFile);

    setSize(600, 540);
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
    // Use rotary knobs instead of linear sliders for larger, more usable controls
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 55, 14);
    slider.setColour(juce::Slider::textBoxTextColourId, TapeColors::cream);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setColour(juce::Label::textColourId, TapeColors::cream.withAlpha(0.85f));
    label.setFont(juce::FontOptions(10.0f).withStyle("Bold"));
    label.setJustificationType(juce::Justification::centred);
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

    // Faceplate area with brushed metal texture (matching PDLBRD style)
    auto faceplateArea = getLocalBounds().reduced(8).toFloat();

    // Draw background image if available, otherwise fall back to brushed metal
    if (backgroundImage.isValid())
    {
        // Draw image scaled to fit, with dark overlay for readability
        g.saveState();

        // Clip to rounded rectangle
        juce::Path clipPath;
        clipPath.addRoundedRectangle(faceplateArea, 12.0f);
        g.reduceClipRegion(clipPath);

        // Calculate source rectangle for proper cropping
        float imgW = (float)backgroundImage.getWidth();
        float imgH = (float)backgroundImage.getHeight();
        float destAspect = faceplateArea.getWidth() / faceplateArea.getHeight();
        float srcAspect = imgW / imgH;

        juce::Rectangle<float> srcRect;
        if (srcAspect > destAspect)
        {
            // Image is wider - crop sides
            float srcW = imgH * destAspect;
            float srcX = (imgW - srcW) / 2.0f;
            srcRect = juce::Rectangle<float>(srcX, 0, srcW, imgH);
        }
        else
        {
            // Image is taller - crop top/bottom
            float srcH = imgW / destAspect;
            float srcY = (imgH - srcH) * 0.3f;  // Shift up slightly
            srcRect = juce::Rectangle<float>(0, srcY, imgW, srcH);
        }

        g.drawImage(backgroundImage,
                    faceplateArea.getX(), faceplateArea.getY(),
                    faceplateArea.getWidth(), faceplateArea.getHeight(),
                    (int)srcRect.getX(), (int)srcRect.getY(),
                    (int)srcRect.getWidth(), (int)srcRect.getHeight());

        // Dark overlay for contrast (semi-transparent) - use faceplate color for warmth
        g.setColour(TapeColors::faceplate.withAlpha(0.75f));
        g.fillRect(faceplateArea);

        g.restoreState();
    }
    else
    {
        // Fallback to brushed metal texture
        drawBrushedMetalTexture(g, faceplateArea, TapeColors::faceplate);
    }

    // Beveled edge effect
    g.setColour(TapeColors::faceplate.brighter(0.2f));
    g.drawRoundedRectangle(faceplateArea, 12.0f, 2.0f);
    g.setColour(TapeColors::faceplate.darker(0.3f));
    g.drawRoundedRectangle(faceplateArea.reduced(2), 10.0f, 1.0f);

    // Title with slight emboss effect
    g.setColour(juce::Colour(0xff111111));
    g.setFont(juce::FontOptions(26.0f).withStyle("Bold"));
    g.drawText("TAPEWARM", 1, 21, getWidth(), 30, juce::Justification::centred);
    g.setColour(juce::Colours::white);
    g.drawText("TAPEWARM", 0, 20, getWidth(), 30, juce::Justification::centred);

    // Subtitle
    g.setColour(TapeColors::cream.withAlpha(0.6f));
    g.setFont(juce::FontOptions(10.0f));
    g.drawText("ANALOG TAPE EMULATION", 0, 46, getWidth(), 14, juce::Justification::centred);

    // VU meter labels (above the meters)
    g.setColour(TapeColors::cream.withAlpha(0.9f));
    g.setFont(juce::FontOptions(10.0f).withStyle("Bold"));
    g.drawText("INPUT", 170, 63, 80, 14, juce::Justification::centred);
    g.drawText("OUTPUT", 350, 63, 80, 14, juce::Justification::centred);

    // Corner screws (matching PDLBRD style)
    float screwSize = 12.0f;
    drawScrew(g, 22, 22, screwSize);
    drawScrew(g, getWidth() - 22.0f, 22, screwSize);
    drawScrew(g, 22, getHeight() - 22.0f, screwSize);
    drawScrew(g, getWidth() - 22.0f, getHeight() - 22.0f, screwSize);

    // Divider line above secondary controls
    g.setColour(TapeColors::cream.withAlpha(0.15f));
    g.drawHorizontalLine(405, 25, getWidth() - 25.0f);
}

void TapeWarmAudioProcessorEditor::resized()
{
    // Tape reels - top section (slightly smaller to make room for meters)
    int reelSize = 70;
    int reelY = 65;
    leftReel.setBounds(35, reelY, reelSize, reelSize);
    rightReel.setBounds(getWidth() - 35 - reelSize, reelY, reelSize, reelSize);

    // VU Meters between reels - SIGNIFICANTLY LARGER
    int meterWidth = 160;
    int meterHeight = 32;
    inputMeter.setBounds(130, 78, meterWidth, meterHeight);
    outputMeter.setBounds(310, 78, meterWidth, meterHeight);

    // Type selectors
    int selectorWidth = 130;
    machineLabel.setBounds(145, 118, selectorWidth, 14);
    machineTypeBox.setBounds(145, 133, selectorWidth, 24);

    tapeLabel.setBounds(310, 118, selectorWidth, 14);
    tapeTypeBox.setBounds(310, 133, selectorWidth, 24);

    // Main knobs - Row 1 (larger knobs)
    int knobSize = 75;
    int knobY1 = 168;
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

    // Secondary controls - bottom row as LARGER rotary knobs (4 across)
    int secKnobSize = 65;
    int secKnobY = 415;
    int secKnobSpacing = 140;
    int secStartX = 35;

    mixLabel.setBounds(secStartX, secKnobY, secKnobSize, 14);
    mixSlider.setBounds(secStartX, secKnobY + 14, secKnobSize, secKnobSize);

    biasLabel.setBounds(secStartX + secKnobSpacing, secKnobY, secKnobSize, 14);
    biasSlider.setBounds(secStartX + secKnobSpacing, secKnobY + 14, secKnobSize, secKnobSize);

    ageLabel.setBounds(secStartX + secKnobSpacing * 2, secKnobY, secKnobSize, 14);
    ageSlider.setBounds(secStartX + secKnobSpacing * 2, secKnobY + 14, secKnobSize, secKnobSize);

    bumpFreqLabel.setBounds(secStartX + secKnobSpacing * 3, secKnobY, secKnobSize, 14);
    bumpFreqSlider.setBounds(secStartX + secKnobSpacing * 3, secKnobY + 14, secKnobSize, secKnobSize);
}
