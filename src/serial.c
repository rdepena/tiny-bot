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

	return 0;
}