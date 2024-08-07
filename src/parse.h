#pragma once

#include "ssl.h"

Command
parse_command(const char* str);

u32
parse_options(Command cmd, void* out_options);

void
print_help(Command cmd);
