### Hi there, welcome to MITL

### What is MITL?
MITL is a little "autopilot" I'm building, and is heavily inspired by px4. 

### Goal
This more of a learning project, but I think the idea is pretty cool. I want MITL to have a mavlink communication with a GCS, and run the control loops by borrowing physics and sensor data from gazebo through the gz_x500 vehicle. The main purpose is to log and benchmark the performance of different control loops (PID, cascaded, ADRC, etc), by allowing them to be swappable at compile time and run time! This project is still in the works, but I thought I'd lay it out so you have some idea of what I'm working on.

### MITL?
From the description, you can tell this is more of a simulator than an autopilot, which is true. This isn't going on actual firmware any time soon... This is why I call it MITL, short for "Mulla In The Loop".

### Dependencies

-[MAVSDK](https://mavsdk.mavlink.io/main/en/cpp/guide/installation.html)

-[gz_transport](https://gazebosim.org/api/transport/15/installation.html)

-[gz_msgs](https://gazebosim.org/api/msgs/11/install.html)