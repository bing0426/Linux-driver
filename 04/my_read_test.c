#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char ** argv){
    int fd;
    char user_buf[128];
    if(argc != 2){
        printf("Usage %s <devpath>\n", argv[0]);
        return -1;
    }
    fd = open(argv[1],O_RDWR|O_NONBLOCK);
    if(fd < 0){
        perror("fd");
        return -1;
    }
    memset(user_buf, 0, sizeof(user_buf));
    while(1){
        read(fd, user_buf, sizeof(user_buf));
        printf("读取结果: %s\n", user_buf);
        memset(user_buf, 0, sizeof(user_buf));
        sleep(1);
    }
    close(fd);
    return 0;
}