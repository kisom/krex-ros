#include <iostream>
#include <krex/hardware/i2c.h>
#include <krex/sensory/lidar.h>


namespace krex {
namespace sensory {


constexpr uint8_t	DEFAULT_ADDRESS = 0x62;

constexpr uint8_t	REG_UNIT_ID_LOW = 0x17;
constexpr uint8_t	REG_UNIT_ID_HIGH = 0x18;
constexpr uint8_t	REG_POWER = 0x65;


Lidar::Lidar()
	: lidar(I2CDevice::DEFAULT_BUS, DEFAULT_ADDRESS)
{

}


bool
Lidar::Healthy()
{
	uint8_t		tmp;
	uint16_t	id;

	std::cerr << "lidar: read power settings\n";
	if (!this->lidar.ReadByte(REG_POWER, &tmp)) {
		return false;
	}

	if (tmp != 0x80) {
		std::cerr << "lidar: bad power " << int(tmp) << "\n";
		return false;
	}

	std::cerr << "lidar: read unit ID\n";
	if (!this->lidar.ReadByte(REG_UNIT_ID_HIGH, &tmp)) {
		return false;
	}
	id = tmp;
	id <<= 8;
	if (!this->lidar.ReadByte(REG_UNIT_ID_LOW, &tmp)) {
		return false;
	}
	id += tmp;
	std::cerr << "lidar: unit ID=" << int(id) << "\n";
	return true;
}


} // namespace sensory
} // namespace krex
