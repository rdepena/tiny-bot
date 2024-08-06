#include "serial.h"

int set_interface_attribs(int fd, int speed) {
    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        return -1;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    // 8-bit chars
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; 
    // disable break processing
    tty.c_iflag &= ~IGNBRK;
    // no signaling chars, no echo,
    // no canonical processing
    tty.c_lflag = 0;
    // no remapping, no delays
    tty.c_oflag = 0;
    // read doesn't block
    tty.c_cc[VMIN]  = 0;
    // 0.5 seconds read timeout
    tty.c_cc[VTIME] = 5;
    // shut off xon/xoff ctrl
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    // ignore modem controls,
    // enable reading
    tty.c_cflag |= (CLOCAL | CREAD);
    // shut off parity
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag |= 0;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        return -1;
    }
    return 0;
}

void set_blocking(int fd, int should_block) {
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        return;
    }

    tty.c_cc[VMIN]  = should_block ? 1 : 0;
    // 0.5 seconds read timeout
    tty.c_cc[VTIME] = 5;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
    }
}

int open_port(const char *portname) {
    int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    // set speed to 9600 bps, 8n1 (no parity)
    set_interface_attribs(fd, B9600);
    // set no blocking
    set_blocking(fd, 0);

    return fd;
}

int clean_serial(int fd) {
    return close(fd);
}

int read_from_port(int fd) {
    //serial read buffer
    char buf[100];
    // read up to 100 characters if ready to read
    int n = read(fd, buf, sizeof(buf) - 1);

    if (n > 0) {
        // Null-terminate the string
        buf[n] = '\0';

        int distance = atoi(buf);

        return distance;
    } else {
        return -1;
    }
}