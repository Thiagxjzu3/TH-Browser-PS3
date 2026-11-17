
#include <ppu-lv2.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>

#include <io/pad.h>
#include "rsxutil.h"

#define MAX_BUFFERS 2

static void drawFrame(rsxBuffer *buffer)
{
    int x, y;
    const u32 background = 0x00101827;

    for (y = 0; y < buffer->height; y++)
    {
        for (x = 0; x < buffer->width; x++)
        {
            buffer->ptr[y * buffer->width + x] = background;
        }
    }
}

int main(void)
{
    gcmContextData *context = NULL;
    void *host_addr = NULL;

    rsxBuffer buffers[MAX_BUFFERS] = {0};

    padInfo padinfo;
    padData paddata;

    u16 width;
    u16 height;

    int currentBuffer = 0;
    int i;

    host_addr = memalign(1024 * 1024, HOST_SIZE);

    if (host_addr == NULL)
        return 1;

    context = initScreen(host_addr, HOST_SIZE);

    if (context == NULL)
    {
        free(host_addr);
        return 1;
    }

    ioPadInit(7);

    if (!getResolution(&width, &height))
        goto cleanup;

    for (i = 0; i < MAX_BUFFERS; i++)
    {
        if (!makeBuffer(&buffers[i], width, height, i))
            goto cleanup;
    }

    if (!flip(context, MAX_BUFFERS - 1))
        goto cleanup;

    while (1)
    {
        ioPadGetInfo(&padinfo);

        for (i = 0; i < MAX_PADS; i++)
        {
            if (padinfo.status[i])
            {
                ioPadGetData(i, &paddata);

                if (paddata.BTN_START)
                    goto cleanup;
            }
        }

        waitFlip();

        drawFrame(&buffers[currentBuffer]);

        if (!flip(context, buffers[currentBuffer].id))
            goto cleanup;

        currentBuffer++;

        if (currentBuffer >= MAX_BUFFERS)
            currentBuffer = 0;
    }

cleanup:

    gcmSetWaitFlip(context);

    for (i = 0; i < MAX_BUFFERS; i++)
    {
        if (buffers[i].ptr != NULL)
            rsxFree(buffers[i].ptr);
    }

    rsxFinish(context, 1);
    free(host_addr);
    ioPadEnd();

    return 0;
}
