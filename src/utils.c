#include "utils.h"
#include "globals.h"

#ifdef __APPLE__
#include <readpassphrase.h>
#else
#include <bsd/readpassphrase.h>
#endif

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

u64
ft_strlen(const char* str) {
    u64 len = 0;
    while (str[len]) len++;
    return len;
}

i64
ft_strcmp(const char* s1, const char* s2) {
    while (*s1 == *s2) {
        if (*s1 == 0) return (0);
        ++s1;
        ++s2;
    }
    return (*s1 - *s2);
}

void
ft_memcpy(Buffer dst, Buffer src) {
    assert(dst.len == src.len);

    u64 len = dst.len;
    while (len) {
        *dst.ptr = *src.ptr;
        dst.ptr++;
        src.ptr++;
        len--;
    }
}

void
ft_memset(Buffer dst, u8 value) {
    u64 len = dst.len;
    while (len) {
        *dst.ptr = value;
        dst.ptr++;
        len--;
    }
}

bool
ft_memcmp(Buffer a, Buffer b) {
    if (a.len != b.len) return false;
    for (u64 i = 0; i < a.len; i++) {
        if (a.ptr[i] != b.ptr[i]) return false;
    }
    return true;
}

char
ft_lower(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A' + 'a';
    return c;
}

u64
ft_hextol(const char* value) {
    u64 i = 0;
    if (ft_memcmp(str("0x"), buf((u8*)value, 2)) || ft_memcmp(str("0X"), buf((u8*)value, 2))) {
        i += 2;
    }

    u64 len = ft_strlen(value);
    u64 out = 0;
    while (i < len) {
        char c = ft_lower(value[i]);

        if (c >= '0' && c <= '9')
            c -= '0';
        else if (c >= 'a' && c <= 'f')
            c = (c - 'a') + 10;
        else
            return out;

        out *= 16;
        out += c;

        i++;
    }

    return out;
}

u32
rotate_left32(u32 value, u32 shift) {
    assert(shift < 32);
    return (value << shift) | (value >> (32 - shift));
}

u32
rotate_right32(u32 value, u32 shift) {
    assert(shift < 32);
    return (value >> shift) | (value << (32 - shift));
}

u64
rotate_left64(u64 value, u64 shift) {
    assert(shift < 64);
    return (value << shift) | (value >> (64 - shift));
}

u64
rotate_right64(u64 value, u64 shift) {
    assert(shift < 64);
    return (value >> shift) | (value << (64 - shift));
}

u32
byte_swap32(u32 value) {
    u32 a = (value << 24) & 0xFF000000u;
    u32 b = (value << 8) & 0x00FF0000u;
    u32 c = (value >> 8) & 0x0000FF00u;
    u32 d = (value >> 24) & 0x000000FFu;
    return a | b | c | d;
}

u64
byte_swap64(u64 value) {
    u64 a = (value << 56) & 0xFF00000000000000ull;
    u64 b = (value << 40) & 0x00FF000000000000ull;
    u64 c = (value << 24) & 0x0000FF0000000000ull;
    u64 d = (value << 8) & 0x000000FF00000000ull;
    u64 e = (value >> 8) & 0x00000000FF000000ull;
    u64 f = (value >> 24) & 0x0000000000FF0000ull;
    u64 g = (value >> 40) & 0x000000000000FF00ull;
    u64 h = (value >> 56) & 0x00000000000000FFull;
    return a | b | c | d | e | f | g | h;
}

Buffer
str(const char* s) {
    return (Buffer){ .ptr = (u8*)s, .len = ft_strlen(s) };
}

Buffer
buf(u8* ptr, u64 len) {
    return (Buffer){ .ptr = ptr, .len = len };
}

Buffer
read_all_fd(int fd) {
    u64 capacity = 2048;
    Buffer str = { .ptr = malloc(capacity + 1), .len = 0 };
    if (!str.ptr) return (Buffer){ 0 };

    u8 buffer[2048];
    i64 bytes = sizeof(buffer);
    while (bytes > 0) {
        bytes = read(fd, buffer, sizeof(buffer));
        if (bytes < 0) return (Buffer){ 0 };

        u64 remaining = capacity - str.len;

        if ((u64)bytes > remaining) {
            u64 rest = bytes - remaining;
            capacity = (capacity * 2 > rest) ? capacity * 2 : rest;

            u8* ptr = malloc(capacity + 1);
            if (!ptr) return (Buffer){ 0 };

            ft_memcpy(buf(ptr, str.len), str);
            free(str.ptr);
            str.ptr = ptr;
        }

        ft_memcpy(buf(str.ptr + str.len, bytes), buf(buffer, bytes));
        str.len += bytes;
    }

    str.ptr[str.len] = 0;

    return str;
}

u32
read_u32(u8* buffer) {
    u32 out = 0;

    out |= (u32)buffer[0];
    out |= (u32)buffer[1] << 8;
    out |= (u32)buffer[2] << 16;
    out |= (u32)buffer[3] << 24;

    return out;
}

u64
read_u64(u8* buffer) {
    u64 out = 0;

    out |= (u64)buffer[0];
    out |= (u64)buffer[1] << 8;
    out |= (u64)buffer[2] << 16;
    out |= (u64)buffer[3] << 24;
    out |= (u64)buffer[4] << 32;
    out |= (u64)buffer[5] << 40;
    out |= (u64)buffer[6] << 48;
    out |= (u64)buffer[7] << 56;

    return out;
}

u32
read_u32_be(u8* buffer) {
    u32 out = 0;

    out |= (u32)buffer[3];
    out |= (u32)buffer[2] << 8;
    out |= (u32)buffer[1] << 16;
    out |= (u32)buffer[0] << 24;

    return out;
}

u64
read_u48_be(u8* buffer) {
    u64 out = 0;

    out |= (u64)buffer[5];
    out |= (u64)buffer[4] << 8;
    out |= (u64)buffer[3] << 16;
    out |= (u64)buffer[2] << 24;
    out |= (u64)buffer[1] << 32;
    out |= (u64)buffer[0] << 40;

    return out;
}

u64
read_u64_be(u8* buffer) {
    u64 out = 0;

    out |= (u64)buffer[7];
    out |= (u64)buffer[6] << 8;
    out |= (u64)buffer[5] << 16;
    out |= (u64)buffer[4] << 24;
    out |= (u64)buffer[3] << 32;
    out |= (u64)buffer[2] << 40;
    out |= (u64)buffer[1] << 48;
    out |= (u64)buffer[0] << 56;

    return out;
}

u32
read_u24_be(u8* buffer) {
    u32 out = 0;

    out |= (u32)buffer[2];
    out |= (u32)buffer[1] << 8;
    out |= (u32)buffer[0] << 16;

    return out;
}

u32
read_u16_be(u8* buffer) {
    u32 out = 0;

    out |= (u32)buffer[1];
    out |= (u32)buffer[0] << 8;

    return out;
}

void
print_error(void) {
    dprintf(STDERR_FILENO, "%s: %s\n", progname, strerror(errno));
}

bool
is_space(u8 c) {
    return (c == '\n' || c == '\v' || c == '\t' || c == '\r' || c == '\f' | c == ' ');
}

static u8
from_hex(u8 c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0xFF;
}

void
parse_hex(Buffer str, Buffer out, u32* err) {
    u64 len = str.len < out.len * 2 ? str.len : out.len * 2;
    ft_memset(out, 0);

    for (u64 i = 0; i < len; i += 2) {
        u8 hi = from_hex(str.ptr[i]);
        u8 lo = from_hex(i + 1 < len ? str.ptr[i + 1] : '0');

        if (hi == 0xFF || lo == 0xFF) {
            *err = 1;
            return;
        }

        out.ptr[i / 2] = (hi << 4) | lo;
    }
}

void
print_hex(Buffer str) {
    for (u64 i = 0; i < str.len; i++) {
        printf("%02X", str.ptr[i]);
    }
    printf("\n");
}

bool
get_random_bytes(Buffer buffer) {
    int fd = open("/dev/random", O_RDONLY);
    if (fd < 0) return false;

    ssize_t bytes = read(fd, buffer.ptr, buffer.len);
    if (bytes < 0) {
        close(fd);
        return false;
    }

    return true;
}

bool
read_password(Buffer buffer) {
    char verify[MAX_PASSWORD_SIZE] = { 0 };

    const char* pass_ptr =
        readpassphrase("enter password: ", (char*)buffer.ptr, sizeof(buffer.len), 0);
    const char* verify_ptr = readpassphrase("reenter password: ", verify, sizeof(verify), 0);

    if (!pass_ptr || !verify_ptr) {
        dprintf(STDERR_FILENO, "%s: error reading password\n", progname);
        return false;
    }

    if (!ft_memcmp(str(pass_ptr), str(verify_ptr))) {
        dprintf(STDERR_FILENO, "%s: passwords don't match\n", progname);
        return false;
    }

    return true;
}

int
get_infile_fd(const char* filename) {
    int fd = -1;
    if (filename) {
        fd = open(filename, O_RDONLY);
    } else {
        fd = STDIN_FILENO;
    }

    return fd;
}

int
get_outfile_fd(const char* filename) {
    int fd = -1;
    if (filename) {
        fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRGRP | S_IROTH);
    } else {
        fd = STDOUT_FILENO;
    }

    return fd;
}
