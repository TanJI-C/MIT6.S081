#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#define RD 0
#define WR 1
void
panic(char *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}

int
fork1(void)
{
  int pid;

  pid = fork();
  if(pid == -1)
    panic("fork");
  return pid;
}

int
main(int argc, char *argv[])
{
    close(0);
    close(1);
    int pip[2][2];
    int flag = 1;
    if (pipe(pip[flag]) < 0) panic("pipe");
    if (pipe(pip[!flag]) < 0) panic("pipe");
    close(pip[flag][WR]);
    for (int i = 2; i <= 35; i++)
        write(pip[!flag][WR], &i, sizeof(int));
    start:
    close(pip[!flag][WR]);
    close(pip[flag][RD]);
    flag = !flag;
    if (pipe(pip[!flag]) < 0) panic("pipe");
    int p;
    read(pip[flag][RD], &p, sizeof(int));
    
    fprintf(2, "prime %d\n", p);
    int n, flag1 = 1;
    while (read(pip[flag][RD], &n, sizeof(int)) == sizeof(int)) {
        if (n % p != 0) {
            write(pip[!flag][WR], &n, sizeof(int));
            if (flag1) {
                int pid = fork1();
                flag1 = 0;
                if (pid == 0) goto start;
                close(pip[!flag][RD]);
            }
        }
    }
    if (flag1) close(pip[!flag][RD]);
    close(pip[flag][RD]);
    close(pip[!flag][WR]);
    // wait(0);
    exit(0);
    
}
