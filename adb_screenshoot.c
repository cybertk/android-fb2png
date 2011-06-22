/*
 *   -- http://android-fb2png.googlecode.com/svn/trunk/adb_screenshoot.c --
 *
 *   Copyright 2011, Kyan He <kyan.ql.he@gmail.com>
 *
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <linux/fb.h>

#include "fb2png.h"
#include "log.h"

#define DEFAULT_SAVE_PATH "fbdump.png"

/* defined in $T/system/core/adb/framebuffer_service.c */
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

static int remote_socket(const char *host, int port)
{
    struct sockaddr_in sa;
    struct hostent *hp;
    int s;

    if(!(hp = gethostbyname(host))){ return -1; }

    memset(&sa, 0, sizeof(sa));
    sa.sin_port = htons(port);
    sa.sin_family = hp->h_addrtype;
    memcpy((void*) &sa.sin_addr, (void*) hp->h_addr, hp->h_length);

    if((s = socket(hp->h_addrtype, SOCK_STREAM, 0)) < 0) {
        return -1;
    }

    if(connect(s, (struct sockaddr*) &sa, sizeof(sa)) != 0){
        close(s);
        return -1;
    }

    return s;
}

char *target = "usb";
static int adb_fd;

static int adb_write(const char *cmd)
{
    char buf[1024];
    int sz;

    /* Construct command. */
    sz = sprintf(buf, "%04x%s", strlen(cmd), cmd);

    write(adb_fd, buf, sz);

    D("<< %s", buf);
    return sz;
}

static int adb_read(char *buf, int sz)
{
    sz = read(adb_fd, buf, sz);
    if (sz < 0) {
        E("Fail to read from adb socket, %s", strerror(errno));
    }
    buf[sz] = '\0';

    D(">> %s", buf);
    return sz;
}

static void get_framebuffer()
{
    char buf[1024];
    const struct fbinfo* fbinfo;
    struct fb_var_screeninfo vinfo;

    /* Init socket */
    adb_fd = remote_socket("localhost", 5037);
    if (adb_fd < 0) {
        E("Fail to create socket, %s", strerror(errno));
    }

    adb_write("host:transport-");
    adb_read(buf, 1024);

    adb_write("framebuffer:");
    adb_read(buf, 1024);

    /* Parse FB header. */
    adb_read(buf, sizeof(struct fbinfo));
    fbinfo = (struct fbinfo*) buf;
    {
        if (fbinfo->version != DDMS_RAWIMAGE_VERSION) {
            E("unspport adb version");
        }

        vinfo.bits_per_pixel = fbinfo->bpp;
        vinfo.xres = fbinfo->width;
        vinfo.yres = fbinfo->height;

        /* RGBA */
        vinfo.red.offset = fbinfo->red_offset;
        vinfo.red.length = fbinfo->red_length;
        vinfo.green.offset = fbinfo->green_offset;
        vinfo.green.length = fbinfo->green_length;
        vinfo.blue.offset = fbinfo->blue_offset;
        vinfo.blue.length = fbinfo->blue_length;
        vinfo.transp.offset = fbinfo->alpha_offset;
        vinfo.transp.length = fbinfo->alpha_length;
    }

    //dump_fb_var_screeninfo(&vinfo);

    char *fb_data = malloc(fbinfo->size);
    if (!fb_data) { E("ENOMEM"); }

    adb_read(fb_data, fbinfo->size);
    D("done");
}

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

    get_framebuffer();
    //return fb2png(fn);
}
