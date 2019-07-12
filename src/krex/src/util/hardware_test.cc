#include <iostream>

#include <krex/DriveHealth.h>
#include <krex/somatic/motorhat.h>
#include <krex/sensory/lidar.h>
#include <krex/sensory/mpu9250.h>

using namespace std;


bool	testMotorHat();
bool	testIMU();
bool	testLidar();


struct component {
	string	name;
	bool	(*testFunction)();
	bool	run() {
		cout << "test: " << this->name << "... ";
		if (!this->testFunction()) {
			cout << "FAILED\n";
			return false;
		}
		else {
			cout << "OK\n";
			return true;
		}
	};
};

struct component componentTests[] = {
	{"MotorHat", testMotorHat},
	{"MPU9250", testIMU},
	{"Lidar", testLidar},
};
const uint8_t	componentTestCount = 3;


bool
testMotorHat()
{
	krex::somatic::MotorHat	hat;
	krex::DriveHealth	msg;

	hat.Health(&msg);
	return msg.healthy;
}


bool
testIMU()
{
	krex::sensory::MPU9250	imu;

	return imu.Healthy();
}


bool
testLidar()
{
	krex::sensory::Lidar	lidar;

	return lidar.Healthy();
}


int
main(int argc, char *argv[])
{
	bool	passed = true;

	cout << "HARDWARE TESTS\n";

	for (uint8_t i = 0; i < componentTestCount; i++) {
		passed = passed && componentTests[i].run();
	}

	if (!passed) {
		cerr << "HARDWARE TESTS FAILED\n";
		exit(1);
	}
	cout << "HARDWARE OK\n";
	return 0;
}
