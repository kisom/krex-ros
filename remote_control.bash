source devel/setup.bash

drive () {
	DIRECTION=$1
	SPEED="${SPEED:-255}"
	rostopic pub --once /somatic/drive/control krex/Drive ${DIRECTION} ${SPEED}
}

drive_stop () {
	drive 0 
}

drive_forward () {
	drive 1
}

drive_backward () {
	drive 2
}

turn_left () {
	drive 3
}

turn_right () {
	drive 4
}


