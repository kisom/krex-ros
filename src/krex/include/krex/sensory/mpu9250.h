#ifndef __KREX_HARDWARE_MPU9250_H
#define __KREX_HARDWARE_MPU9250_H


#include <stdint.h>
#include <krex/hardware/i2c.h>


namespace krex {
namespace sensory {


class MPU9250 {
public:
	MPU9250();
	MPU9250(uint8_t addr);

	bool	Healthy() { return this->healthy; }
	bool	Reset();
	bool	Temperature(double *temp);
	bool	WhoAmI();
private:
	uint8_t		addr; // I2C bus address
	I2CDevice	mpu;
	bool		healthy;

	bool		init();
};

} // namespace sensory
} // namespace krex


#endif // __KREX_HARDWARE_MPU9250_H
