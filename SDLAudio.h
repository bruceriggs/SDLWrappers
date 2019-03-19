#ifndef SDLAUDIO_H_
#define SDLAUDIO_H_

#include <vector>
#include "SDL_mixer.h"

using std::vector;

class SDLAudio {
private:
    struct SoundEffect {
        Mix_Chunk* effect;
        const char* fileName;
        int refCount;
        vector<int> channels;

        SoundEffect(Mix_Chunk* effect, const char* fileName);
    };
    // Members
    // Music
    Mix_Music* music;
    bool pausedMusic;
    int soundCount;

    // Vector of sound effects
    vector<SoundEffect> soundEffects;

    // Helper Method
    void StartMusic(int loop, bool overridePause, int milliseconds, bool fade);
    int StartSound(int soundIndex, int loop, int milliseconds, bool fade);
    bool ValidSoundIndex(int soundIndex) const;
    

    // Singleton
    SDLAudio();
    SDLAudio(const SDLAudio& rhs);
    SDLAudio& operator=(const SDLAudio& rhs);
public:

    // Friend function for Sound Effect Unhooking
    friend void ChannelFinished(int channel);

    // Destructor
    ~SDLAudio();

    // Instance function
    static SDLAudio* Instance();

    // Initializes all audio
    void Init();

    // Cleans up any dynamic memory allocated for audio
    void Shutdown();

    // Load music
    void LoadMusic(const char* fileName);

    // Change music
    void ChangeMusic(const char* fileName, int loop = -1);

    // Unload music
    void UnloadMusic();

    // Play music (0 means play 0 times, -1 forever)
    void PlayMusic(int loop = -1, bool overridePause = false);

    // Stop music
    void StopMusic();

    // Pause music
    void PauseMusic();

    // Resume (Left out for now, maybe add later)

    // Fade out music (if you fade while paused, the fade will not start until the song resumes)
    void FadeOutMusic(int milliseconds);

    // Fade in music
    void FadeInMusic(int milliseconds, int loop = -1, bool overridePause = false);
    
    // Get the volume
    int MusicVolume();

    // 0 = mute, 128 = MIX_MAX_VOLUME = max
    void MusicVolume(int volume);

    // Mute music
    void MuteMusic();

    // Load sound
    int LoadSound(const char* fileName);

    // Unload sound
    void UnloadSound(int soundIndex);

    // Play sound
    int PlaySoundEffect(int soundIndex, int loop = 0);

    // Stop sound
    void StopSoundEffect(int soundIndex, int channel);

    // Fade in sound
    int FadeInSoundEffect(int soundIndex, int milliseconds, int loop = 0);

    // Fade out sound
    void FadeOutSoundEffect(int soundIndex, int channel, int milliseconds);

    // Pause sound
    void PauseSoundEffect(int soundIndex, int channel);

    // Mute sound
    void MuteSoundEffects();

    // Set the sound effects volume
    void SoundEffectsVolume(int newVolume);

    // Get the sound effects volume
    int SoundEffectsVolume();

    // Mute sound & music
    void Mute();

};

#endif // SDLAUDIO_H_