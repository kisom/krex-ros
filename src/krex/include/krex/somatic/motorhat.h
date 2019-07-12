#ifndef __KREX_MOTORHAT_H
#define __KREX_MOTORHAT_H


// A lot of this code is a port of parts of the Adafruit Python PCA9685
// controller library. This is designed to control an Adafruit DC &
// Stepper Motor Hat (part #2348).
//
// https://github.com/adafruit/Adafruit-Motor-HAT-Python-Library/


#include <krex/Drive.h>
#include <krex/DriveHealth.h>
#include <krex/hardware/i2c.h>


namespace krex {
namespace somatic {


enum class Behaviour : uint8_t {
	Release = 0,
	Forward = 1,
	Backward = 2
};


bool	ResetAll(uint8_t bus);

class MotorHat {
public:
	MotorHat();
	MotorHat(uint8_t addr);
	MotorHat(uint8_t bus, uint8_t addr);

	bool		Drive(const krex::Drive::ConstPtr &msg);
	bool		Go(Behaviour behaviour, uint8_t speed);
	bool		SetMotor(uint8_t id, Behaviour behaviour, uint8_t speed);
	bool		ReleaseAll();
	void		Health(krex::DriveHealth *msg);

private:
	I2CDevice	controller;
	uint8_t		currentBehaviour;
	uint8_t		currentSpeed;
	bool		healthy;
	double		prescale;

	bool		initController();
	bool		setPrescaler(double freq);
	bool		setAllChannels(uint16_t on, uint16_t off);
	bool		setChannel(uint8_t id, uint16_t on, uint16_t off);
};

} // namespace somatic
} // namespace krex

#endif /* __KREX_MOTORHAT_H */
