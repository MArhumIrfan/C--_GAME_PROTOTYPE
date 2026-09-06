#include "AudioState.h"

#include <cmath>
#include <algorithm>

void audioCallback(void* userdata, Uint8* stream, int len) {
    AudioState* audio = static_cast<AudioState*>(userdata);
    int16_t* buffer = reinterpret_cast<int16_t*>(stream);
    int samples = len / sizeof(int16_t);
    float sampleDt = 1.0f / AUDIO_SAMPLE_RATE;

    auto getAudioNoise = [](uint32_t& seed) -> float {
        seed = seed * 1664525 + 1013904223;
        return (static_cast<float>(seed >> 16) / 32768.0f) * 2.0f - 1.0f;
    };

    for (int i = 0; i < samples; ++i) {
        audio->globalTime += sampleDt;
        float finalSample = 0.0f;
        float bgm = 0.0f;
        float t = audio->globalTime;

        if (audio->gameState == STATE_TITLE || audio->gameState == STATE_SUCCESS) {
            // CUSTOM BEEPBOX THEME (150 BPM)
            int step = int(t * 10.0f) % 16; 
            
            // Your exact melody: Pattern 1 followed by Pattern 2
            float freqs[16] = {
                1046.50f, 1046.50f, 783.99f, 0.0f, 880.00f, 880.00f, 0.0f, 880.00f, // Pattern 1
                880.00f, 783.99f, 659.25f, 523.25f, 523.25f, 523.25f, 880.00f, 880.00f  // Pattern 2
            };
            
            float f = freqs[step];
            float sq = (f > 0.0f) ? ((std::fmod(t * f, 1.0f) > 0.5f) ? 0.15f : -0.15f) : 0.0f;
            
            // Subtle C-root bassline to support your melody
            int bassStep = int(t * 5.0f) % 8;
            float bassFreq = (bassStep == 0 || bassStep == 3 || bassStep == 6) ? 65.41f : 0.0f; 
            if (bassFreq > 0.0f) sq += (std::fmod(t * bassFreq, 1.0f) > 0.5f) ? 0.2f : -0.2f;
            
            finalSample = sq;
        }

        else if (audio->gameState == STATE_GAMEOVER) {
            // Same melodic shape as the menu theme, but defeated: half tempo,
            // dropped an octave, and with random dropouts like a record that's
            // given up - plus a low drone underneath instead of the bright bass.
            int step = int(t * 3.0f) % 16;
            float freqs[16] = {261.63f, 329.63f, 392.0f, 523.25f, 392.0f, 329.63f, 261.63f, 196.0f, 261.63f, 329.63f, 392.0f, 523.25f, 659.25f, 523.25f, 392.0f, 329.63f};
            float f = freqs[step] * 0.5f;
            bool dropout = std::abs(getAudioNoise(audio->rngSeed)) < 0.35f;
            float sq = (std::fmod(t * f, 1.0f) > 0.5f) ? 0.15f : -0.15f;
            float lowDrone = std::sin(t * 55.0f * 6.283f) * 0.1f;
            finalSample = dropout ? lowDrone * 0.5f : (sq * 0.6f + lowDrone);
        } 
        else if (audio->gameState == STATE_JUMPSCARE) {
            audio->screamPhase += (350.0f * 2.0f * 3.14159f) * sampleDt;
            if (audio->screamPhase > 2.0f * 3.14159f) audio->screamPhase -= 2.0f * 3.14159f;
            float screech = std::sin(audio->screamPhase) * 0.5f;
            float rawNoise = getAudioNoise(audio->rngSeed) * 0.7f;
            float demonicRumble = std::sin(audio->screamPhase * 0.1f) * 0.4f;
            finalSample = screech + rawNoise + demonicRumble;
        } 
        else if (audio->gameState == STATE_PLAYING) {
            int lvl = audio->currentLevel;
            
            // BACKGROUND MUSIC & AMBIENCE GENERATOR
            if (lvl <= 10) {
                // Peaceful ethereal chord
                float chord = std::sin(t * 130.81f * 6.283f) + std::sin(t * 164.81f * 6.283f) + std::sin(t * 196.0f * 6.283f);
                bgm = chord * 0.04f;
            } 
            else if (lvl <= 13) {
                // Music corrupting and glitching out
                float corr = (lvl - 10) / 3.0f;
                if (std::abs(getAudioNoise(audio->rngSeed)) < corr) t += getAudioNoise(audio->rngSeed) * 0.1f;
                float chord = std::sin(t * 130.81f * 6.283f) + std::sin(t * 164.81f * 6.283f) + std::sin(t * 196.0f * 6.283f);
                bgm = chord * (0.04f - (corr * 0.02f)) + getAudioNoise(audio->rngSeed) * corr * 0.03f;
            } 
            else if (lvl <= 15) {
                // Absolute Silence (14-15)
                bgm = 0.0f;
            } 
            else {
                // Level 16+: Mist / Stalker Ambience
                // The hum drifts sharper and the wind thickens as corruption
                // climbs, so the base ambience itself slowly sours over time
                // rather than staying static.
                float detune = audio->corruption * 4.0f;
                float hum = std::sin(t * (45.0f + detune) * 6.283f) * 0.06f;
                float wind = getAudioNoise(audio->rngSeed) * (0.02f + audio->corruption * 0.015f);
                bgm = hum + wind;

                // Random distant hallucinations
                audio->ambientEventCooldown -= sampleDt;
                if (audio->ambientEventCooldown <= 0.0f) {
                    audio->ambientEventCooldown = 5.0f + (std::abs(getAudioNoise(audio->rngSeed)) * 10.0f);
                    audio->ambientEventTimer = 1.0f;
                    audio->ambientEventPhase = 0.0f;
                    audio->ambientEventType = (std::abs(getAudioNoise(audio->rngSeed)) > 0.5f) ? 1 : 2;
                }
                if (audio->ambientEventTimer > 0.0f) {
                    audio->ambientEventTimer -= sampleDt;
                    audio->ambientEventPhase += sampleDt;
                    float pt = audio->ambientEventPhase;
                    if (audio->ambientEventType == 1) { // Distant door scrape
                        float env = std::exp(-pt * 3.0f);
                        bgm += getAudioNoise(audio->rngSeed) * env * 0.05f;
                    } else { // Distant heavy thud
                        float env = std::exp(-pt * 5.0f);
                        bgm += std::sin(pt * 50.0f * 6.283f) * env * 0.15f;
                    }
                }

                // Level 20+: the peaceful chord from the tutorial levels
                // bleeds back through - the SAME "old music" the player
                // heard early on, now corrupted - growing louder and more
                // distracting the deeper the game goes.
                // Level 20+: Your custom melody returns, but corrupted
                if (lvl >= 20) {
                    float resurgeVol = std::min(1.0f, (lvl - 19) * 0.12f);
                    float jitterAmt = std::min(1.0f, (lvl - 19) * 0.08f);
                    float ct = t;
                    
                    // Time distortion (skipping)
                    if (std::abs(getAudioNoise(audio->rngSeed)) < jitterAmt) ct += getAudioNoise(audio->rngSeed) * 0.15f;
                    bool dropout = std::abs(getAudioNoise(audio->rngSeed)) < (jitterAmt * 0.15f);
                    
                    // The same sequence
                    int step = int(ct * 10.0f) % 16;
                    float freqs[16] = {
                        1046.50f, 1046.50f, 783.99f, 0.0f, 880.00f, 880.00f, 0.0f, 880.00f, 
                        880.00f, 783.99f, 659.25f, 523.25f, 523.25f, 523.25f, 880.00f, 880.00f
                    };
                    
                    // Detune the pitch based on the corruption jitter
                    float f = freqs[step] * (1.0f + getAudioNoise(audio->rngSeed) * (jitterAmt * 0.3f));
                    float sq = (f > 0.0f) ? ((std::fmod(ct * f, 1.0f) > 0.5f) ? 0.15f : -0.15f) : 0.0f;

                    if (!dropout) bgm += sq * 0.3f * resurgeVol;
                }
             }

            // GAMEPLAY AUDIO (Overlays the BGM)
            float footstep = 0.0f;
            if (audio->isMoving && !audio->isCrouching) {
                float stepFreq = audio->isSprinting ? 4.5f : 2.5f;
                audio->footstepPhase += (stepFreq * 2.0f * 3.14159f) * sampleDt;
                if (audio->footstepPhase > 2.0f * 3.14159f) audio->footstepPhase -= 2.0f * 3.14159f;
                float stepEnv = std::max(0.0f, std::sin(audio->footstepPhase));
                stepEnv = std::pow(stepEnv, 6.0f); 
                
                float stepNoise = getAudioNoise(audio->rngSeed);
                if (audio->floorSurface == 1) { 
                    float thud = std::sin(audio->footstepPhase * 10.0f);
                    footstep = (stepNoise * 0.3f + thud * 0.7f) * stepEnv * 0.2f;
                } else if (audio->floorSurface == 2) { 
                    footstep = stepNoise * stepEnv * 0.08f;
                } else { 
                    footstep = stepNoise * stepEnv * 0.15f; 
                }
            } else {
                audio->footstepPhase = 0.0f; 
            }

            float heartBPM = 1.0f + (100.0f - audio->sanity) / 100.0f * 2.0f;
            audio->heartbeatPhase += (heartBPM * 2.0f * 3.14159f) * sampleDt;
            if (audio->heartbeatPhase > 2.0f * 3.14159f) audio->heartbeatPhase -= 2.0f * 3.14159f;
            float mixHeart = std::clamp((audio->corruption - 0.5f) * 3.0f, 0.0f, 1.0f);
            float beatEnv = 0.0f;
            float cyclePos = audio->heartbeatPhase / (2.0f * 3.14159f);
            if (cyclePos < 0.15f) beatEnv = std::sin(cyclePos / 0.15f * 3.14159f);
            else if (cyclePos > 0.22f && cyclePos < 0.35f) beatEnv = std::sin((cyclePos - 0.22f) / 0.13f * 3.14159f) * 0.7f;
            float heartbeat = std::sin(audio->heartbeatPhase * 40.0f) * beatEnv * (0.35f + (100.0f - audio->sanity) / 100.0f * 0.50f) * mixHeart;

            float monsterAudio = 0.0f;
            if (audio->isChasing || audio->closestEnemyDist < 15.0f) {
                float proxVol = std::pow(std::max(0.0f, 1.0f - (audio->closestEnemyDist / 12.0f)), 2.0f);
                float breathFreq = audio->isChasing ? 3.0f : 0.8f; 
                
                audio->monsterPhase += (breathFreq * 2.0f * 3.14159f) * sampleDt;
                if (audio->monsterPhase > 2.0f * 3.14159f) audio->monsterPhase -= 2.0f * 3.14159f;
                
                float breathEnv = std::sin(audio->monsterPhase) * 0.5f + 0.5f;
                float raspyNoise = getAudioNoise(audio->rngSeed) * 0.6f + 0.4f; 
                monsterAudio = raspyNoise * breathEnv * proxVol * 0.6f;
            }

            float itemSound = 0.0f;
            if (audio->itemSoundTimer > 0.0f) {
                audio->itemSoundTimer -= sampleDt;
                audio->itemSoundPhase += sampleDt;
                float it = audio->itemSoundPhase;

                if (audio->itemSoundType == 1) { 
                    float env = 0.0f;
                    if (it < 0.05f) env = std::exp(-it * 100.0f);
                    else if (it > 0.1f && it < 0.15f) env = std::exp(-(it - 0.1f) * 100.0f);
                    itemSound = std::sin(it * 2.0f * 3.14159f * 2500.0f) * env * 0.4f;
                }
                else if (audio->itemSoundType == 2) { 
                    itemSound = std::sin(it * 2.0f * 3.14159f * 1800.0f) * std::exp(-it * 8.0f) * 0.4f;
                }
                else if (audio->itemSoundType == 3) { 
                    itemSound = (getAudioNoise(audio->rngSeed) * 0.4f + std::sin(it * 2.0f * 3.14159f * 100.0f) * 0.6f) * std::exp(-it * 12.0f) * 0.4f;
                }
                else if (audio->itemSoundType == 4) { 
                    itemSound = getAudioNoise(audio->rngSeed) * std::exp(-it * 30.0f) * 0.6f;
                }
                else if (audio->itemSoundType == 5 || audio->itemSoundType == 6) { 
                    itemSound = getAudioNoise(audio->rngSeed) * std::exp(-it * 15.0f) * 0.4f;
                }
                else if (audio->itemSoundType == 7) {  
                    itemSound = (getAudioNoise(audio->rngSeed) * 0.6f + std::sin(it * 2.0f * 3.14159f * 50.0f) * 0.4f) * std::exp(-it * 5.0f) * 0.8f;
                }
                else if (audio->itemSoundType == 8) {  
                    itemSound = std::sin(it * 2.0f * 3.14159f * 800.0f) * std::exp(-it * 10.0f) * 0.3f;
                }
                else if (audio->itemSoundType == 9) { 
                    itemSound = std::sin(it * 2.0f * 3.14159f * 4000.0f) * std::sin(it * 2.0f * 3.14159f * 4100.0f) * std::exp(-it * 15.0f) * 0.3f;
                }
            }

            float vocalAudio = 0.0f;
            if (audio->monsterVocalTimer > 0.0f) {
                audio->monsterVocalTimer -= sampleDt;
                audio->monsterVocalPhase += sampleDt;
                float vt = audio->monsterVocalPhase;
                
                float distScale = std::pow(std::clamp(1.0f - (audio->closestEnemyDist / 20.0f), 0.0f, 1.0f), 1.5f);

                if (audio->monsterVocalType == 1) { // Growl
                    float env = std::sin((vt / 2.0f) * 3.14159f); 
                    if (env < 0.0f) env = 0.0f;
                    float wave = std::sin(vt * 2.0f * 3.14159f * 35.0f) * 0.5f + std::sin(vt * 2.0f * 3.14159f * 45.0f) * 0.5f;
                    float rumble = getAudioNoise(audio->rngSeed) * 0.3f;
                    vocalAudio = (wave + rumble) * env * 1.2f * distScale;
                } else if (audio->monsterVocalType == 2) { // Hiss
                    float env = std::exp(-vt * 4.0f);
                    vocalAudio = getAudioNoise(audio->rngSeed) * env * 0.6f * distScale;
                } else if (audio->monsterVocalType == 3) { // FM Scream
                    float env = std::exp(-vt * 1.5f);
                    float mod = std::sin(vt * 2.0f * 3.14159f * 15.0f) * 200.0f; 
                    float wave = std::sin(vt * 2.0f * 3.14159f * (800.0f + mod));
                    float harsh = getAudioNoise(audio->rngSeed) * 0.5f;
                    vocalAudio = (wave * 0.7f + harsh) * env * 1.5f * distScale; 
                }
            }

            finalSample = bgm + footstep + heartbeat + monsterAudio + itemSound + vocalAudio;
        }

        buffer[i] = static_cast<int16_t>(std::clamp(finalSample * audio->masterVolume, -1.0f, 1.0f) * 32767.0f);
    }
}
