#include <ros/ros.h>

#include <krex/Drive.h>
#include <krex/DriveHealth.h>
#include <krex/somatic/motorhat.h>


static krex::somatic::MotorHat	hat;


void
motorControl(const krex::Drive::ConstPtr &msg)
{
	ROS_INFO("received drive msg: behaviour=%d, speed=%d",
		       	(int)msg->behaviour, (int)msg->speed);
	if (!hat.Control(msg)) {
		ROS_ERROR("somatic-drive: couldn't behave as %d", (int)msg->behaviour);
	}
}


void
releaseAll()
{
	hat.ReleaseAll();
}


int
main(int argc, char *argv[])
{
	// If the drive process exits, try to stop the motors.
	atexit(releaseAll);

	ros::init(argc, argv, "somatic-drive");
	ROS_INFO("somatic/drive starting");

	ros::NodeHandle	node;
	ros::Subscriber	sub = node.subscribe("drive/control", 1, motorControl);
	ros::Publisher	pub = node.advertise<krex::DriveHealth>("drive/feedback", 1);

	// Publish health messages at most five times per second.
	ros::Rate	feedbackFrequency(5);

	ROS_INFO("somatic/drive ready");
	while (ros::ok()) {
		krex::DriveHealth	msg;
		hat.Health(&msg);

		pub.publish(msg);

		ros::spinOnce();
		feedbackFrequency.sleep();
	}

	ros::spin();
	hat.ReleaseAll();
	return 0;
}
