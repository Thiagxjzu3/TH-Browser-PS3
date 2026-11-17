#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <io/pad.h>
#include <sysutil/sysutil.h>
#include <rsx/rsx.h>
#include <video/video.h>

#include "interface.h"
#include "controller.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    THB_InitController();
    THB_InitInterface();

    int running = 1;

    while (running)
    {
        THB_UpdateController(&running);
        THB_RenderInterface();

        sysUtilCheckCallback();
        rsxFlushBuffer(NULL);
    }

    THB_ShutdownInterface();
    THB_ShutdownController();

    return 0;
}