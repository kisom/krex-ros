#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <gflags/gflags.h>

#include <krex/DriveHealth.h>
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
DEFINE_validator(speed, &validateSpeed);


void
stop()
{
	cout << "ReleaseAll" << endl;
	mc.ReleaseAll();
}


int
main(int argc, char *argv[])
{
	uint8_t	speed;
	atexit(stop);
	gflags::ParseCommandLineFlags(&argc, &argv, true);
	speed = static_cast<uint8_t>(FLAGS_speed);

	cout << "forward\n";
	mc.Go(krex::somatic::Behaviour::Forward, speed);
	sleep(3);
	cout << "stop\n";
	mc.ReleaseAll();
	sleep(3);
	cout << "backward\n";
	mc.Go(krex::somatic::Behaviour::Backward, speed);
	sleep(3);
}
