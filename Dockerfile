FROM nathanosman/qt
MAINTAINER Nathan Osman <nathan@quickmediasolutions.com>

# Install QtSerialPort
RUN \
    apt-get update && \
    apt-get install -y libqt5serialport5-dev && \
    rm -rf /var/lib/apt/lists/*

# Copy the source code to the container
COPY CMakeLists.txt /root/
COPY src /root/src

# Configure and build the project
RUN \
    cd /root && \
    cmake . && \
    make && \
    make install

# Set runtime defaults
ENV \
    ARDUINO_DEVICE=/dev/ttyACM0 \
    ARDUINO_BAUD=9600 \
    INFLUXDB_ADDR=http://localhost:8086 \
    INFLUXDB_DATABASE=thstat \
    INFLUXDB_USERNAME= \
    INFLUXDB_PASSWORD= \
    INFLUXDB_TAGS=

# Set the command for running the application
CMD sechatr \
    --arduino-device "$ARDUINO_DEVICE" \
    --arduino-baud "$ARDUINO_BAUD" \
    --influxdb-addr "$INFLUXDB_ADDR" \
    --influxdb-database "$INFLUXDB_DATABASE" \
    --influxdb-username "$INFLUXDB_USERNAME" \
    --influxdb-password "$INFLUXDB_PASSWORD" \
    --influxdb-tags "$INFLUXDB_TAGS"
