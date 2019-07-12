extern "C" {
	#include <linux/i2c-dev.h>
	#include <i2c/smbus.h>
}
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <cassert>
#include <iostream>
#include <cstdlib>

#include "krex/hardware/i2c.h"


I2CDevice::I2CDevice(uint8_t _bus, uint8_t _addr)
	: addr(_addr)
{
	char	devBus[11] = "/dev/i2c- ";	

	if (_bus > 9) {
		std::cerr << "Invalid bus (0 <= bus <= 9)." << std::endl;
		abort();
	}
	_bus += 0x30;
	devBus[9] = _bus;

	bus = open(devBus, O_RDWR);
	if (bus < 0) {
		perror("open");
		abort();
	}
}


bool
I2CDevice::ReadByte(uint8_t reg, uint8_t *data)
{
	uint8_t	buf[1] = {reg};

	if (ioctl(this->bus, I2C_SLAVE, this->addr) < 0) {
		perror("ioctl");	
		return false;
	}

	if (1 != write(this->bus, buf, 1)) {
		perror("write");
		return false;
	}

	if (1 != read(this->bus, data, 1)) {
		perror("read");
		return false;
	}

	return true;	
}


bool
I2CDevice::WriteByte(uint8_t reg, uint8_t data)
{
	uint8_t buf[2] = {reg, data};

	if (ioctl(this->bus, I2C_SLAVE, this->addr) < 0) {
		perror("ioctl");
		return false;
	}

	if (2 != write(this->bus, buf, 2)) {
		perror("write");
		return false;
	}

	return true;
}


bool
I2CDevice::WriteBuffer(uint8_t reg, uint8_t *data, uint8_t len)
{
	if (ioctl(this->bus, I2C_SLAVE, this->addr) < 0) {
		perror("ioctl");
		return false;
	}

	if (1 != write(this->bus, &reg, 1)) {
		perror("write");
		return false;
	}

	if ((ssize_t)len != write(this->bus, data, len)) {
		perror("write");
		return false;
	}

	return true;
}


int8_t
I2CDevice::ReadBuffer(uint8_t reg, uint8_t *data, uint8_t len)
{
	int8_t	rlen = -1;

	assert(len <= 32);	
	if (ioctl(this->bus, I2C_SLAVE, this->addr) < 0) {
		perror("ioctl");
		return false;
	}

	rlen = i2c_smbus_read_i2c_block_data(this->bus, reg, len, data);
	return rlen;
}

