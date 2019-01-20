#include "SDLAudio.h"
#include <cassert>

SDLAudio::SoundEffect::SoundEffect(Mix_Chunk* effect, const char* fileName) : effect(effect),
fileName(fileName), refCount(1), currentChannel(-1) {}

// Constructor
SDLAudio::SDLAudio() {
    music = nullptr;
    pausedMusic = false;
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
    return (soundIndex >= 0 && soundIndex < soundEffects.size());
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
    if (ValidSoundIndex(soundIndex) && soundEffects[soundIndex].refCount > 0) {
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

//int Mix_PlayChannel(int channel, Mix_Chunk *chunk, int loops)
void SDLAudio::PlaySoundEffect(int soundIndex, int loop) {
    // Only bother if given a valid index
    if (ValidSoundIndex(soundIndex)) {
        soundEffects[soundIndex].currentChannel = Mix_PlayChannel(-1, soundEffects[soundIndex].effect, loop);
        assert(soundEffects[soundIndex].currentChannel != -1 && "An error occurred while attempting to play a sound effect");
    }
}