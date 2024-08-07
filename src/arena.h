#pragma once

#include "types.h"

typedef struct {
    char* block;
    u64 size;
    u64 index;
    u64 high_watermark;
} Arena;

bool
arena_init(Arena* arena, u64 size);

void*
arena_alloc(Arena* arena, u64 size);

void
arena_clear(Arena* arena);

void
arena_free(Arena* arena);

void
arena_log_watermark(Arena* arena);
