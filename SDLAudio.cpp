#include "SDLAudio.h"
#include <cassert>
#include <cstring>

#include <iostream>
#include <string>
using std::cout;
using std::string;

// Sound Effect Constructor
SDLAudio::SoundEffect::SoundEffect(Mix_Chunk* effect, const char* fileName) : effect(effect),
fileName(fileName), refCount(1) {
    channels.clear();
}

// Constructor
SDLAudio::SDLAudio() {
    music = nullptr;
    pausedMusic = false;
    soundCount = 0;
}

// Assignment operator & Copy Constructor (Not used)
SDLAudio::SDLAudio(const SDLAudio& rhs) { }
SDLAudio& SDLAudio::operator=(const SDLAudio& rhs) { return *this; }

// Destructor
SDLAudio::~SDLAudio() {
    Shutdown();
}

// Instance function
SDLAudio* SDLAudio::Instance() {
    // Lazy instantiation
    static SDLAudio instance;
    return &instance;
}

// Initializes all audio
void SDLAudio::Init() {
    // Stop any previously loaded stuff
    Shutdown();

    // Initialize support for all these types
    int mask = MIX_INIT_FLAC | MIX_INIT_MOD
        | MIX_INIT_MP3 | MIX_INIT_OGG;

    // Attempt to initialize for all types
    assert(Mix_Init(mask) == mask && "Unable to Initialize for all file types");

    // Open audio channels
    assert(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT,
        MIX_DEFAULT_CHANNELS, 4096) != -1 && "Unable to Open Audio channels");

    // Set the Audio Hook for finished sound effects
    Mix_ChannelFinished((&ChannelFinished));
}

// Cleans up any dynamic memory allocated for audio
void SDLAudio::Shutdown() {

    // Stop & Unload any music
    UnloadMusic();

    // Stop & Unload any sound effects
    if (soundEffects.size() > 0) {
        Mix_HaltChannel(-1);
        for (unsigned int ii = 0; ii < soundEffects.size(); ++ii) {
            // Unload the mix chunk
            Mix_FreeChunk(soundEffects[ii].effect);
        }
        // Clear the vector
        soundEffects.clear();
    }

    soundCount = 0;

    // Close the audio
    Mix_CloseAudio();

    // Quit the SDL sound
    Mix_Quit();
}

// Loads a song for playing
void SDLAudio::LoadMusic(const char* fileName) {
    // Clean up any existing song
    UnloadMusic();
    // Load the song
    music = Mix_LoadMUS(fileName);
    assert(music != nullptr && "Failed to load the music file");
}

// Changes the loaded song & instantly begins playing it
void SDLAudio::ChangeMusic(const char* fileName, int loop) {
    // Clean up any existing song
    UnloadMusic();
    // Load the song
    music = Mix_LoadMUS(fileName);
    assert(music != nullptr && "Failed to load the music file");

    // Play the song
    assert(Mix_PlayMusic(music, 1) != -1 && "Failed to play the new song during changeover");
}

// Unloads the loaded music
void SDLAudio::UnloadMusic() {
    if (music) {
        Mix_FreeMusic(music);
        music = nullptr;
    }
    pausedMusic = false;
}

void SDLAudio::StartMusic(int loop, bool overridePause, int milliseconds, bool fade) {
    // If the song is valid and...
    // We're not paused
    // Or we are paused and can override pauses
    if (music && (!pausedMusic || (pausedMusic && overridePause))) {
        if (fade) {
            assert(Mix_FadeInMusic(music, loop, milliseconds) != -1 && "An error occurred attempting to play the song");
        }
        else {
            assert(Mix_PlayMusic(music, loop) != -1 && "An error occurred attempting to play the song");
        }
    }
    // We're paused and cannot override pauses
    else {
        // Resume
        Mix_ResumeMusic();
        pausedMusic = false;
    }
}

// Play music (0 means play 0 times, -1 forever)
void SDLAudio::PlayMusic(int loop, bool overridePause) {
    StartMusic(loop, overridePause, 0, false);
}

// Stop music
void SDLAudio::StopMusic() {
    Mix_HaltMusic();
    pausedMusic = false;
}

// Pause music
void SDLAudio::PauseMusic() {
    Mix_PauseMusic();
    pausedMusic = true;
}

// Fade out music
void SDLAudio::FadeOutMusic(int milliseconds) {
    assert(Mix_FadeOutMusic(milliseconds) == 1 && "Failed to initiate the Fade Out of a song");
}

// Start playing a song, fading in at the start
void SDLAudio::FadeInMusic(int milliseconds, int loop, bool overridePause) {
    StartMusic(loop, overridePause, milliseconds, true);
}

// Get the volume
int SDLAudio::MusicVolume() {
    return Mix_VolumeMusic(-1);
}

// 0 = mute, 128 = MIX_MAX_VOLUME = max
void SDLAudio::MusicVolume(int volume) {
    // returns the old volume (int)
    Mix_VolumeMusic(volume);
}

// Mutes the music
void SDLAudio::MuteMusic() {
    this->MusicVolume(0);
}

bool SDLAudio::ValidSoundIndex(int soundIndex) const {
    return (soundIndex >= 0 && soundIndex < soundEffects.size() && soundEffects[soundIndex].refCount > 0);
}

// Loads a sound effect
int SDLAudio::LoadSound(const char* fileName) {
    // For every loaded sound
    for (unsigned int ii = 0; ii < soundEffects.size(); ++ii) {
        // Does it match?
        if (soundEffects[ii].fileName == fileName) {
            if (soundEffects[ii].refCount <= 0) {
                Mix_Chunk* newSound = Mix_LoadWAV(fileName);
                assert(newSound != nullptr && "Failed to load Sound Effect");
                soundEffects[ii].effect = newSound;
                soundEffects[ii].refCount = 1;
                return ii;
            }
            // Increase ref count, return that number
            soundEffects[ii].refCount++;
            return ii;
        }
        // Use this spot if it's empty
        else if (soundEffects[ii].refCount <= 0) {
            Mix_Chunk* newSound = Mix_LoadWAV(fileName);
            assert(newSound != nullptr && "Failed to load Sound Effect");
            soundEffects[ii].effect = newSound;
            soundEffects[ii].refCount = 1;
            soundEffects[ii].fileName = fileName;
            return ii;
        }
    }

    // Actually load this fucker
    Mix_Chunk* newSound = Mix_LoadWAV(fileName);
    assert(newSound != nullptr && "Failed to load Sound Effect");
    SoundEffect newGuy(newSound, fileName);
    // Stuff into list, return index
    soundEffects.push_back(newGuy);
    return (int)soundEffects.size() - 1;
}

// Unloads a sound
void SDLAudio::UnloadSound(int soundIndex) {
    // Only bother if given a valid index
    if (ValidSoundIndex(soundIndex)) {
        // Decrement the refCount at the chosen index
        soundEffects[soundIndex].refCount--;
        // If the refCount has hit 0, unload the sound
        if (soundEffects[soundIndex].refCount <= 0) {
            // Unload the effect, if he exists
            Mix_FreeChunk(soundEffects[soundIndex].effect);
            soundEffects[soundIndex].effect = nullptr;
            soundEffects[soundIndex].refCount = 0;
            soundEffects[soundIndex].fileName = "UNINITIALIZED";
        }
    }
}

// Plays a sound effect
int SDLAudio::PlaySoundEffect(int soundIndex, int loop) {
    return StartSound(soundIndex, loop, 0, false);
}

// This function is called when a SoundEffect is finished playing. It removes a channel from a SoundEffect
void ChannelFinished(int channel) {
    cout << "Sound finished... ";
    // For every sound effect
    for (unsigned int ii = 0; ii < SDLAudio::Instance()->soundEffects.size(); ++ii) {
        // For every channel it has
        for (unsigned int jj = 0; jj < SDLAudio::Instance()->soundEffects[ii].channels.size(); ++jj) {
            if (channel == SDLAudio::Instance()->soundEffects[ii].channels[jj]) {
                SDLAudio::Instance()->soundEffects[ii].channels.erase(SDLAudio::Instance()->soundEffects[ii].channels.begin() + jj);
                SDLAudio::Instance()->soundCount -= 1;
                cout << "And removed! " << SDLAudio::Instance()->soundCount << "\n";
                return;
            }
        }
    }
}

// Stop sound
void SDLAudio::StopSoundEffect(int soundIndex, int channel) {
    // Don't bother if it's a bad index
    if (ValidSoundIndex(soundIndex) && channel != -1) {
        for (unsigned int ii = 0; ii < soundEffects[soundIndex].channels.size(); ++ii) {
            // If we have this channel... stop it
            if (soundEffects[soundIndex].channels[ii] == channel) {
                Mix_HaltChannel(soundEffects[soundIndex].channels[ii]);
                // Callback gets called automatically
            }
        }
    }
}

// Fade in sound
int SDLAudio::FadeInSoundEffect(int soundIndex, int milliseconds, int loop) {
    return StartSound(soundIndex, loop, milliseconds, true);
}

// Actually starts the sound effect. This function is shared between Play and FadeIn
int SDLAudio::StartSound(int soundIndex, int loop, int milliseconds, bool fade) {
    // Only bother if given a valid index
    int channel = -1;
    if (ValidSoundIndex(soundIndex)) {
        if (!fade) {
            channel = Mix_PlayChannel(-1, soundEffects[soundIndex].effect, loop);
        }
        else {
            channel = Mix_FadeInChannel(-1, soundEffects[soundIndex].effect, loop, milliseconds);
        }
        string temp(Mix_GetError());
        // If there are no channels left, it will not play. This is okay.
        if (channel == -1 && strcmp(Mix_GetError(), "No free channels available") != 0) {
            // Any other reason though,... freak out
            assert(channel != -1 && "An error occurred while attempting to play a sound effect");
        }
        // If it played
        else if (channel != -1) {
            // Keep track
            ++soundCount;
            cout << "Played: " << soundCount << "\n";
            soundEffects[soundIndex].channels.push_back(channel);
        }
    }
    return channel;
}

// Fade out sound
void SDLAudio::FadeOutSoundEffect(int soundIndex, int channel, int milliseconds) {
    // Don't bother if the index is bad
    if (ValidSoundIndex(soundIndex)) {
        Mix_FadeOutChannel(channel, milliseconds);
    }
}

// Pause sound
void SDLAudio::PauseSoundEffect(int soundIndex, int channel) {
    if (ValidSoundIndex(soundIndex)) {
        Mix_Pause(channel);
    }
}

// Mute sound
void SDLAudio::MuteSoundEffects() {
    Mix_Volume(-1, 0);
}

// Set the sound effects volume, 0 = mute, 128 = max
void SDLAudio::SoundEffectsVolume(int newVolume) {
    Mix_Volume(-1, newVolume);
}

// Get the sound effects volume
int SDLAudio::SoundEffectsVolume() {
    return Mix_Volume(-1, -1);
}

// Mute sound & music
void SDLAudio::Mute() {
    MuteMusic();
    MuteSoundEffects();
}