#include <krex/util/util.h>
#include <krex/hardware/i2c.h>
#include <krex/sensory/mpu9250.h>


namespace krex {
namespace sensory {


constexpr uint8_t	DEFAULT_MPU9250_ADDRESS = 0x68;


// Register definitions.
constexpr uint8_t	REG_SELF_TEST_X_GYRO  = 0x00;
constexpr uint8_t	REG_SELF_TEST_Y_GYRO  = 0x01;
constexpr uint8_t	REG_SELF_TEST_Z_GYRO  = 0x02;
constexpr uint8_t	REG_SELF_TEST_X_ACCEL = 0x0d;
constexpr uint8_t	REG_SELF_TEST_Y_ACCEL = 0x0e;
constexpr uint8_t	REG_SELF_TEST_Z_ACCEL = 0x0f;
constexpr uint8_t	REG_XG_OFFSET_H       = 0x13;
constexpr uint8_t	REG_XG_OFFSET_L       = 0x14;
constexpr uint8_t	REG_YG_OFFSET_H       = 0x15;
constexpr uint8_t	REG_YG_OFFSET_L       = 0x16;
constexpr uint8_t	REG_ZG_OFFSET_H       = 0x17;
constexpr uint8_t	REG_ZG_OFFSET_L       = 0x18;
constexpr uint8_t	REG_SIMPLRT_DIV       = 0x19;
constexpr uint8_t	REG_GYRO_CONFIG       = 0x1a;
constexpr uint8_t	REG_ACCEL_CONFIG      = 0x1b;
constexpr uint8_t	REG_FIFO_EN           = 0x23;
constexpr uint8_t	REG_I2C_MST_CTRL      = 0x24;
constexpr uint8_t	REG_INT_PIN_CFG       = 0x37;
constexpr uint8_t	REG_TEMP_OUT_H        = 0x41;
constexpr uint8_t	REG_TEMP_OUT_L        = 0x42;
constexpr uint8_t	REG_USER_CTRL         = 0x6a;
constexpr uint8_t	REG_PWR_MGMT_1        = 0x6b;
constexpr uint8_t	REG_PWR_MGMT_2        = 0x6c;
constexpr uint8_t	REG_WHO_AM_I          = 0x75;


// Other constant values that are useful.
constexpr uint8_t	INT_BYPASS_EN         = 0x02; // I2C bypass
constexpr uint8_t	PWR_RESET             = 0x80;
constexpr uint8_t	WAI_NOMINAL           = 0x71;

constexpr uint16_t	TEMP_ROOM_OFFSET      = 21;
constexpr double	TEMP_SENSITIVITY      = 333.87;


MPU9250::MPU9250()
	: addr(DEFAULT_MPU9250_ADDRESS),
	  mpu(1, DEFAULT_MPU9250_ADDRESS),
	  healthy(false)
{
	if (this->init()) {
		this->healthy = true;
	}
}


MPU9250::MPU9250(uint8_t addr)
	: addr(addr),
	  mpu(1, addr),
	  healthy(false)
{
	if (this->init()) {
		this->healthy = true;
	}
}


bool
MPU9250::init()
{
	// Clear the sleep bit.
	if (!this->mpu.WriteByte(REG_PWR_MGMT_1, 0)) {
		return false;
	}

	// MPU9250 product spec v10 section 4.23, p28:
	// "Valid gyroscope data is available 35 ms (typical) after
	// VDD has risen to its final voltage from a cold start."
	krex::util::Sleep(35);

	// Disable master aux I2C mode.
	if (!this->mpu.WriteByte(REG_USER_CTRL, 0x00)) {
		return false;
	}

	// Enable I2C bypass.
	if (!this->mpu.WriteByte(REG_INT_PIN_CFG, INT_BYPASS_EN)) {
		return false;
	}

	return this->WhoAmI();
}


bool
MPU9250::WhoAmI()
{
	uint8_t	wai;
	if (!this->mpu.ReadByte(REG_WHO_AM_I, &wai)) {
		this->healthy = false;
		return false;
	}

	if (wai != WAI_NOMINAL) {
		this->healthy = false;
		return false;
	}

	this->healthy = true;
	return true;
}


bool
MPU9250::Reset()
{
	uint8_t	i;

	if (!this->mpu.WriteByte(REG_PWR_MGMT_1, PWR_RESET)) {
		return false;
	}

	// From product spec, 3.4.2 on p12: typical startup time is
	// 11ms, max is 100ms.
	for (i = 0; i < 11; i++) {
		if (this->WhoAmI()) {
			break;
		}
		krex::util::Sleep(10);
	}

	if (this->Healthy() && this->init()) {
		this->healthy = true;
	}

	return this->Healthy();
}


bool
MPU9250::Temperature(double *temp)
{
	uint16_t	temp_out;
	uint8_t		temp_buf;

	if (!this->mpu.ReadByte(REG_TEMP_OUT_H, &temp_buf)) {
		return false;
	}
	temp_out = temp_buf & 0xff;
	temp_out <<= 8;

	if (!this->mpu.ReadByte(REG_TEMP_OUT_L, &temp_buf)) {
		return false;
	}
	temp_out += temp_buf;
	
	temp_out -= TEMP_ROOM_OFFSET;
	*temp = static_cast<double>(temp_out);
	*temp /= TEMP_SENSITIVITY;
	*temp += static_cast<double>(TEMP_ROOM_OFFSET);
	return true;
}


} // namespace sensory
} // namespace krex
