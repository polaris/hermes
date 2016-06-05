#ifndef _Sound_H
#define _Sound_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <unordered_map>

class Sound {
public:
    ~Sound();

    static Sound* getInstance();

    void loadSound(unsigned int soundId, const char* filename);

    void playSound(unsigned int soundId);

private:
    Sound();

    std::unordered_map<unsigned int, Mix_Chunk*> chunckMap_;
};

#endif  // _Sound_H
