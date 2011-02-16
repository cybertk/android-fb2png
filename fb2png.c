#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <errno.h>

#include "log.h"
#include "img_process.h"

#ifdef ANDROID
    #define DEFAULT_SAVE_PATH "/data/local/fbdump.png"
#else
    #define DEFAULT_SAVE_PATH "fbdump.png"
#endif

struct fb {
    unsigned int width;
    unsigned int height;
#define FB_FORMAT_RGB565    0
#define FB_FORMAT_ARGB8888  1
    unsigned int format;
    char* data;
};

void dump_fb_var_screeninfo(const struct fb_var_screeninfo* vinfo)
{
    D("%12s : %d\n", "bpp", vinfo->bits_per_pixel);
    D("%12s : %d\n", "width", vinfo->xres);
    D("%12s : %d\n", "height", vinfo->yres);
    D("%12s : %d\n", "vwidth", vinfo->xres_virtual);
    D("%12s : %d\n", "vheight", vinfo->yres_virtual);
    D("%12s : %d\n", "r_offset", vinfo->red.offset);
    D("%12s : %d\n", "g_offset", vinfo->green.offset);
    D("%12s : %d\n", "b_offset", vinfo->blue.offset);
    D("%12s : %d\n", "a_offset", vinfo->transp.offset);
    D("%12s : %d\n", "r_length", vinfo->red.length);
    D("%12s : %d\n", "g_length", vinfo->green.length);
    D("%12s : %d\n", "b_length", vinfo->blue.length);
    D("%12s : %d\n", "a_length", vinfo->transp.length);
}

int map_framebuffer(const char* path, struct fb* fb)
{
    int fd;
    int bytespp;
    int offset;
    struct fb_var_screeninfo vinfo;

    fd = open(path, O_RDONLY);
    if (fd < 0) return -1;

    if(ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
        D("ioctl failed, %s\n", strerror(errno));
        return -1;
    }

    bytespp = vinfo.bits_per_pixel / 8;
    fb->width = vinfo.xres;
    fb->height = vinfo.yres;

    dump_fb_var_screeninfo(&vinfo);

    /* TODO: determine image format according to fb_bitfield of 
     *       red/green/blue */
    if (bytespp == 2) {
        fb->format = FB_FORMAT_RGB565;
    } else {
        fb->format = FB_FORMAT_ARGB8888;
    }

#ifdef ANDROID
    /* HACK: for several of 3d cores a specific alignment
     * is required so the start of the fb may not be an integer number of lines
     * from the base.  As a result we are storing the additional offset in
     * xoffset. This is not the correct usage for xoffset, it should be added
     * to each line, not just once at the beginning */

    offset = vinfo.xoffset * bytespp;

    /* Android use double-buffer, capture 2nd */
    offset += vinfo.xres * vinfo.yoffset * bytespp * 2;
#else
    offset = 0;
#endif

    fb->data = mmap(0, vinfo.xres * vinfo.yoffset * bytespp,
                    PROT_READ, MAP_SHARED, fd, offset);
    if (!fb->data) return -1;

    close(fd);

    return 0;
}

int main(int argc, char *argv[])
{
    FILE *fp;
    struct fb fb;
    char *rgb_matrix;
    char fn[128];
    int i, ret;

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

#ifdef ANDROID
    ret = map_framebuffer("/dev/graphics/fb0", &fb);
#else
    ret = map_framebuffer("/dev/fb0", &fb);
#endif

    if(ret < 0) {
        D("Cannnot open framebuffer\n");
        goto done;
    }

    rgb_matrix = malloc(fb.width * fb.height * 3);
    if(!rgb_matrix) goto done;

    switch(fb.format) {
    case FB_FORMAT_RGB565:
        /* emulator use rgb565 */
        ret = rgb565_to_rgb888(fb.data, rgb_matrix, fb.width * fb.height);
        break;
    case FB_FORMAT_ARGB8888:
        /* most devices use argb8888 */
        ret = argb8888_to_rgb888(fb.data, rgb_matrix, fb.width * fb.height);
        break;
    dafault:
        D("treat framebuffer as rgb888\n");
    }

    if (ret) { E("error process image"); goto done; }

    fp = fopen(fn, "w");
    if (!fp)
        E("Cannot open file %s for write\n", fn);

    if (save_png(fp, rgb_matrix, fb.width, fb.height))
        E("save_png");

    D("OKAY\n");

    fclose(fp);

done:
    if (!rgb_matrix) free(rgb_matrix);

    return 0;
}
