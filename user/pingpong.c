#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

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
    char ch[1];
    int p1[2], p2[2];
    if(pipe(p1) < 0 || pipe(p2) < 0)
      panic("pipe");
    int pid;
    if((pid = fork1()) != 0){
      close(1);
      dup(p1[1]);
      close(p1[0]);
      close(p1[1]);
      write(1, "a", 1);
      close(1);
      wait(0);

      close(0);
      dup(p2[0]);
      close(p2[0]);
      close(p2[1]);
      read(0, ch, 1);
      fprintf(2, "%d: received pong\n", getpid());
      exit(0);
    }
    else {
      close(0);
      dup(p1[0]);
      close(p1[0]);
      close(p1[1]);
      read(0, ch, 1);
      fprintf(2, "%d: received ping\n", getpid());

      close(1);
      dup(p2[1]);
      close(p2[0]);
      close(p2[1]);
      write(1, "b", 1);
      exit(0);
    }
}
