#include <stdio.h>
#include <stdlib.h>

#include "fb2png.h"

#ifdef ANDROID
    #define DEFAULT_SAVE_PATH "/data/local/fbdump.png"
#else
    #define DEFAULT_SAVE_PATH "fbdump.png"
#endif

int main(int argc, char *argv[])
{
    char fn[128];

    if (argc == 2) {
        //if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
        if (argv[1][0] == '-') {
            printf(
                "Usage: fb2png [path/to/output.png]\n"
                "    The default output path is /data/local/fbdump.png\n"
                );
            exit(0);
        } else {
            sprintf(fn, "%s", argv[1]);
        }
    } else {
        sprintf(fn, "%s", DEFAULT_SAVE_PATH);
    }

    return fb2png(fn);
}
