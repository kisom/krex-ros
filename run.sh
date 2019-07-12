#!/usr/bin/env bash

set -eu

source /opt/ros/melodic/setup.bash

if [ ! -d devel ]
then
	catkin_make
fi

source ./devel/setup.bash

if [ "${1:-}" = "test" ]
then
	catkin_make
	./devel/lib/krex/hardware-test
fi

roslaunch krex krex.launch
