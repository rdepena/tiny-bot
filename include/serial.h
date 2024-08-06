#ifndef SERIAL_H
#define SERIAL_H

#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int set_interface_attribs(int fd, int speed);

#endif // SERIAL_H