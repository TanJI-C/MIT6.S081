#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void find(char *path, char *str) {
    int fd;
    struct dirent de;
    struct stat st;
    if ((fd = open(path, 0)) < 0) {
        fprintf(1, "find: cannot open %s\n", path);
        return;
    }
    if (fstat(fd, &st) < 0) {
        fprintf(1, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type) {
    case T_FILE:
        if (memcmp(fmtname(path), str, strlen(str)) == 0) 
            fprintf(1, "%s\n", path);
        break;
    case T_DIR:
        if (strlen(path) + 1 + DIRSIZ + 1 > 512) {
            fprintf(1, "find: path too long\n");
            break;
        }
        char *p = path + strlen(path);
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0) continue;
            int curlen = strlen(de.name);
            if (memcmp(".", de.name, curlen) == 0 || 
                memcmp("..", de.name, curlen) == 0)
                continue;
            memmove(p, de.name, curlen);
            p[curlen] = 0;
            if (stat(path, &st) < 0) { //get stat of file
                printf("find: cannot stat %s\n", path);
                continue;
            }
            if (st.type == T_DIR) {
                find(path, str);
            }
            else {
                if (memcmp(de.name, str, strlen(str)) == 0) 
                    fprintf(1, "%s\n", path);
            }

        }


        break;
    }
}

int
main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(2, "usage: find <dir> <string>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
