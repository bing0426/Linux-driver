#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <string.h>
#include <linux/input.h>

int main(int argc, char ** argv){
    int fd1, fd2, ret;
    char user_buf[128];
    int fdmax;
    struct input_absinfo mouseInfo = {0};
    fd_set save_fd_set, modify_fd_set;
    if(argc != 2){
        printf("Usage %s <devpath>\n", argv[0]);
        return -1;
    }
    fd1 = open(argv[1],O_RDWR);
    if(fd1 < 0){
        perror("fd1");
        return -1;
    }
    printf("%d \n",fd1);
    //fd1 = 0;
    fd2 = open("/dev/input/mouse0", O_RDONLY);
    if(fd2 < 0){
        perror("fd2");
        return -1;
    }
    FD_ZERO(&save_fd_set);
    FD_SET(fd1, &save_fd_set);
    FD_SET(fd2, &save_fd_set);
    fdmax = fd1 > fd2 ? fd1 : fd2;
    memset(user_buf, 0, sizeof(user_buf));
    while(1){
        modify_fd_set = save_fd_set;
        int fds = select(fdmax+1, &modify_fd_set, NULL, NULL, NULL);
        if(fds < 0){
            perror("select err ");
            break;
        }
        for(int fd = 0; fd < fdmax+1; fd++){
            if(FD_ISSET(fd, &modify_fd_set)){
                if(fd == fd1){
                    ret = read(fd1, user_buf, sizeof(user_buf)-1);
                    if(ret < 0){
                        perror("fd1 read");
                        break;
                    }
                    user_buf[ret] = '\0';
                    printf("读取结果: %s\n", user_buf);
                    memset(user_buf, 0, sizeof(user_buf));
                }
                if(fd == fd2){
                    printf("%d ", __LINE__);
                    ret = read(fd2, &mouseInfo, sizeof(mouseInfo));
                    if(ret < 0){
                        perror("fd2 read");
                        break;
                    }
                    printf("mouse: %d\n", mouseInfo.value);
                }
            }
        }

    }
    close(fd1);
    close(fd2);
    return 0;
}