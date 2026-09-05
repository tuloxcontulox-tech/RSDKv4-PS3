#ifndef VIDEO_PLAYER_HPP
#define VIDEO_PLAYER_HPP

#include "RetroEngine.hpp"

struct VideoPlayerState {
    bool isPlaying;
    bool isPaused;
    bool hasAudio;
    int videoWidth;
    int videoHeight;
    float fps;
    float currentFrame;
    float totalFrames;
    float duration;
    float elapsedTime;
    char filePath[256];
    int textureID;
};

extern VideoPlayerState videoPlayer;

bool InitVideoPlayer();
void ReleaseVideoPlayer();

bool GetVideoFileExists(const char *filePath);
bool PlayVideo(const char *filePath);
void StopVideo();
bool IsVideoPlaying();
void ProcessVideo();
void RenderVideo();
void SkipVideo();

#endif // VIDEO_PLAYER_HPP
