# TapeWarm - Usage Guide

**Analog Tape Emulation Plugin**

TapeWarm brings the warmth, saturation, and character of classic analog tape machines to your mixes. With multiple machine types, tape formulations, and comprehensive modulation controls, it can add subtle warmth or aggressive lo-fi character to any source.

---

## Use Cases in Modern Rock Production

### Drum Bus Processing

Tape saturation on drums adds weight, punch, and that "finished" sound that's hard to achieve with EQ alone.

**Modern Rock Drum Bus:**
- Machine: 15 IPS
- Tape: Type II (Chrome)
- Input Drive: +2 to +4 dB
- Saturation: 40-50%
- Warmth: 40%
- Head Bump: 50-60%
- Bump Freq: 80 Hz
- Wow/Flutter: 0%
- Mix: 100%

**Vintage Drum Character:**
- Machine: 7.5 IPS
- Tape: Type I (Ferric)
- Input Drive: +6 dB
- Saturation: 60-70%
- Warmth: 60%
- Head Bump: 70%
- Bump Freq: 60 Hz
- Wow: 10-20%
- Flutter: 15%
- Age: 30-50%
- Mix: 80-100%

**Parallel Drum Crush:**
- Machine: 15 IPS
- Tape: Type I (Ferric)
- Input Drive: +8 to +12 dB
- Saturation: 80-100%
- Mix: 20-40%

### Guitar Bus / Individual Tracks

Tape saturation is a secret weapon for guitar tones - it smooths harsh frequencies while adding harmonic richness.

**Clean/Crunch Guitars:**
- Machine: 15 IPS
- Tape: Modern
- Input Drive: 0 dB
- Saturation: 30-40%
- Warmth: 50%
- Head Bump: 30%
- Wow/Flutter: 0%
- Mix: 100%

**Heavy Rhythm Guitars:**
- Machine: 15 IPS
- Tape: Type II (Chrome)
- Input Drive: +3 dB
- Saturation: 50-60%
- Warmth: 40%
- Head Bump: 40%
- Bump Freq: 100 Hz
- Mix: 100%

**Lo-Fi/Vintage Guitar:**
- Machine: 7.5 IPS
- Tape: Type I (Ferric)
- Input Drive: +6 dB
- Saturation: 70%
- Warmth: 60%
- Wow: 25%
- Flutter: 20%
- Hiss: 10-20%
- Age: 50%
- Mix: 100%

### Bass Guitar

Tape saturation helps bass cut through the mix while adding harmonic content that translates well on small speakers.

**Modern Rock Bass:**
- Machine: 15 IPS
- Tape: Type II (Chrome)
- Input Drive: +2 dB
- Saturation: 40-50%
- Warmth: 30%
- Head Bump: 70%
- Bump Freq: 80-100 Hz
- Mix: 100%

**Vintage Thump Bass:**
- Machine: 7.5 IPS
- Tape: Type I (Ferric)
- Input Drive: +4 dB
- Saturation: 50%
- Warmth: 50%
- Head Bump: 80%
- Bump Freq: 60 Hz
- Mix: 100%

### Vocals

Subtle tape saturation can add presence and smoothness to vocals without being obvious.

**Lead Vocal Polish:**
- Machine: 30 IPS
- Tape: Modern
- Input Drive: 0 dB
- Saturation: 20-30%
- Warmth: 30%
- Head Bump: 0-20%
- Wow/Flutter: 0%
- Mix: 60-100%

**Vintage Vocal Character:**
- Machine: 15 IPS
- Tape: Type I (Ferric)
- Input Drive: +2 dB
- Saturation: 40%
- Warmth: 50%
- Wow: 5-10%
- Age: 20%
- Mix: 100%

### Mix Bus / Mastering

Tape on the mix bus is the classic "mastering" move for adding analog warmth and cohesion.

**Mix Bus Warmth - Subtle:**
- Machine: 30 IPS
- Tape: Modern
- Input Drive: 0 dB
- Saturation: 15-25%
- Warmth: 20-30%
- Head Bump: 20-30%
- Bump Freq: 60 Hz
- Wow/Flutter: 0%
- Mix: 100%

**Mix Bus - More Character:**
- Machine: 15 IPS
- Tape: Type II (Chrome)
- Input Drive: +1 to +2 dB
- Saturation: 30-40%
- Warmth: 40%
- Head Bump: 40%
- Bump Freq: 80 Hz
- Mix: 100%

---

## Recommended Settings

### Quick Reference Table

| Application | Machine | Tape | Drive | Saturation | Warmth | Head Bump |
|------------|---------|------|-------|------------|--------|-----------|
| Drum Bus | 15 IPS | Type II | +3 dB | 50% | 40% | 60% |
| Vintage Drums | 7.5 IPS | Type I | +6 dB | 70% | 60% | 70% |
| Guitars | 15 IPS | Modern | 0 dB | 35% | 50% | 30% |
| Heavy Guitars | 15 IPS | Type II | +3 dB | 55% | 40% | 40% |
| Bass | 15 IPS | Type II | +2 dB | 45% | 30% | 70% |
| Lead Vocal | 30 IPS | Modern | 0 dB | 25% | 30% | 10% |
| Mix Bus | 30 IPS | Modern | 0 dB | 20% | 25% | 25% |

### Machine Type Guide

- **7.5 IPS**: Maximum warmth, reduced high frequencies, most pronounced tape effects - vintage/lo-fi
- **15 IPS**: Balanced warmth and clarity - the classic "studio" sound, works on everything
- **30 IPS**: Most transparent, extended high frequencies - best for mastering and clean sources

### Tape Type Guide

- **Type I (Ferric)**: Warmest, most saturated - vintage character, great for lo-fi
- **Type II (Chrome)**: Balanced saturation with better high-frequency response - the workhorse
- **Modern**: Cleanest formulation, subtle saturation - best for transparent warmth

---

## Signal Flow Tips

### Where to Place TapeWarm

1. **End of Insert Chain**: Place after EQ and compression for classic tape "printing"

2. **Before Compression**: Can soften transients before they hit a compressor for smoother compression

3. **On Individual Tracks**: Add character to key elements (kick, snare, bass, vocals)

4. **On Buses**: Unify groups of tracks with cohesive analog character

5. **Mix Bus (Last)**: Add final polish and warmth before limiting

### Gain Staging

- Use Input Drive to push into saturation
- Match Output to Input level for proper A/B comparison
- Watch for excessive level buildup from Head Bump on bass-heavy material

---

## Combining with Other Plugins

### Drum Chain
1. **Bus Glue** - compression first
2. **TapeWarm** - add warmth and weight
3. **StereoImager** - final width adjustment

### Guitar Chain
1. **PDLBRD** - amp and effects
2. **TapeWarm** - smooth and warm the tone
3. **Bus Glue** (optional) - subtle glue compression

### Mix Bus Chain
1. **Bus Glue** - glue compression
2. **MasterBus** - EQ shaping
3. **TapeWarm** - analog warmth (subtle)
4. **Automaster** - final limiting

### Bass Chain
1. **NeveStrip** - EQ and compression
2. **TapeWarm** - add harmonics and weight

---

## Quick Start Guide

**Add warmth to your drum bus in 30 seconds:**

1. Insert TapeWarm on your drum bus
2. Set **Machine** to 15 IPS
3. Set **Tape** to Type II (Chrome)
4. Set **Input Drive** to +3 dB
5. Set **Saturation** to 50%
6. Set **Warmth** to 40%
7. Set **Head Bump** to 60%
8. Set **Bump Freq** to 80 Hz
9. Leave Wow/Flutter at 0%
10. Adjust **Output** to match bypass level
11. A/B to confirm improvement

**Add subtle mix bus warmth in 30 seconds:**

1. Insert TapeWarm on your mix bus
2. Set **Machine** to 30 IPS
3. Set **Tape** to Modern
4. Leave **Input Drive** at 0 dB
5. Set **Saturation** to 20%
6. Set **Warmth** to 25%
7. Set **Head Bump** to 25%
8. Set **Bump Freq** to 60 Hz
9. Leave Wow/Flutter/Hiss at 0%
10. Match Output to Input
11. Compare with bypass - it should sound "better" without being obviously different
