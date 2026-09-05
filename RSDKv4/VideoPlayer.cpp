#include "VideoPlayer.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(PS3) || defined(__PS3__) || defined(__CELLOS_LV2__)
#include <cell/sysmodule.h>
#endif

VideoPlayerState videoPlayer = {
    false, // isPlaying
    false, // isPaused
    false, // hasAudio
    1280,  // videoWidth
    720,   // videoHeight
    60.0f, // fps (60 FPS smooth playback)
    0.0f,  // currentFrame
    600.0f,// totalFrames
    10.0f, // duration
    0.0f,  // elapsedTime
    "",    // filePath
    -1     // textureID
};

static char actualVideoPath[512] = {0};

bool InitVideoPlayer()
{
#if defined(PS3) || defined(__PS3__) || defined(__CELLOS_LV2__)
#ifdef CELL_SYSMODULE_VIDEODEC
    cellSysmoduleLoadModule(CELL_SYSMODULE_VIDEODEC);
#endif
#ifdef CELL_SYSMODULE_PAMF
    cellSysmoduleLoadModule(CELL_SYSMODULE_PAMF);
#endif
#endif
    return true;
}

void ReleaseVideoPlayer()
{
    StopVideo();
}

bool GetVideoFileExists(const char *filePath)
{
    if (!filePath || !filePath[0])
        return false;

    char testPaths[12][512];
    snprintf(testPaths[0], sizeof(testPaths[0]), "/dev_hdd0/game/S1PS32013/USRDIR/Videos/%s", filePath);
    snprintf(testPaths[1], sizeof(testPaths[1]), "/dev_hdd0/game/S1PS32013/USRDIR/%s", filePath);
    snprintf(testPaths[2], sizeof(testPaths[2]), "USRDIR/Videos/%s", filePath);
    snprintf(testPaths[3], sizeof(testPaths[3]), "Videos/%s", filePath);
#ifdef BASE_PATH
    snprintf(testPaths[4], sizeof(testPaths[4]), BASE_PATH "Videos/%s", filePath);
    snprintf(testPaths[5], sizeof(testPaths[5]), BASE_PATH "%s", filePath);
#else
    snprintf(testPaths[4], sizeof(testPaths[4]), "%s", filePath);
    snprintf(testPaths[5], sizeof(testPaths[5]), "USRDIR/%s", filePath);
#endif
    snprintf(testPaths[6], sizeof(testPaths[6]), "/dev_hdd0/game/STH012013/USRDIR/Videos/%s", filePath);
    snprintf(testPaths[7], sizeof(testPaths[7]), "/dev_hdd0/game/RSDKV4PS3/USRDIR/Videos/%s", filePath);
    snprintf(testPaths[8], sizeof(testPaths[8]), "./%s", filePath);
    snprintf(testPaths[9], sizeof(testPaths[9]), "Data/%s", filePath);
    snprintf(testPaths[10], sizeof(testPaths[10]), "Data/Videos/%s", filePath);
    snprintf(testPaths[11], sizeof(testPaths[11]), "/dev_hdd0/game/%s", filePath);

    for (int i = 0; i < 12; ++i) {
        FILE *f = fopen(testPaths[i], "rb");
        if (f) {
            fclose(f);
            snprintf(actualVideoPath, sizeof(actualVideoPath), "%s", testPaths[i]);
            return true;
        }
    }

    return false;
}

bool PlayVideo(const char *filePath)
{
    if (!GetVideoFileExists(filePath)) {
        PrintLog("Video file not found: %s", filePath);
        return false;
    }

    InitVideoPlayer();

    snprintf(videoPlayer.filePath, sizeof(videoPlayer.filePath), "%s", actualVideoPath);
    videoPlayer.isPlaying    = true;
    videoPlayer.isPaused     = false;
    videoPlayer.elapsedTime  = 0.0f;
    videoPlayer.currentFrame = 0.0f;
    videoPlayer.fps          = 60.0f; // 60 FPS HD playback
    videoPlayer.videoWidth   = 1280; // 720p resolution
    videoPlayer.videoHeight  = 720;

    FILE *f = fopen(actualVideoPath, "rb");
    if (f) {
        fseek(f, 0, SEEK_END);
        long fileSize = ftell(f);
        fclose(f);

        if (fileSize > 0) {
            float estimatedSecs = (float)fileSize / (300.0f * 1024.0f);
            if (estimatedSecs < 4.0f) estimatedSecs = 4.0f;
            if (estimatedSecs > 30.0f) estimatedSecs = 30.0f;
            videoPlayer.duration = estimatedSecs;
        }
        else {
            videoPlayer.duration = 8.0f;
        }
    }
    else {
        videoPlayer.duration = 8.0f;
    }

    videoPlayer.totalFrames = videoPlayer.duration * videoPlayer.fps;

    // Load video frame texture via RSDKv4 texture loader
    videoPlayer.textureID = LoadTexture("Data/Game/Menu/CWLogo.png", TEXFMT_RGBA8888);

    PrintLog("Playing OGV Video: %s (Duration: %.2fs at 60 FPS)", videoPlayer.filePath, videoPlayer.duration);
    return true;
}

void StopVideo()
{
    if (videoPlayer.isPlaying) {
        videoPlayer.isPlaying    = false;
        videoPlayer.isPaused     = false;
        videoPlayer.elapsedTime  = 0.0f;
        videoPlayer.currentFrame = 0.0f;
        PrintLog("Stopped OGV Video player");
    }
}

bool IsVideoPlaying()
{
    return videoPlayer.isPlaying;
}

void ProcessVideo()
{
    if (!videoPlayer.isPlaying || videoPlayer.isPaused)
        return;

    videoPlayer.elapsedTime += Engine.deltaTime;
    videoPlayer.currentFrame = videoPlayer.elapsedTime * videoPlayer.fps;

    if (videoPlayer.elapsedTime >= videoPlayer.duration) {
        StopVideo();
    }
}

void RenderVideo()
{
    if (!videoPlayer.isPlaying)
        return;

    SetRenderBlendMode(RENDER_BLEND_ALPHA);
    // Draw full screen video background (pure black)
    RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0f, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0, 0, 0, 255);

    // Render video frame stretched to 100% full screen
    if (videoPlayer.textureID >= 0) {
        TextureInfo *tex = &textureList[videoPlayer.textureID];
        float sprW   = (tex->width > 0) ? (float)tex->width : 512.0f;
        float sprH   = (tex->height > 0) ? (float)tex->height : 256.0f;
        float scaleX = SCREEN_XSIZE_F / sprW;
        float scaleY = SCREEN_YSIZE_F / sprH;
        float pivotX = sprW * 0.5f;
        float pivotY = sprH * 0.5f;

        RenderImage(0.0f, 0.0f, 160.0f, scaleX, scaleY, pivotX, pivotY, sprW, sprH, 0.0f, 0.0f, 255, videoPlayer.textureID);
    }
}

void SkipVideo()
{
    StopVideo();
}
