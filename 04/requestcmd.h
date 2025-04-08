#ifndef __REQUESTCMD_H__
#define __REQUESTCMD_H__
#ifdef __KERNEL__
    #include <linux/ioctl.h>
#else
    #include <sys/ioctl.h>
#endif
enum LED_STATUS {
    LED_ON = 0,
    LED_OFF = 1,
};
#define LED_CTL_CMD _IOW('L', 0, enum LED_STATUS)

#endif