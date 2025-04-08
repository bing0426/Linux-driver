#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "requestcmd.h"

int main(int argc, char ** argv){

    int fd;
    char userbuf[128] = "copilot";
    int err;
    int offset = 2;
    enum LED_STATUS led_status = LED_ON;
    if(argc != 2){
        printf("usag %s <filepath>\n", argv[0]);
        return 0;
    }
    fd = open(argv[1], O_RDWR);
    if(fd < 0){
        perror("fd");
        printf("%s \n",argv[1]);
        return -1;
    }
    err = write(fd, userbuf, strlen(userbuf));
    if(err < 0){
        perror("write");
        close(fd);
        return -1;
    }
    memset(userbuf, 0, strlen(userbuf));
    err = read(fd, userbuf, sizeof(userbuf)-1);
    if(err < 0){
        perror("read");
        close(fd);
        return -1;
    }
    userbuf[strlen(userbuf)] = '\0';
    printf("%s\n",userbuf);
    while(1){
        char err;
        err = getchar();
        printf("%c",err);
        if(err == '0'){
            led_status = LED_OFF;
        }
        if(err == '1'){
            led_status = LED_ON;
        }
        if(err == 'c')
            break;
        ioctl(fd, LED_CTL_CMD, &led_status);
    }
    close(fd);
    return 0;
}