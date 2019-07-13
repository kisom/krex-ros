#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <gflags/gflags.h>

#include <krex/Drive.h>
#include <krex/somatic/motorhat.h>

using namespace std;


static krex::somatic::MotorHat	mc;


static bool
validateSpeed(const char* flagname, uint32_t value)
{
	if (value < 256) {
		return true;
	}
	cerr << "Speed must be between 0 and 255." << endl;
	return false;
}

DEFINE_uint32(speed, 127, "drive speed (0-255, 255 is max speed)");
DEFINE_uint32(sleep, 3, "seconds to wait between transitions");
DEFINE_validator(speed, &validateSpeed);


void
stop()
{
	cout << "ReleaseAll" << endl;
	mc.ReleaseAll();
}


static void
drive(uint8_t direction, uint8_t speed, int sleepSeconds)
{
	if (!mc.Drive(direction, speed)) {
		cerr << "drive failed\n";
		exit(1);
	}
	sleep(sleepSeconds);
}


int
main(int argc, char *argv[])
{
	int	sleepSeconds;
	uint8_t	speed;

	atexit(stop);
	gflags::ParseCommandLineFlags(&argc, &argv, true);
	speed = static_cast<uint8_t>(FLAGS_speed);
	sleepSeconds = static_cast<int>(FLAGS_sleep);


	cout << "DRIVE TEST\nspeed=" << (int)speed << endl;

	cout << "forward\n";
	drive(krex::Drive::DRIVE_FORWARD, speed, sleepSeconds);

	cout << "turn right\n";
	drive(krex::Drive::DRIVE_TURN_RIGHT, speed, sleepSeconds);

	cout << "backwards\n";
	drive(krex::Drive::DRIVE_BACKWARD, speed, sleepSeconds);

	cout << "turn left\n";
	drive(krex::Drive::DRIVE_TURN_LEFT, speed, sleepSeconds);

	sleep(3);
}
