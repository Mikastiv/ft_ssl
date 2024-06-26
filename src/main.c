#include "cipher.h"
#include "parse.h"
#include "ssl.h"
#include "types.h"
#include "utils.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

u32 argc;
const char* const* argv;
const char* progname = 0;

int
main(int in_argc, const char* const* in_argv) {
    argc = in_argc;
    argv = in_argv;

    if (argc > 0) progname = argv[0];
    if (argc < 2) {
        usage();
        return EXIT_FAILURE;
    }

    Command cmd = parse_command(argv[1]);
    if (cmd == Command_None) {
        dprintf(STDERR_FILENO, "%s: unknown command: '%s'\n", progname, argv[1]);
        print_help();
        return EXIT_FAILURE;
    }

    u32 first_input = 2;
    switch (cmd) {
        case Command_Md5:
        case Command_Sha256:
        case Command_Sha224:
        case Command_Sha512:
        case Command_Sha384:
        case Command_Whirlpool: {
            DigestOptions options = { 0 };
            first_input = parse_options(cmd, &options);

            bool success = digest(first_input, cmd, options);
            if (!success) return EXIT_FAILURE;
        } break;
        case Command_Base64: {
            Base64Options options = { 0 };
            first_input = parse_options(cmd, &options);
            if (options.decode && options.encode) {
                dprintf(STDERR_FILENO, "%s: cannot encode and decode at the same time\n", progname);
                exit(EXIT_FAILURE);
            }
            if (!options.decode && !options.encode) options.encode = true;

            int in_fd = -1;
            if (options.input_file) {
                in_fd = open(options.input_file, O_RDONLY);
                if (in_fd < 0) print_error_and_quit();
            } else {
                in_fd = STDIN_FILENO;
            }

            int out_fd = -1;
            if (options.output_file) {
                out_fd = open(
                    options.output_file,
                    O_WRONLY | O_CREAT | O_TRUNC,
                    S_IRWXU | S_IRGRP | S_IROTH
                );
                if (out_fd < 0) print_error_and_quit();
            } else {
                out_fd = STDOUT_FILENO;
            }

            Buffer input = read_all_fd(in_fd);
            if (options.input_file) close(in_fd);
            if (!input.ptr) print_error_and_quit();

            Buffer res;
            if (options.decode) {
                res = base64_decode(input);
            } else {
                res = base64_encode(input);
            }

            if (!res.ptr) {
                dprintf(STDERR_FILENO, "%s: invalid input\n", progname);
                exit(EXIT_FAILURE);
            }

            write(out_fd, res.ptr, res.len);
            if (options.encode) write(out_fd, "\n", 1);
            if (options.output_file) close(out_fd);

        } break;
        case Command_Des:
        case Command_DesEcb:
        case Command_DesCbc: {
            u64 msg = byte_swap64(0x8787878787878787);
            DesKey key = { .raw = byte_swap64(0x0E329232EA6D0D73) };
            Des64 iv = { .raw = 0x5711577929EFDFF1 };
            (void)iv;

            Buffer cipher = des_ecb_encrypt(buffer_create((u8*)&msg, 8), key);
            for (u64 i = 0; i < cipher.len; i++) {
                dprintf(1, "%02X", cipher.ptr[i]);
            }
            dprintf(1, "\n");

            Buffer original = des_ecb_decrypt(cipher, key);
            for (u64 i = 0; i < original.len; i++) {
                dprintf(1, "%02X", original.ptr[i]);
            }
            dprintf(1, "\n");

            Des64 salt = { .raw = 0x23879823498FDCE3 };
            Des64 prf = des_pbkdf2_generate(str("Hello World!"), &salt);
            for (u64 i = 0; i < 8; i++) {
                printf("%02X", prf.block[i]);
            }
            printf("\n");
        } break;
        case Command_None: {
            dprintf(STDERR_FILENO, "Unreachable\n");
            return EXIT_FAILURE;
        } break;
    }
}
