/*
 * Copyright (C) 2016-2018 Jolla Ltd.
 * Contact: Slava Monich <slava.monich@jolla.com>
 *
 * You may use this file under the terms of BSD license as follows:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. Neither the name of Jolla Ltd nor the names of its contributors may
 *      be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "gutil_misc.h"

#include <glib-object.h>

#include <ctype.h>

void
gutil_disconnect_handlers(
    gpointer instance,
    gulong *ids,
    int count)
{
    if (G_LIKELY(instance) && G_LIKELY(ids)) {
        int i;
        for (i=0; i<count; i++) {
            if (ids[i]) {
                g_signal_handler_disconnect(instance, ids[i]);
                ids[i] = 0;
            }
        }
    }
}

void*
gutil_hex2bin(
    const char* str,
    gssize len,
    void* data)
{
    if (str && data && len > 0 && !(len & 1)) {
        gssize i;
        guint8* ptr = data;
        for (i=0; i<len; i+=2) {
            static const guint8 hex[] = {
                0, 1, 2, 3, 4, 5, 6, 7,     /* 0x30..0x37 */
                8, 9, 0, 0, 0, 0, 0, 0,     /* 0x3a..0x3f */
                0,10,11,12,13,14,15, 0,     /* 0x40..0x47 */
                0, 0, 0, 0, 0, 0, 0, 0,     /* 0x4a..0x4f */
                0, 0, 0, 0, 0, 0, 0, 0,     /* 0x40..0x47 */
                0, 0, 0, 0, 0, 0, 0, 0,     /* 0x5a..0x5f */
                0,10,11,12,13,14,15         /* 0x60..0x66 */
            };
            const char x1 = str[i];
            const char x2 = str[i+1];
            if (isxdigit(x1) && isxdigit(x2)) {
                *ptr++ = (hex[x1-0x30] << 4) + hex[x2-0x30];
            } else {
                return NULL;
            }
        }
        return data;
    }
    return NULL;
}

GBytes*
gutil_hex2bytes(
    const char* str,
    gssize len)
{
    if (str) {
        if (len < 0) len = strlen(str);
        if (len > 0 && !(len & 1)) {
            void* data = g_malloc(len/2);
            if (gutil_hex2bin(str, len, data)) {
                return g_bytes_new_take(data, len/2);
            }
            g_free(data);
        }
    }
    return NULL;
}

/**
 * The caller makes sure that the destination buffer has at least
 * GUTIL_HEXDUMP_BUFSIZE bytes available. Returns the number of
 * bytes actually dumped (no more than GUTIL_HEXDUMP_MAXBYTES).
 *
 * Since 1.0.29
 */
guint
gutil_hexdump(
    char* buf,
    const void* data,
    guint len)
{
    static const char hex[] = "0123456789abcdef";
    const guint bytes_dumped = MIN(len, GUTIL_HEXDUMP_MAXBYTES);
    const guchar* bytes = data;
    char* ptr = buf;
    guint i;

    for (i=0; i<GUTIL_HEXDUMP_MAXBYTES; i++) {
        if (i > 0) {
            *ptr++ = ' ';
            if (i == 8) *ptr++ = ' ';
        }
        if (i < len) {
            const guchar b = bytes[i];
            *ptr++ = hex[(b >> 4) & 0xf];
            *ptr++ = hex[b & 0xf];
        } else {
            *ptr++ = ' ';
            *ptr++ = ' ';
        }
    }

    *ptr++ = ' ';
    *ptr++ = ' ';
    *ptr++ = ' ';
    *ptr++ = ' ';
    for (i=0; i<bytes_dumped; i++) {
        const char c = bytes[i];
        if (i == 8) *ptr++ = ' ';
        *ptr++ = isprint(c) ? c : '.';
    }

    *ptr++ = 0;
    return bytes_dumped;
}

/*
 * Local Variables:
 * mode: C
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
