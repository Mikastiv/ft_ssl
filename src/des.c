#include "cipher.h"
#include "types.h"
#include "utils.h"
#include <stdlib.h>

// Permuted choice 1
const static u8 pc1[] = {
    57, 49, 41, 33, 25, 17, 9,  1,  58, 50, 42, 34, 26, 18, 10, 2,  59, 51, 43,
    35, 27, 19, 11, 3,  60, 52, 44, 36, 63, 55, 47, 39, 31, 23, 15, 7,  62, 54,
    46, 38, 30, 22, 14, 6,  61, 53, 45, 37, 29, 21, 13, 5,  28, 20, 12, 4,
};

// Permuted choice 2
const static u8 pc2[] = {
    14, 17, 11, 24, 1,  5,  3,  28, 15, 6,  21, 10, 23, 19, 12, 4,  26, 8,  16, 7,  27, 20, 13, 2,
    41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48, 44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32,
};

// Initial permutation
const static u8 ip[] = {
    58, 50, 42, 34, 26, 18, 10, 2,  60, 52, 44, 36, 28, 20, 12, 4,  62, 54, 46, 38, 30, 22,
    14, 6,  64, 56, 48, 40, 32, 24, 16, 8,  57, 49, 41, 33, 25, 17, 9,  1,  59, 51, 43, 35,
    27, 19, 11, 3,  61, 53, 45, 37, 29, 21, 13, 5,  63, 55, 47, 39, 31, 23, 15, 7,
};

// Final permutation
const static u8 ip2[] = {
    40, 8,  48, 16, 56, 24, 64, 32, 39, 7,  47, 15, 55, 23, 63, 31, 38, 6,  46, 14, 54, 22,
    62, 30, 37, 5,  45, 13, 53, 21, 61, 29, 36, 4,  44, 12, 52, 20, 60, 28, 35, 3,  43, 11,
    51, 19, 59, 27, 34, 2,  42, 10, 50, 18, 58, 26, 33, 1,  41, 9,  49, 17, 57, 25,
};

// Permutation
const static u8 p[] = {
    16, 7, 20, 21, 29, 12, 28, 17, 1,  15, 23, 26, 5,  18, 31, 10,
    2,  8, 24, 14, 32, 27, 3,  9,  19, 13, 30, 6,  22, 11, 4,  25,
};

// Expansion function
const static u8 e[] = {
    32, 1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,  8,  9,  10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25, 24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1,
};

// Substitution boxes
const static u8 s[][64] = {
    // clang-format off
    {
        14, 4,  13, 1, 2,  15, 11, 8, 3, 10, 6, 12, 5,  9,  0,  7,  0,  15, 7,  4,  14, 2,
        13, 1,  10, 6, 12, 11, 9,  5, 3, 8,  4, 1,  14, 8,  13, 6,  2,  11, 15, 12, 9,  7,
        3,  10, 5,  0, 15, 12, 8,  2, 4, 9,  1, 7,  5,  11, 3,  14, 10, 0,  6,  13,
    },
    {
        15, 1,  8,  14, 6,  11, 3,  4, 9,  7,  2, 13, 12, 0,  5,  10, 3,  13, 4,  7, 15, 2,
        8,  14, 12, 0,  1,  10, 6,  9, 11, 5,  0, 14, 7,  11, 10, 4,  13, 1,  5,  8, 12, 6,
        9,  3,  2,  15, 13, 8,  10, 1, 3,  15, 4, 2,  11, 6,  7,  12, 0,  5,  14, 9,
    },
    {
        10, 0,  9,  14, 6, 3,  15, 5,  1,  13, 12, 7, 11, 4,  2,  8,  13, 7, 0,  9,  3, 4,
        6,  10, 2,  8,  5, 14, 12, 11, 15, 1,  13, 6, 4,  9,  8,  15, 3,  0, 11, 1,  2, 12,
        5,  10, 14, 7,  1, 10, 13, 0,  6,  9,  8,  7, 4,  15, 14, 3,  11, 5, 2,  12,
    },
    {
        7, 13, 14, 3, 0, 6,  9, 10, 1,  2, 8,  5, 11, 12, 4,  15, 13, 8,  11, 5,  6, 15,
        0, 3,  4,  7, 2, 12, 1, 10, 14, 9, 10, 6, 9,  0,  12, 11, 7,  13, 15, 1,  3, 14,
        5, 2,  8,  4, 3, 15, 0, 6,  10, 1, 13, 8, 9,  4,  5,  11, 12, 7,  2,  14,
    },
    {
        2,  12, 4, 1,  7,  10, 11, 6, 8, 5,  3, 15, 13, 0,  14, 9,  14, 11, 2,  12, 4,  7,
        13, 1,  5, 0,  15, 10, 3,  9, 8, 6,  4, 2,  1,  11, 10, 13, 7,  8,  15, 9,  12, 5,
        6,  3,  0, 14, 11, 8,  12, 7, 1, 14, 2, 13, 6,  15, 0,  9,  10, 4,  5,  3,
    },
    {
        12, 1,  10, 15, 9,  2,  6, 8,  0, 13, 3,  4,  14, 7,  5, 11, 10, 15, 4, 2,  7, 12,
        9,  5,  6,  1,  13, 14, 0, 11, 3, 8,  9,  14, 15, 5,  2, 8,  12, 3,  7, 0,  4, 10,
        1,  13, 11, 6,  4,  3,  2, 12, 9, 5,  15, 10, 11, 14, 1, 7,  6,  0,  8, 13,
    },
    {
        4, 11, 2,  14, 15, 0,  8,  13, 3, 12, 9,  7, 5,  10, 6,  1,  13, 0,  11, 7,  4, 9,
        1, 10, 14, 3,  5,  12, 2,  15, 8, 6,  1,  4, 11, 13, 12, 3,  7,  14, 10, 15, 6, 8,
        0, 5,  9,  2,  6,  11, 13, 8,  1, 4,  10, 7, 9,  5,  0,  15, 14, 2,  3,  12,
    },
    {
        13, 2, 8,  4, 6, 15, 11, 1,  10, 9,  3, 14, 5,  0,  12, 7,  1,  15, 13, 8, 10, 3,
        7,  4, 12, 5, 6, 11, 0,  14, 9,  2,  7, 11, 4,  1,  9,  12, 14, 2,  0,  6, 10, 13,
        15, 3, 5,  8, 2, 1,  14, 7,  4,  10, 8, 13, 15, 12, 9,  0,  3,  5,  6,  11,
    }
    // clang-format on
};

const static u8 shift[] = { 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };

typedef u64 Subkey;
typedef Subkey Subkeys[16];

static bool
get_bit(u64 value, u64 bit) {
    u64 mask = 1ull << (63ull - bit);
    return (value & mask) != 0;
}

static u64
set_bit(u64 value, u64 bit, bool v) {
    u64 mask = 1ull << (63ull - bit);
    if (v) {
        return value | mask;
    } else {
        return value & ~mask;
    }
}

static u64
permute(u64 value, const u8* permuted_choice, u64 len) {
    u64 permuted_value = 0;
    for (u64 i = 0; i < len; i++) {
        u64 bit = permuted_choice[i] - 1;
        permuted_value = set_bit(permuted_value, i, get_bit(value, bit));
    }

    return permuted_value;
}

u32
shift_left28(u32 value, u32 times) {
    for (u32 i = 0; i < times; i++) {
        u32 carry = value & (1u << 27);
        value <<= 1;
        value &= 0xFFFFFFF;
        if (carry) value |= 1;
    }

    return value;
}

static void
generate_subkeys(DesKey key, Subkeys out) {
    DesKey permuted_key = permute(key, pc1, array_len(pc1));

    u32 left = (permuted_key >> 36) & 0xFFFFFFF;
    u32 right = (permuted_key >> 8) & 0xFFFFFFF;

    for (u32 i = 0; i < 16; i++) {
        right = shift_left28(right, shift[i]);
        left = shift_left28(left, shift[i]);

        u64 concat = ((u64)left << 28) | (u64)right;
        concat <<= 8;
        out[i] = permute(concat, pc2, array_len(pc2));
    }
}

static u32
feistel(u32 halfblock, Subkey subkey) {
    u64 expanded = permute((u64)halfblock << 32, e, array_len(e));
    expanded ^= subkey;

    u64 substituted = 0;
    for (u64 i = 0; i < 8; i++) {
        u64 j = i * 6;
        u32 bits[6] = { 0 };
        for (u64 k = 0; k < 6; k++) {
            if (get_bit(expanded, j + k)) {
                bits[k] = 1;
            }
        }

        u64 row = 2 * bits[0] + bits[5];
        u64 col = 8 * bits[1] + 4 * bits[2] + 2 * bits[3] + bits[4];
        u64 m = s[i][row * 16 + col];
        u64 n = 1;

        while (m > 0) {
            u64 bit = (i + 1) * 4 - n;
            substituted = set_bit(substituted, bit, m & 1ul);

            m >>= 1;
            n++;
        }
    }

    return permute(substituted, p, array_len(p)) >> 32;
}

static u64
process_block(u64 block, Subkeys subkeys) {
    u64 permuted = permute(block, ip, array_len(ip));
    u32 left = (permuted >> 32) & 0xFFFFFFFF;
    u32 right = permuted & 0xFFFFFFFF;

    for (u64 i = 0; i < 16; i++) {
        u32 tmp = right;
        right = feistel(right, subkeys[i]);
        right ^= left;
        left = tmp;
    }

    u64 block_cipher = ((u64)right << 32) | (u64)left;
    block_cipher = permute(block_cipher, ip2, array_len(ip2));

    return block_cipher;
}

Buffer
des_encrypt(Buffer message, DesKey key) {
    Subkeys subkeys;
    generate_subkeys(key, subkeys);

    u8 padding = 8 - (message.len % 8);
    u64 len = message.len + padding;
    u8* buffer = malloc(len);
    if (!buffer) return (Buffer){ 0 };

    u64 i;
    for (i = 0; i + 7 < message.len; i += 8) {
        u64 block = read_u64(&message.ptr[i]);
        u64 cipher = process_block(block, subkeys);

        u8* bytes = (u8*)&cipher;
        buffer[i + 0] = bytes[7];
        buffer[i + 1] = bytes[6];
        buffer[i + 2] = bytes[5];
        buffer[i + 3] = bytes[4];
        buffer[i + 4] = bytes[3];
        buffer[i + 5] = bytes[2];
        buffer[i + 6] = bytes[1];
        buffer[i + 7] = bytes[0];
    }
    if (i < len) {
        u64 block;
        ft_memset(buffer_create((u8*)&block, sizeof(block)), padding);

        u8* ptr = &message.ptr[i];
        for (u64 j = 56; i < message.len; i++, j -= 8) {
            u64 mask = (0xFFull << j);
            block &= ~mask;
            block |= (u64)*ptr << j;
            ptr++;
        }

        u64 cipher = process_block(block, subkeys);

        u8* bytes = (u8*)&cipher;
        buffer[i + 0] = bytes[7];
        buffer[i + 1] = bytes[6];
        buffer[i + 2] = bytes[5];
        buffer[i + 3] = bytes[4];
        buffer[i + 4] = bytes[3];
        buffer[i + 5] = bytes[2];
        buffer[i + 6] = bytes[1];
        buffer[i + 7] = bytes[0];
    }

    return buffer_create(buffer, len);
}

Buffer
des_decrypt(Buffer message, DesKey key) {
    (void)message;
    (void)key;
    return (Buffer){ 0 };
}
