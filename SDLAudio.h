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
        int currentChannel;

        SoundEffect(Mix_Chunk* effect, const char* fileName);
    };
    // Members
    // Music
    Mix_Music* music;
    bool pausedMusic;

    // Vector of sound effects
    vector<SoundEffect> soundEffects;

    // Helper Method
    void StartMusic(int loop, bool overridePause, int milliseconds, bool fade);
    bool ValidSoundIndex(int soundIndex) const;

    // Singleton
    SDLAudio();
    SDLAudio(const SDLAudio& rhs);
    SDLAudio& operator=(const SDLAudio& rhs);
public:
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
    void PlaySoundEffect(int soundIndex, int loop = 0);


    // Stop sound
    // Fade in sound
    // Fade out sound
    // Pause sound
    // Mute sound
    // Mute sound & music

};

#endif // SDLAUDIO_H_