#include "Sound.h"
#include "Logging.h"

#include <stdexcept>

Sound::~Sound() {
    for (auto& itr : chunckMap_) {
        Mix_FreeChunk(itr.second);
    }
    chunckMap_.clear();
}

Sound* Sound::getInstance() {
    static Sound sound;
    return &sound;
}

void Sound::loadSound(unsigned int soundId, const char* filename) {
    if (chunckMap_.find(soundId) != chunckMap_.end()) {
        throw std::logic_error("You can not use the same sound ID twice.");
    }
    auto sound = Mix_LoadWAV(filename);
    if (sound == nullptr) {
        throw std::runtime_error("Failed to load wave file.");
    }
    chunckMap_.insert(std::make_pair(soundId, sound));
}

void Sound::playSound(unsigned int soundId) {
    const auto itr = chunckMap_.find(soundId);
    if (itr != chunckMap_.end()) {
        if (Mix_PlayChannel(-1, itr->second, 0) == -1) {
            ERROR("Failed to play sound with ID {0}.", soundId);
        }
    } else {
        ERROR("Sound with ID {0} does not exist.", soundId);
    }
}

Sound::Sound()
: chunckMap_() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
        throw std::runtime_error("Failed to open audio.");
    }
}