#include <iostream>
#include <krex/sensory/mpu9250.h>

using namespace std;
using namespace krex::sensory;


int
main()
{
	MPU9250		mpu;
	double		temp;

	cout << "SELF TEST: WHO_AM_I? ";
	if (!mpu.Healthy()) {
		cerr << "FAILED" << endl;
		return 1;
	}

	cout << "OK\n";

	if (!mpu.Temperature(&temp)) {
		cerr << "failed to read temperature\n";
		return 1;
	}
	cout << "Temp: " << temp << " degC\n";
}
