#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int
main(int argc, char *argv[])
{
    char cmd[256];
    char* arg[MAXARG];
    for (int i = 1; i < argc; i++) 
        arg[i - 1] = argv[i];
    arg[argc - 1] = (char*)malloc(MAXARG * sizeof(char));
    int idx = 0;
    // fprintf(2, "%d\n", argc);
    int len;
    while ((len = read(0, cmd, sizeof(cmd))) > 0) {
        cmd[len] = 0;
        // fprintf(2, "tmp: %d\n", len);
        for (int i = 0; i < len; i++) {
            if (cmd[i] == '\n') {
                int pid = fork();
                if (pid == -1) {
                    fprintf(2, "fork");
                    exit(1);
                }
                else if (pid == 0) {
                    arg[argc - 1][idx] = 0;
                    // fprintf(2, "%s %s %s %d\n", arg[0], arg[1], arg[2], idx);
                    exec(arg[0], &arg[0]);
                }
                idx = 0;
                wait(0);
            }
            else {
                arg[argc - 1][idx++] = cmd[i];
            }

        }
    }

    exit(0);
}