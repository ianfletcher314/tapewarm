# TapeWarm - Analog Tape Emulation Plugin

A tape saturation and warmth plugin that adds the character, harmonics, and subtle imperfections of analog tape machines.

## Overview

TapeWarm captures the magic of analog tape - the warmth, the saturation, the subtle compression, and the musical imperfections that make recordings feel alive. More than just EQ, it models the complete tape recording chain.

## What Makes Tape Sound Like Tape

### Tape Saturation
- Soft clipping that adds harmonics (primarily even harmonics)
- Gentle compression of transients
- Increases perceived loudness without harshness

### Wow & Flutter
- Slow pitch variations (wow) from reel eccentricity
- Fast pitch variations (flutter) from capstan irregularities
- Adds subtle movement and life to static sounds

### Tape Compression
- Natural soft-knee compression from tape
- Transients are smoothed, sustain is enhanced
- "Glue" effect on mixes

### Head Bump
- Low frequency boost around 60-120Hz
- Caused by head geometry interacting with wavelength
- Adds weight and warmth to bass

### High Frequency Response
- Gentle rolloff above 12-15kHz
- Self-erasure at high frequencies
- Removes harshness, adds smoothness

### Tape Hiss
- Characteristic noise floor
- Can add vintage "vibe" in small amounts
- Variable noise level control

### Bias & Hysteresis
- Non-linear magnetic behavior
- Affects transient response and harmonics
- Contributes to "tape sound"

## Features

### Machine Type Selection
- **15 IPS (Inches Per Second)**: Brighter, more extended HF, tighter low end
- **7.5 IPS**: Warmer, more pronounced head bump, vintage vibe
- **30 IPS**: Cleanest, most extended frequency response (mastering)

### Tape Type Emulation
- **Type I (Ferric)**: Classic warm sound, more saturation
- **Type II (Chrome)**: Brighter, cleaner, less hiss
- **Modern Formulation**: Extended response, cleaner transients

### Controls

| Control | Range | Description |
|---------|-------|-------------|
| Input Drive | -12dB to +12dB | Drive into tape saturation |
| Saturation | 0-100% | Amount of tape compression/harmonics |
| Warmth | 0-100% | Low-mid emphasis and HF rolloff |
| Head Bump | 0-100% | Low frequency boost amount |
| Bump Freq | 40-150Hz | Head bump center frequency |
| Wow | 0-100% | Slow pitch modulation depth |
| Flutter | 0-100% | Fast pitch modulation depth |
| Hiss | 0-100% | Tape noise amount |
| Output | -12dB to +12dB | Output level compensation |

### Additional Features
- **Age**: Simulates worn tape/heads (increases wow, softens HF)
- **Bias**: Adjusts the bias point for different saturation character
- **Mix**: Parallel blend (dry/wet)
- **Stereo Width**: Tape's effect on stereo imaging

## Signal Flow

```
Input -> Input Gain -> Bias/Hysteresis Model -> Saturation
      -> Head Bump EQ -> HF Rolloff -> Wow/Flutter
      -> Compression Modeling -> Hiss -> Output Gain
```

## UI Design

```
+------------------------------------------------------------------+
|  TAPEWARM                          [Machine: 15 IPS v] [Type v]  |
+------------------------------------------------------------------+
|                                                                   |
|        _____________________                                      |
|       /                     \     VU METER                        |
|      |    O           O      |    [====|====]                     |
|      |   REEL       REEL     |                                    |
|       \_____________________/                                     |
|                                                                   |
|  +----------+  +----------+  +----------+  +----------+           |
|  |  INPUT   |  |SATURATION|  |  WARMTH  |  | HEAD BUMP|           |
|  |    ()    |  |    ()    |  |    ()    |  |    ()    |           |
|  +----------+  +----------+  +----------+  +----------+           |
|                                                                   |
|  +----------+  +----------+  +----------+  +----------+           |
|  |   WOW    |  | FLUTTER  |  |   HISS   |  |  OUTPUT  |           |
|  |    ()    |  |    ()    |  |    ()    |  |    ()    |           |
|  +----------+  +----------+  +----------+  +----------+           |
|                                                                   |
|  [BUMP FREQ: 80Hz]  [AGE: New]  [MIX: 100%]  [BYPASS]            |
+------------------------------------------------------------------+
```

## Implementation Plan

### Phase 1: Project Setup
- [ ] Create JUCE project
- [ ] Set up build targets (AU/VST3)
- [ ] Create parameter layout
- [ ] Basic plugin shell

### Phase 2: Core DSP - Saturation
- [ ] Implement tape saturation algorithm
  - [ ] Soft clipping waveshaper
  - [ ] Hysteresis modeling (optional: full Jiles-Atherton model)
  - [ ] Harmonic generation
  - [ ] Input-dependent compression
- [ ] Anti-aliasing (oversampling)

### Phase 3: Core DSP - Tape Characteristics
- [ ] Implement head bump
  - [ ] Low shelf/bell filter
  - [ ] Frequency-adjustable
- [ ] Implement HF rolloff
  - [ ] Gentle low-pass filter
  - [ ] Speed-dependent cutoff
- [ ] Implement warmth control
  - [ ] Combined low boost + HF cut

### Phase 4: Modulation & Noise
- [ ] Implement wow & flutter
  - [ ] Dual LFO system (slow + fast)
  - [ ] Subtle pitch/delay modulation
  - [ ] Randomization for realism
- [ ] Implement tape hiss
  - [ ] Filtered noise generator
  - [ ] Authentic tape noise spectrum

### Phase 5: UI Development
- [ ] Design vintage/analog aesthetic
- [ ] Create tape reel animation (optional)
- [ ] VU meter with analog ballistics
- [ ] Custom knobs (vintage style)
- [ ] Machine/tape type selectors

### Phase 6: Polish & Optimization
- [ ] CPU optimization
- [ ] Oversampling options (2x, 4x)
- [ ] Preset system
- [ ] A/B comparison
- [ ] Mix knob (parallel processing)

### Phase 7: Testing & Release
- [ ] Test on various material (drums, vocals, mix bus)
- [ ] Compare to reference tape machines
- [ ] Create factory presets
- [ ] Documentation

## Technical Details

### Saturation Algorithm Options
1. **Simple waveshaper**: `tanh(x)` or soft clipper
2. **Asymmetric saturation**: Different positive/negative response
3. **Hysteresis model**: More accurate but CPU intensive

### Wow & Flutter Implementation
- Wow: 0.5-3Hz sine/random LFO -> pitch shift
- Flutter: 5-30Hz sine/random LFO -> pitch shift
- Combined with slight delay modulation

### Head Bump
- Peak/shelf filter at 60-120Hz
- Boost amount depends on tape speed
- Q varies with frequency

## Dependencies

- JUCE Framework 7.x
- C++17 or later

## References

- "Magnetic Recording: The First 100 Years" - IEEE
- Jiles-Atherton hysteresis model papers
- Analysis of classic tape machines (Studer, Ampex, MCI)

## Building

```bash
# macOS
cd Builds/MacOSX
xcodebuild -project TapeWarm.xcodeproj -configuration Release
```

## License

MIT License
