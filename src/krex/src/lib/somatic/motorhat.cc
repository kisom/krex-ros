#include <chrono>
#include <cmath>
#include <thread>

#include <krex/Drive.h>
#include <krex/DriveHealth.h>
#include <krex/hardware/i2c.h>
#include <krex/somatic/motorhat.h>

namespace krex {
namespace somatic {


// The defaults presume an Adafruit PCA9685 controller on a Jetson Nano.
constexpr uint8_t	DEFAULT_ADDRESS = 0x60;

// This is based on how the motors are wired on the bot.
constexpr uint8_t	MOTOR_RF = 0;
constexpr uint8_t	MOTOR_RR = 1;
constexpr uint8_t	MOTOR_LF = 2;
constexpr uint8_t	MOTOR_LR = 3;


// I had to do some soul-searching before adding this; it makes the code
// a lot easier to read and simplifies some of the boilerplate.
#define WITH_HEALTHCHECK(x)	if (!(x)) { this->healthy = false; return false; }


bool
ResetAll(uint8_t bus)
{
	I2CDevice	all(bus, 0x70);

	return all.WriteByte(0x0, 0x6);
}


// See the PCA9685 docs for descriptions. This was originally an enum class,
// but that made the code unnecessarily verbose, because what's really needed
// is the uint8_t value.
constexpr uint8_t REG_MODE1              = 0x00;
constexpr uint8_t REG_MODE2              = 0x01;
constexpr uint8_t REG_SUBADR1            = 0x02;
constexpr uint8_t REG_SUBADR2            = 0x03;
constexpr uint8_t REG_SUBADR3            = 0x04;
constexpr uint8_t REG_PRESCALE           = 0xFE;
constexpr uint8_t REG_LED0_ON_L          = 0x06;
constexpr uint8_t REG_LED0_ON_H          = 0x07;
constexpr uint8_t REG_LED0_OFF_L         = 0x08;
constexpr uint8_t REG_LED0_OFF_H         = 0x09;
constexpr uint8_t REG_ALL_LED_ON_L       = 0xFA;
constexpr uint8_t REG_ALL_LED_ON_H       = 0xFB;
constexpr uint8_t REG_ALL_LED_OFF_L      = 0xFC;
constexpr uint8_t REG_ALL_LED_OFF_H      = 0xFD;


// The follow bit constants are used in the docs.
constexpr uint8_t BV_RESTART            = 0x80;
constexpr uint8_t BV_SLEEP              = 0x10;
constexpr uint8_t BV_ALLCALL            = 0x01;
constexpr uint8_t BV_INVRT              = 0x10;
constexpr uint8_t BV_OUTDRV             = 0x04;


MotorHat::MotorHat() :
	controller(I2CDevice::DEFAULT_BUS, DEFAULT_ADDRESS),
	currentBehaviour(krex::Drive::DRIVE_RELEASE),
	currentSpeed(0),
	healthy(false),
	prescale(0.0)
{
	this->initController();
};


MotorHat::MotorHat(uint8_t addr) :
	controller(I2CDevice::DEFAULT_BUS, addr),
	currentBehaviour(krex::Drive::DRIVE_RELEASE),
	currentSpeed(0),
	healthy(false),
	prescale(0.0)
{
	this->initController();
};


MotorHat::MotorHat(uint8_t bus, uint8_t addr) :
	controller(bus, addr),
	currentBehaviour(krex::Drive::DRIVE_RELEASE),
	currentSpeed(0),
	healthy(false),
	prescale(0.0)
{
	this->initController();
};


bool
MotorHat::Control(const krex::Drive::ConstPtr &msg)
{
	uint8_t speed = static_cast<const uint8_t>(msg->speed);

	return this->Drive(msg->behaviour, speed);
}


bool
MotorHat::Drive(const uint8_t behaviour, const uint8_t speed)
{
	this->ReleaseAll();

	/*
	 * channel 0: RF
	 * channel 1: RR
	 * channel 2: LF
	 * channel 3: LR
	 */

	switch (behaviour) {
	case krex::Drive::DRIVE_RELEASE:
		// Already released.
		break;
	case krex::Drive::DRIVE_FORWARD:
		WITH_HEALTHCHECK(this->SetMotor(MOTOR_RF, Behaviour::Backward, speed));
		WITH_HEALTHCHECK(this->SetMotor(MOTOR_RR, Behaviour::Forward, speed));
		WITH_HEALTHCHECK(this->SetMotor(MOTOR_LF, Behaviour::Forward, speed));
		WITH_HEALTHCHECK(this->SetMotor(MOTOR_LR, Behaviour::Backward, speed));
		break;
	case krex::Drive::DRIVE_BACKWARD:
		WITH_HEALTHCHECK(this->SetMotor(MOTOR_RF, Behaviour::Forward, speed));
		WITH_HEALTHCHECK(this->SetMotor(MOTOR_RR, Behaviour::Backward, speed));
		WITH_HEALTHCHECK(this->SetMotor(MOTOR_LF, Behaviour::Backward, speed));
		WITH_HEALTHCHECK(this->SetMotor(MOTOR_LR, Behaviour::Forward, speed));
		break;
	case krex::Drive::DRIVE_TURN_LEFT:
		WITH_HEALTHCHECK(this->SetMotor(MOTOR_RF, Behaviour::Backward, speed));
		WITH_HEALTHCHECK(this->SetMotor(MOTOR_RR, Behaviour::Forward, speed));
		WITH_HEALTHCHECK(this->SetMotor(MOTOR_LF, Behaviour::Backward, speed));
		WITH_HEALTHCHECK(this->SetMotor(MOTOR_LR, Behaviour::Forward, speed));
		break;
	case krex::Drive::DRIVE_TURN_RIGHT:
		WITH_HEALTHCHECK(this->SetMotor(MOTOR_RF, Behaviour::Forward, speed));
		WITH_HEALTHCHECK(this->SetMotor(MOTOR_RR, Behaviour::Backward, speed));
		WITH_HEALTHCHECK(this->SetMotor(MOTOR_LF, Behaviour::Forward, speed));
		WITH_HEALTHCHECK(this->SetMotor(MOTOR_LR, Behaviour::Backward, speed));
		break;
	default:
		std::cerr << "invalid motor behaviour " << (int)behaviour << std::endl;
		// Getting a bad message isn't an indicator that the controller
		// is unhealthy.
		return false;
	}

	this->currentBehaviour = behaviour;
	this->currentSpeed = speed;
	return true;
}


bool
MotorHat::ReleaseAll()
{
	uint8_t	i = 0;

	for (i = 0; i < 4; i++) {
		WITH_HEALTHCHECK(this->SetMotor(i, Behaviour::Release, 255));
	}

	return true;
}


void
MotorHat::Health(krex::DriveHealth *msg)
{
	msg->healthy = this->healthy;
	msg->behaviour = this->currentBehaviour;
	msg->speed = this->currentSpeed;
}


// Go is useful for checking the behaviour of all of the motors, and isn't
// particularly useful for controlling the drivetrain in normal operation.
bool
MotorHat::Go(Behaviour behaviour, uint8_t speed)
{
	uint8_t	i = 0;

	for (i = 0; i < 4; i++) {
		WITH_HEALTHCHECK(this->SetMotor(i, behaviour, 255));
	}

	return true;
}


bool
MotorHat::initController()
{
	uint8_t	mode1;

	ResetAll(I2CDevice::DEFAULT_BUS);
	WITH_HEALTHCHECK(this->setPrescaler(1600.0));

	WITH_HEALTHCHECK(this->setAllChannels(0, 0));
	WITH_HEALTHCHECK(this->controller.WriteByte(REG_MODE2, BV_OUTDRV));
	WITH_HEALTHCHECK(this->controller.WriteByte(REG_MODE1, BV_ALLCALL));

	// Wait for the oscillator, then reset sleep.
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	WITH_HEALTHCHECK(this->controller.ReadByte(REG_MODE1, &mode1));
	mode1 &= ~BV_SLEEP;
	WITH_HEALTHCHECK(this->controller.WriteByte(REG_MODE1, mode1));
	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	WITH_HEALTHCHECK(this->ReleaseAll());

	this->healthy = true;
	return true;
}


// 25MHz with 16-bit sampling.
constexpr double prescale25MHz16bit = 25000000.0 / 4096.0;


bool
MotorHat::setPrescaler(double freq)
{
	this->prescale = (prescale25MHz16bit / freq) - 1.0;
	uint8_t	oldMode;
	uint8_t	newMode;
	uint8_t	prescaleValue = static_cast<uint8_t>(std::floor(this->prescale));

	WITH_HEALTHCHECK(this->controller.ReadByte(REG_MODE1, &oldMode));

	// Go to sleep, then set the prescale and mode.
	newMode = (oldMode & 0x7f) | BV_SLEEP;
	WITH_HEALTHCHECK(this->controller.WriteByte(REG_MODE1, newMode));
	WITH_HEALTHCHECK(this->controller.WriteByte(REG_PRESCALE, prescaleValue));
	WITH_HEALTHCHECK(this->controller.WriteByte(REG_MODE1, oldMode));

	// Wait for it... then wake up the controller.
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	WITH_HEALTHCHECK(this->controller.WriteByte(REG_MODE1, oldMode | BV_SLEEP));

	this->healthy = true;
	return true;
}


bool
MotorHat::setAllChannels(uint16_t on, uint16_t off)
{
	WITH_HEALTHCHECK(this->controller.WriteByte(REG_LED0_ON_L, on & 0xff));
	WITH_HEALTHCHECK(this->controller.WriteByte(REG_LED0_ON_H, on >> 8));

	WITH_HEALTHCHECK(this->controller.WriteByte(REG_LED0_OFF_L, off & 0xff));
	WITH_HEALTHCHECK(this->controller.WriteByte(REG_LED0_OFF_H, off >> 8));

	this->healthy = true;
	return true;
}


bool
MotorHat::setChannel(uint8_t id, uint16_t on, uint16_t off)
{
	WITH_HEALTHCHECK(this->controller.WriteByte(REG_LED0_ON_L + (4*id), on & 0xff));
	WITH_HEALTHCHECK(this->controller.WriteByte(REG_LED0_ON_H + (4*id), on >> 8));
	WITH_HEALTHCHECK(this->controller.WriteByte(REG_LED0_OFF_L + (4*id), off & 0xff));
	WITH_HEALTHCHECK(this->controller.WriteByte(REG_LED0_OFF_H + (4*id), off >> 8));

	this->healthy = true;
	return true;
}



// SetMotor is a behavioural interface to setChannel.
bool
MotorHat::SetMotor(uint8_t id, Behaviour behaviour, uint8_t speed)
{
	uint8_t		pwm, in1, in2;
	uint16_t	on = 0;
	uint16_t	off = 0;
	uint16_t	pwmSpeed = speed * 16;

	switch (id) {
	case 0:
		pwm = 8;
		in1 = 10;
		in2 = 9;
		break;
	case 1:
		pwm = 13;
		in1 = 11;
		in2 = 12;
		break;
	case 2:
		pwm = 2;
		in1 = 4;
		in2 = 3;
		break;
	case 3:
		pwm = 7;
		in1 = 5;
		in2 = 6;
		break;
	default:
		std::cerr << "invalid motor " << id << std::endl;
		return false;
	}

	switch (behaviour) {
	case Behaviour::Forward:
		on = 4096;
		off = 0;
		break;
	case Behaviour::Backward:
		on = 0;
		off = 4096;
		break;
	case Behaviour::Release:
		on = 0;
		off = 0;
		break;
	default:
		std::cerr << "invalid motor behaviour " << (int)behaviour << std::endl;
		return false;
	}

	WITH_HEALTHCHECK(this->setChannel(in1, on, off));
	WITH_HEALTHCHECK(this->setChannel(in2, off, on));
	WITH_HEALTHCHECK(this->setChannel(pwm, 0, pwmSpeed));
	this->healthy = true;
	return true;
}


} // namespace somatic
} // namespace krex

