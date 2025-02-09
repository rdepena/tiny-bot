#ifndef SERIAL_H
#define SERIAL_H

#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int open_port(const char *portname);

int clean_serial(int fd);

int read_from_port(int fd);

int write_to_port(int fd, char *msg);

#endif // SERIAL_H