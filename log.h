/*
 *   Author: Kyan He <kyan.ql.he@gmail.com>
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
#ifndef __KYAN_LOG_H__
#define __KYAN_LOG_H__


#ifdef ANDROID

#ifndef LOG_TAG
#define LOG_TAG "im.kyan"
#endif

#include <utils/Log.h>

#define D LOGD
#define E LOGE

#else

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#if DEBUG == 1

#define LOG_FUNCTION_NAME \
            fprintf(stderr, "\033[0;1;31m__func__: %s\033[0;0m\n", __FUNCTION__);

#else

#define LOG_FUNCTION_NAME

#endif

static void
D(const char *msg, ...)
{
    va_list ap;

    va_start (ap, msg);
    vfprintf(stdout, msg, ap);
    fprintf(stdout, "\n");
    va_end (ap);
    fflush(stdout);
}

static void
E(const char *msg, ...)
{
    va_list ap;

    va_start (ap, msg);
    vfprintf(stderr, msg, ap);
    fprintf(stderr, "\n");
    va_end (ap);

    exit(EXIT_FAILURE);
}

#endif /* ANDROID */

#endif
