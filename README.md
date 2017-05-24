## thsat

Tool for acquiring sensor data from a connected Arduino and storing it in an InfluxDB database.

### Running the App

The easiest way to run the application is by using Docker. To build the container, use the following command:

    docker build -t thsat .

Then to run the application within the container, use the following command:

    docker run -v /dev/ttyACM0:/dev/ttyACM0 thsat

Environment variables can be used to change application configuration.

### Building the App

The following items are required to build the application:

- CMake 3.2.0
- A C++ compiler that supports C++11
- Qt 5.4+ (with QtSerialPort)

To perform the build, use the following commands:

    mkdir build
    cd build
    cmake ..
    make
