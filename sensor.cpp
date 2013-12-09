#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>


#define CHECK_ERROR(X) if (X != 0) {printf("error on %s:%i: %i\n", __FILE__, __LINE__, errno); return -1;}
#define MEASURE_IN_CM 84

static char *ttyFileName = "/dev/ttyAMA0";
static int sensorFd;

void send(unsigned char cmd) {
	unsigned char data[2];

	data[0] = 0; // default ID of SRV02
	data[1] = cmd;
	write(sensorFd, data, 2);
}

int connectDistanceSensor() {
	struct termios options;
	sensorFd = open(ttyFileName, O_RDWR | O_NONBLOCK | O_NOCTTY );
	if (sensorFd == -1) {
		printf("could not open tty\n");
		return -1;
	}

	CHECK_ERROR(tcgetattr(sensorFd, &options));
	options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	CHECK_ERROR(tcflush(sensorFd, TCIFLUSH));
	CHECK_ERROR(tcsetattr(sensorFd, TCSANOW, &options));

	return 0;
}

static int readDistance() {
	int bytesRead;
	unsigned char data[2];
	int dataIndex = 0;
	int distance = 0;

	send(MEASURE_IN_CM);

	while (dataIndex < 2) {
		bytesRead = read(sensorFd, &(data[dataIndex]), 1);
		if (bytesRead > 0) {
			dataIndex++;
		}
	}

	distance |= data[0] << 8;
	distance |= data[1];
	return distance;
}

void blockUntilDistanceChanges() {
	int normValue = 0;
	int diffReq;
	int distance;

	// first read 3 values to calibrate
	normValue += readDistance();
	normValue += readDistance();
	normValue += readDistance();
	normValue /= 3;
	do {
		usleep(10000); // sleep 10ms
		distance = readDistance();
		if (distance <= 0) continue; // wrong value
printf("%i\n",distance);
		if (distance > normValue) {
			normValue = distance;
		}
		diffReq = normValue / 10;
		if (diffReq == 0) diffReq = 1;
	} while (distance > (normValue - diffReq));
}

void closeDistanceSensor() {
	close(sensorFd);
}
