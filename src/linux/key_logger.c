#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main ( ) {
    int fd = open("/dev/input/event2", O_RDONLY);
    if (fd < 0) {
        perror("open event device");
        return 1;
    }

    FILE * log = fopen("keypresses.txt", "w");
    if (! log) {
        perror("fopen");
        return 1;
    }

    struct input_event ev;

    while (1) {
        ssize_t n = read(fd, &ev, sizeof(ev));
        if (n != sizeof(ev)) continue;

        if (ev.type == EV_KEY && ev.value == 1) {
            fprintf(log, "KEYCODE %d\n", ev.code);
            fflush(log);
        }
    }

    close(fd);
    fclose(log);
    return 0;
}
