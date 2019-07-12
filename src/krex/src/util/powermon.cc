#include <unistd.h>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <gflags/gflags.h>
#include <krex/util/util.h>

using namespace std;


DEFINE_uint32(delay, 10, "delay in ms between readings");


const string	CurrentInput = "/sys/bus/i2c/devices/6-0040/iio_device/in_current0_input";
const string	PowerInput = "/sys/bus/i2c/devices/6-0040/iio_device/in_power0_input";
const string	TemperatureInput = "/sys/devices/virtual/thermal/thermal_zone1/temp";
const string	VoltageInput = "/sys/bus/i2c/devices/6-0040/iio_device/in_voltage0_input";


struct {

	uint32_t	temperature;
	uint16_t	current;
	uint16_t	power;
	uint16_t	voltage;
} PowerState;


static void
takeSnapshot(ostream &results)
{
	ifstream	CurrentFile(CurrentInput);
	ifstream	PowerFile(PowerInput);
	ifstream	TemperatureFile(TemperatureInput);
	ifstream	VoltageFile(VoltageInput);

	CurrentFile >> PowerState.current;
	PowerFile >> PowerState.power;
	VoltageFile >> PowerState.voltage;
	TemperatureFile >> PowerState.temperature;

	results << PowerState.current << ",";
	results << PowerState.power   << ",";
	results << PowerState.voltage << ",";
	results << PowerState.temperature << std::endl;
	results.flush();
	sync();
}

int
main(int argc, char *argv[])
{
	uint32_t	delay;
	ofstream	out;
	bool		console = true;

	gflags::ParseCommandLineFlags(&argc, &argv, true);
	delay = FLAGS_delay;

	if (argc > 1) {
		console = false;
		out.open(argv[1], std::ofstream::out|std::ofstream::app);
	}

	while (true) {
		if (console) {
			takeSnapshot(cout);
		}
		else {
			takeSnapshot(out);
		}

		krex::util::Sleep(delay);
	}
}
