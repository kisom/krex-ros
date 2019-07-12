# Drive stores a behavioural description of the drivetrain.
# This is currently a 4-motor system controlled by a single I2C
# device.
#
# This message may be sent to the motor control somatic process to
# request a given behaviour, and it should be sent regularly by
# the same process on a feedback channel to provide a status.
uint8 DRIVE_RELEASE=0 # use 0 as a default value
uint8 DRIVE_FORWARD=1
uint8 DRIVE_BACKWARD=2
uint8 DRIVE_TURN_LEFT=3
uint8 DRIVE_TURN_RIGHT=4

uint8 behaviour
uint8 speed
