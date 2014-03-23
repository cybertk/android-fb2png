/**
 * fb2png  Save screenshot into .png.
 *
 * Copyright (C) 2012  Kyan <kyan.ql.he@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <errno.h>

#include "log.h"
#include "fb2png.h"
#include "fb.h"

/**
 * Get the {@code struct fb} from device's framebuffer.
 * Return
 *      0 for success.
 */
int get_device_fb(const char* path, struct fb *fb)
{
    int offset;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    unsigned char *raw;
    unsigned int bytespp;
    unsigned int raw_size;
    unsigned int raw_line_length;
    ssize_t read_size;
    int fd;

    fd = open(path, O_RDONLY);
    if (fd < 0) return -1;

    if(ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
        D("ioctl failed, %s", strerror(errno));
        close(fd);
        return -1;
    }

    if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo) < 0) {
        D("ioctl fixed failed, %s", strerror(errno));
        close(fd);
        return -1;
    }

    bytespp = vinfo.bits_per_pixel / 8;
    raw_line_length = finfo.line_length; // (xres + padding_offset) * bytespp
    raw_size = vinfo.yres * raw_line_length;

    // output data handler struct
    fb->bpp = vinfo.bits_per_pixel;
    fb->size = vinfo.xres * vinfo.yres * bytespp;
    fb->width = vinfo.xres;
    fb->height = vinfo.yres;
    fb->red_offset = vinfo.red.offset;
    fb->red_length = vinfo.red.length;
    fb->green_offset = vinfo.green.offset;
    fb->green_length = vinfo.green.length;
    fb->blue_offset = vinfo.blue.offset;
    fb->blue_length = vinfo.blue.length;
    fb->alpha_offset = vinfo.transp.offset;
    fb->alpha_length = vinfo.transp.length;

    // container for raw bits from the active frame buffer
    raw = malloc(raw_size);
    if (!raw) {
        D("raw: memory error");
        close(fd);
        return -1;
    }

#ifdef ANDROID
    /* HACK: for several of 3d cores a specific alignment
     * is required so the start of the fb may not be an integer number of lines
     * from the base.  As a result we are storing the additional offset in
     * xoffset. This is not the correct usage for xoffset, it should be added
     * to each line, not just once at the beginning */

    offset = vinfo.xoffset * bytespp;

    /* Android use double-buffer, capture 2nd */
    offset += vinfo.xres * vinfo.yoffset * bytespp;
#else
    offset = 0;
#endif


    // display debug fb info
    fb_dump(fb);
    D("%13s : %u", "bytespp", bytespp);
    D("%13s : %u", "raw size", raw_size);
    D("%13s : %u", "yoffset", vinfo.yoffset);
    D("%13s : %u", "pad offset", (raw_line_length / bytespp) - fb->width);

    lseek(fd, offset, SEEK_SET);
    read_size = read(fd, raw, raw_size);
    if (read_size < 0 || (unsigned)read_size != raw_size) {
        D("read buffer error");
        goto oops;
    }

/*
    Image padding (needed on some RGBX_8888 formats, maybe others?)
    we have padding_offset in bytes and bytespp = bits_per_pixel / 8
    raw_line_length = (width + padding_offset) * bytespp

    This gives: padding_offset = (raw_line_length / bytespp) - width
*/
    unsigned int padding_offset = (raw_line_length / bytespp) - fb->width;
    if (padding_offset) {
        unsigned char *data;
        unsigned char *pdata;
        unsigned char *praw;
        const unsigned char *data_buffer_end;
        const unsigned char *raw_buffer_end;

        // container for final aligned image data
        data = malloc(fb->size);
        if (!data) {
            D("data: memory error");
            goto oops;
        }

        pdata = data;
        praw = raw;
        data_buffer_end = data + fb->size;
        raw_buffer_end = raw + raw_size;

        // Add a margin to prevent buffer overflow during copy
        data_buffer_end -= bytespp * fb->width;
        raw_buffer_end -= raw_line_length;
        while (praw < raw_buffer_end && pdata < data_buffer_end) {
            memcpy(pdata, praw, bytespp * fb->width);
            pdata += bytespp * fb->width;
            praw += raw_line_length;
        }
        D("Padding done.");

        fb->data = data;
        free(raw);
    } else {
        fb->data = raw;
    }

    close(fd);
    return 0;

oops:
    free(raw);
    close(fd);
    return -1;
}

int fb2png(const char *path)
{
    struct fb fb;
    int ret;

#ifdef ANDROID
    ret = get_device_fb("/dev/graphics/fb0", &fb);
#else
    ret = get_device_fb("/dev/fb0", &fb);
#endif

    if (ret) {
        D("Failed to read framebuffer.");
        return -1;
    }

    return fb_save_png(&fb, path);
}
