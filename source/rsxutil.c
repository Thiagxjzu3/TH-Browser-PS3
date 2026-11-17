
#include <ppu-lv2.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>

#include <sysutil/video.h>
#include <rsx/gcm_sys.h>
#include <rsx/rsx.h>

#include "rsxutil.h"

#define GCM_LABEL_INDEX 255

void waitFlip(void)
{
    while (gcmGetFlipStatus() != 0)
        usleep(200);

    gcmResetFlipStatus();
}

int flip(gcmContextData *context, s32 buffer)
{
    if (gcmSetFlip(context, buffer) == 0)
    {
        rsxFlushBuffer(context);
        gcmSetWaitFlip(context);
        return 1;
    }

    return 0;
}

int makeBuffer(rsxBuffer *buffer, u16 width, u16 height, int id)
{
    int pitch = sizeof(u32) * width;
    int size = pitch * height;

    buffer->ptr = (uint32_t *)rsxMemalign(64, size);

    if (buffer->ptr == NULL)
        return 0;

    if (rsxAddressToOffset(buffer->ptr, &buffer->offset) != 0)
    {
        rsxFree(buffer->ptr);
        buffer->ptr = NULL;
        return 0;
    }

    if (gcmSetDisplayBuffer(id, buffer->offset, pitch,
                            width, height) != 0)
    {
        rsxFree(buffer->ptr);
        buffer->ptr = NULL;
        return 0;
    }

    buffer->width = width;
    buffer->height = height;
    buffer->id = id;

    return 1;
}

int getResolution(u16 *width, u16 *height)
{
    videoState state;
    videoResolution resolution;

    if (videoGetState(0, 0, &state) != 0)
        return 0;

    if (videoGetResolution(state.displayMode.resolution,
                           &resolution) != 0)
        return 0;

    if (width)
        *width = resolution.width;

    if (height)
        *height = resolution.height;

    return 1;
}

static void waitRSXIdle(gcmContextData *context)
{
    u32 label = 1;

    rsxSetWriteBackendLabel(context, GCM_LABEL_INDEX, label);
    rsxSetWaitLabel(context, GCM_LABEL_INDEX, label);

    label++;

    rsxSetWriteBackendLabel(context, GCM_LABEL_INDEX, label);
    rsxFlushBuffer(context);

    while (*(vu32 *)gcmGetLabelAddress(GCM_LABEL_INDEX) != label)
        usleep(30);
}

gcmContextData *initScreen(void *host_addr, u32 size)
{
    gcmContextData *context = NULL;
    videoState state;
    videoConfiguration config;
    videoResolution resolution;

    rsxInit(&context, CB_SIZE, size, host_addr);

    if (context == NULL)
        return NULL;

    if (videoGetState(0, 0, &state) != 0)
        goto error;

    if (state.state != 0)
        goto error;

    if (videoGetResolution(state.displayMode.resolution,
                           &resolution) != 0)
        goto error;

    memset(&config, 0, sizeof(config));

    config.resolution = state.displayMode.resolution;
    config.format = VIDEO_BUFFER_FORMAT_XRGB;
    config.pitch = resolution.width * sizeof(u32);
    config.aspect = state.displayMode.aspect;

    waitRSXIdle(context);

    if (videoConfigure(0, &config, NULL, 0) != 0)
        goto error;

    gcmSetFlipMode(GCM_FLIP_VSYNC);
    gcmResetFlipStatus();

    return context;

error:
    rsxFinish(context, 0);
    return NULL;
}
