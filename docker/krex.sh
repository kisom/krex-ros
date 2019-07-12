#!/usr/bin/env bash

source /opt/ros/melodic/setup.bash
catkin_make


case "${1:-}" in
	run)
		bash /krex/run.sh ;;
	bash)
		bash ;;
	*) ;;
esac
