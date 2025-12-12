#include <string.h>
#include <linux/uinput.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>


void emit (int fd, int type, int code, int val);
int linux_main ( );