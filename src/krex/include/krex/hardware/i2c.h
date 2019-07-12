#ifndef __KREX_HARDWARE_I2C_H
#define __KREX_HARDWARE_I2C_H


#include <stdint.h>


class I2CDevice {
public:
	static constexpr uint8_t	DEFAULT_BUS = 0x01;

	I2CDevice(uint8_t _bus, uint8_t _addr);
	bool ReadByte(uint8_t addr, uint8_t *data);
	bool WriteByte(uint8_t addr, uint8_t data);
	bool WriteBuffer(uint8_t addr, uint8_t *data, uint8_t len);
	int8_t ReadBuffer(uint8_t reg, uint8_t *data, uint8_t len);

private:
	uint8_t	addr;
	int	bus;
};


#endif // __KREX_HARDWARE_I2C_H
