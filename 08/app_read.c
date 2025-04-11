#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <string.h>
#include <sys/signal.h>

char buf[128] = {0};
int fd, ret;

int main(int argc, char ** argv){
    if(argc != 2){
        printf("Usage %s <devpath>\n", argv[0]);
        return -1;
    }
    fd = open(argv[1], O_RDWR);
    if(fd < 0){
        perror("open");
        return -1;
    }
    ret = read(fd, buf, sizeof(buf));
    write(fd, buf, sizeof(buf));
    close(fd);
}