#include <stdio.h>
#include <string.h>
#define BOB_STRIP_PREFIXES
#define BOB_IMPL
#include "bob.h"

#define CC "gcc"
#define BUILD_DIR "build"
#define TARGET BUILD_DIR"/sosijc"

void build_target(Cmd *cmd) {
    cmd_append(cmd, "mkdir", "-p", BUILD_DIR);
    cmd_run_and_reset(cmd);

    cmd_append(cmd, CC, "src/*.c", "-o", TARGET);
    cmd_append(cmd, "-ggdb", "-Wall", "-Wextra", "-fsanitize=address");
    cmd_run_and_reset(cmd);

}

void run_target(Cmd *cmd) {
    cmd_append(cmd, "./"TARGET);
    cmd_run_and_reset(cmd);
}

void clean(Cmd *cmd) {
    // delete build dir
    cmd_append(cmd, "rm", "-rf", BUILD_DIR);
    cmd_run_and_reset(cmd);

    // delete './sosijc' output files
    if (check_file_exists("./out.s")) {
        cmd_append(cmd, "rm", "out*");
        cmd_run_and_reset(cmd);
    }
}

int main(int argc, char **argv) {
    rebuild_yourself(argc, argv);

    Cmd cmd = {0};
    if (argc > 1) {
        const char *arg = argv[1];

        if (strcmp(arg, "run") == 0) {
            printf("Build and Run...\n");
            build_target(&cmd);
            run_target(&cmd);
        }
        else if (strcmp(arg, "clean") == 0) {
            printf("Clean...\n");
            clean(&cmd);
        }
        else
            printf("Unknown command: %s\n", arg);
    }
    else {
        printf("Building...\n");
        build_target(&cmd);
    }

    cmd_destroy(&cmd);
    return 0;
}
