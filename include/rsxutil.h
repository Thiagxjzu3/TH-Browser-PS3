#ifndef THB_RSXUTIL_H
#define THB_RSXUTIL_H

#include <rsx/rsx.h>
#include <ppu-types.h>

#define CB_SIZE  0x100000
#define HOST_SIZE (32 * 1024 * 1024)

typedef struct
{
    int height;
    int width;
    int id;
    uint32_t *ptr;
    uint32_t offset;
} rsxBuffer;

void waitFlip(void);
int flip(gcmContextData *context, s32 buffer);
int makeBuffer(rsxBuffer *buffer, u16 width, u16 height, int id);
int getResolution(u16 *width, u16 *height);
gcmContextData *initScreen(void *host_addr, u32 size);

#endif