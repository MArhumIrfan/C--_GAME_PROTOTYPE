#include "AudioSystem.h"
#include "Constants.h"
#include <cmath>
#include <algorithm>

// This is the static callback function required by SDL
void AudioSystem::audioCallback(void* userdata, Uint8* stream, int len) {
    auto* audio = static_cast<AudioSystem*>(userdata);
    auto* buffer = reinterpret_cast<int16_t*>(stream);
    int samples = len / sizeof(int16_t);
    float sampleDt = 1.0f / AUDIO_SAMPLE_RATE;

    auto getAudioNoise = [](uint32_t& seed) -> float {
        seed = seed * 1664525 + 1013904223;
        return (static_cast<float>(seed >> 16) / 32768.0f) * 2.0f - 1.0f;
    };

    for (int i = 0; i < samples; ++i) {
        if (!audio->inGame && !audio->isJumpscare) {
            buffer[i] = 0;
            continue;
        }

        float finalSample = 0.0f;
        if (audio->isJumpscare) {
            audio->screamPhase += (350.0f * 2.0f * M_PI) * sampleDt;
            float screech = std::sin(audio->screamPhase) * 0.5f;
            float rawNoise = getAudioNoise(audio->rngSeed) * 0.7f;
            float demonicRumble = std::sin(audio->screamPhase * 0.1f) * 0.4f;
            finalSample = screech + rawNoise + demonicRumble;
        } else {
            float mixAmbient = std::clamp((audio->corruption - 0.2f) * 4.0f, 0.0f, 1.0f);
            audio->ambientPhase += (42.0f * 2.0f * M_PI) * sampleDt;
            float ambient = std::sin(audio->ambientPhase) * 0.0896f * mixAmbient;

            float footstep = 0.0f;
            if (audio->isMoving && !audio->isHiding) {
                float stepFreq = audio->isSprinting ? 4.5f : 2.5f;
                audio->footstepPhase += (stepFreq * 2.0f * M_PI) * sampleDt;
                float stepEnv = std::pow(std::max(0.0f, std::sin(audio->footstepPhase)), 6.0f);
                footstep = getAudioNoise(audio->rngSeed) * stepEnv * 0.15f;
            } else { audio->footstepPhase = 0.0f; }

            float heartBPM = 1.0f + (100.0f - audio->sanity) / 100.0f * 2.0f;
            audio->heartbeatPhase += (heartBPM * 2.0f * M_PI) * sampleDt;
            float mixHeart = std::clamp((audio->corruption - 0.5f) * 3.0f, 0.0f, 1.0f);
            float beatEnv = 0.0f;
            float cyclePos = fmod(audio->heartbeatPhase, 2.0f * M_PI) / (2.0f * M_PI);
            if (cyclePos < 0.15f) beatEnv = std::sin(cyclePos / 0.15f * M_PI);
            else if (cyclePos > 0.22f && cyclePos < 0.35f) beatEnv = std::sin((cyclePos - 0.22f) / 0.13f * M_PI) * 0.7f;
            float heartbeat = std::sin(audio->heartbeatPhase * 40.0f) * beatEnv * (0.35f + (100.0f - audio->sanity) / 100.0f * 0.50f) * mixHeart;

            float monsterAudio = 0.0f;
            if (audio->isChasing || audio->closestEnemyDist < 15.0f) {
                float proxVol = std::max(0.0f, 1.0f - (audio->closestEnemyDist / 15.0f));
                float breathFreq = audio->isChasing ? 2.5f : 0.8f;
                audio->monsterPhase += (breathFreq * 2.0f * M_PI) * sampleDt;
                float breathEnv = std::sin(audio->monsterPhase) * 0.5f + 0.5f;
                monsterAudio = getAudioNoise(audio->rngSeed) * breathEnv * proxVol * 0.4f * mixAmbient;
            }

            float breathing = 0.0f;
            float breathFreq = audio->isSprinting ? 2.0f : (audio->closestEnemyDist < 8.0f ? 1.5f : 0.8f);
            float breathVol = audio->isSprinting ? 0.08f : (audio->closestEnemyDist < 8.0f ? 0.06f : 0.04f);
            audio->breathingPhase += (breathFreq * 2.0f * M_PI) * sampleDt;
            breathing = (getAudioNoise(audio->rngSeed) * 0.4f + std::sin(audio->breathingPhase) * 0.6f) * (std::sin(audio->breathingPhase*0.5f) * 0.5f + 0.5f) * breathVol;
            
            float fakeFootstep = 0.0f;
            audio->fakeFootstepTimer -= sampleDt;
            if (audio->sanity < 40.0f && audio->fakeFootstepTimer <= 0.0f && (rand() % 5000) < 10) {
                audio->fakeFootstepTimer = 2.0f + (rand() % 3);
            }
            if(audio->fakeFootstepTimer > 1.5f){
                audio->fakeFootstepPhase += (3.5f * 2.0f * M_PI) * sampleDt;
                float stepEnv = std::pow(std::max(0.0f, std::sin(audio->fakeFootstepPhase)), 6.0f);
                fakeFootstep = getAudioNoise(audio->rngSeed) * stepEnv * 0.12f * (1.0f - audio->sanity / 40.0f);
            }

            float itemSound = 0.0f;
            if (audio->itemSoundTimer > 0.0f) {
                audio->itemSoundTimer -= sampleDt;
                float t = audio->itemSoundTimer > 0.0f ? 1.0f - (audio->itemSoundTimer / 0.5f) : 1.0f;
                float env = std::exp(-t * 12.0f);
                 if (audio->itemSoundType == 1) itemSound = getAudioNoise(audio->rngSeed) * env * 0.4f; // Pebble
                 else if (audio->itemSoundType == 2 || audio->itemSoundType == 5) itemSound = std::sin(audio->itemSoundPhase * 900.0f) * env * 0.3f; // Meds/Oil
                 else if (audio->itemSoundType == 3) itemSound = getAudioNoise(audio->rngSeed) * env * 0.3f; // Bread
                 else if (audio->itemSoundType == 4) itemSound = getAudioNoise(audio->rngSeed) * env * 0.6f; // Lantern
                 audio->itemSoundPhase += sampleDt;
            }

            finalSample = ambient + footstep + heartbeat + monsterAudio + itemSound + breathing + fakeFootstep;
        }

        if (audio->isHiding) finalSample *= 0.3f; // Muffle
        buffer[i] = static_cast<int16_t>(std::clamp(finalSample * audio->masterVolume, -1.0f, 1.0f) * 32767.0f);
    }
}

void AudioSystem::triggerItemSound(ItemType type) {
    itemSoundType = (int)type;
    itemSoundTimer = 0.5f;
    itemSoundPhase = 0.0f;
}

void AudioSystem::triggerStalkerSound(const std::string& type) {
    // Placeholder for vocalizations - could be implemented with more sounds
    if (type == "hunt") {
        itemSoundType = 6; // Example
        itemSoundTimer = 1.0f;
    } else if (type == "close") {
        itemSoundType = 7; // Example
        itemSoundTimer = 0.5f;
    }
}