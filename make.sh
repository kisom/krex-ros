#!/usr/bin/env bash

set -eux

source /opt/ros/melodic/setup.bash
catkin_make "$@"
