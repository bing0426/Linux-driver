#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <string.h>
#include <sys/signal.h>

int fd, ret;

void xxx_callback(int sig){
    if(sig == SIGIO){
        char buf[128] = {0};
        ret = read(fd, buf, sizeof(buf));
        if(ret < 0){
            perror("read");
            return ;
        }
        buf[ret] = '\0';
        printf("read: %s\n",buf);
    }
}

int main(int argc, char ** argv){
    if(argc != 2){
        printf("Usage %s <devpath>\n", argv[0]);
        return -1;
    }
    signal(SIGIO, xxx_callback);
    fd = open(argv[1], O_RDWR);
    if(fd < 0){
        perror("open");
        return -1;
    }
    fcntl(fd, F_SETOWN, getpid());
    int flag = fcntl(fd, F_GETFL);
    flag |= FASYNC;
    fcntl(fd, F_SETFL, flag);
    while(1){
        sleep(1);
        printf("sleeping\n");
    }
    close(fd);
}