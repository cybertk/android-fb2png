#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>

#include "log.h"
#include "img_process.h"

#ifdef ARCH_x86
    #define DEFAULT_SAVE_PATH "fbdump.png"
#else
    #define DEFAULT_SAVE_PATH "/data/local/fbdump.png"
#endif

/* This version number defines the format of the fbinfo struct.
   It must match versioning in ddms where this data is consumed. */
#define DDMS_RAWIMAGE_VERSION 1
struct fbinfo {
    unsigned int version;
    unsigned int bpp;
    unsigned int size;
    unsigned int width;
    unsigned int height;
    unsigned int red_offset;
    unsigned int red_length;
    unsigned int blue_offset;
    unsigned int blue_length;
    unsigned int green_offset;
    unsigned int green_length;
    unsigned int alpha_offset;
    unsigned int alpha_length;
} __attribute__((packed));



int main(int argc, char *argv[])
{
    FILE *fp;
    struct fb_var_screeninfo vinfo;
    int fb, offset;
    struct fbinfo fbinfo;

    char *x;
    char *rgb_matrix;

    char fn[128];

    int i;

    if (argc == 2) {
        //if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
        if (argv[1][0] == '-') {
            printf("Usage: fb2png [path/to/png]\n");
            exit(0);
        } else {
            sprintf(fn, "%s", argv[1]);
        }
    } else {
        sprintf(fn, "%s", DEFAULT_SAVE_PATH);
    }

#ifdef ARCH_x86
    fb = open("/dev/fb0", O_RDONLY);
#else
    fb = open("/dev/graphics/fb0", O_RDONLY);
#endif
    if(fb < 0) {
        D("Cannnot open framebuffer\n");
        goto done;
    }

    if(ioctl(fb, FBIOGET_VSCREENINFO, &vinfo) < 0) goto done;

    fcntl(fb, F_SETFD, FD_CLOEXEC);

    int bytespp = vinfo.bits_per_pixel / 8;

    fbinfo.version = DDMS_RAWIMAGE_VERSION;
    fbinfo.bpp = vinfo.bits_per_pixel;
    fbinfo.size = vinfo.xres * vinfo.yres * bytespp;
    fbinfo.width = vinfo.xres;
    fbinfo.height = vinfo.yres;
    fbinfo.red_offset = vinfo.red.offset;
    fbinfo.red_length = vinfo.red.length;
    fbinfo.green_offset = vinfo.green.offset;
    fbinfo.green_length = vinfo.green.length;
    fbinfo.blue_offset = vinfo.blue.offset;
    fbinfo.blue_length = vinfo.blue.length;
    fbinfo.alpha_offset = vinfo.transp.offset;
    fbinfo.alpha_length = vinfo.transp.length;

    D("%12s : %d\n", "bpp", bytespp);
    D("%12s : %d\n", "width", fbinfo.width);
    D("%12s : %d\n", "height", fbinfo.height);
    D("%12s : %d\n", "pixel", fbinfo.size);
    D("%12s : %d\n", "r_offset", fbinfo.red_offset);
    D("%12s : %d\n", "g_offset", fbinfo.green_offset);
    D("%12s : %d\n", "b_offset", fbinfo.blue_offset);
    D("%12s : %d\n", "a_offset", fbinfo.alpha_offset);
    D("%12s : %d\n", "r_length", fbinfo.red_length);
    D("%12s : %d\n", "g_length", fbinfo.green_length);
    D("%12s : %d\n", "b_length", fbinfo.blue_length);
    D("%12s : %d\n", "a_length", fbinfo.alpha_length);

    /* HACK: for several of our 3d cores a specific alignment
     * is required so the start of the fb may not be an integer number of lines
     * from the base.  As a result we are storing the additional offset in
     * xoffset. This is not the correct usage for xoffset, it should be added
     * to each line, not just once at the beginning */
    offset = vinfo.xoffset * bytespp;
    offset += vinfo.xres * vinfo.yoffset * bytespp;

    lseek(fb, offset, SEEK_SET);

    x = malloc(fbinfo.width * fbinfo.height * bytespp);
    rgb_matrix = malloc(fbinfo.width * fbinfo.height * 3);

    if(!x || !rgb_matrix) goto done;

    if (read(fb, x, fbinfo.size) < 0) goto done;

    int ret;
    switch(bytespp) {
        case 4:
            /* most devices use argb8888 */
            ret = argb8888_to_rgb888(x, rgb_matrix, fbinfo.width * fbinfo.height);
            break;
        case 2:
            /* emulator use rgb565 */
            ret = rgb565_to_rgb888(x, rgb_matrix, fbinfo.width * fbinfo.height);
            break;
        default:
            E("Do not support bytespp %d\n", bytespp);
            goto done;
    }

    if (ret) { E("error process image"); goto done; }

    fp = fopen(fn, "w");
    if (!fp)
        E("Cannot open file %s for write\n", fn);

    if (save_png(fp, rgb_matrix, fbinfo.width, fbinfo.height))
        E("save_png");

    D("OKAY\n");

done:
    if(fb >= 0) close(fb);
    if(!fp) fclose(fp);

    if (!x) free(x);
    if (!rgb_matrix) free(rgb_matrix);

    return 0;
}
