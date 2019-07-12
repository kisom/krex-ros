#ifndef __KREX_SENSORY_LIDAR_H
#define __KREX_SENSORY_LIDAR_H


#include <krex/hardware/i2c.h>


namespace krex {
namespace sensory {


class Lidar {
public:
	Lidar();

	bool	Healthy();
private:
	I2CDevice	lidar;
};


} // namespace sensory
} // namespace krex


#endif // __KREX_SENSORY_LIDAR_H
