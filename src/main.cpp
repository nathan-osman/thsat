/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Nathan Osman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QJsonDocument>

#include "arduino.h"
#include "buffer.h"
#include "influxdb.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QCommandLineOption arduinoDeviceOption(
        "arduino-device",
        "path to Arduino device",
        "path",
        "/dev/ttyACM0"
    );
    QCommandLineOption arduinoBaudOption(
        "arduino-baud",
        "baud rate for communication",
        "bps",
        "9600"
    );
    QCommandLineOption bufferDelayOption(
        "buffer-delay",
        "time between transmissions (in seconds)",
        "time",
        "300"
    );
    QCommandLineOption influxdbAddrOption(
        "influxdb-addr",
        "address of InfluxDB host",
        "addr",
        "http://localhost:8086"
    );
    QCommandLineOption influxdbDatabaseOption(
        "influxdb-database",
        "database to use for storage",
        "name",
        "thstat"
    );
    QCommandLineOption influxdbUsernameOption(
        "influxdb-username",
        "connection username",
        "value",
        ""
    );
    QCommandLineOption influxdbPasswordOption(
        "influxdb-password",
        "connection password",
        "value",
        ""
    );
    QCommandLineOption influxdbTagsOption(
        "influxdb-tags",
        "tags to add to each data point",
        "tags",
        ""
    );

    QCommandLineParser parser;
    parser.addOption(arduinoDeviceOption);
    parser.addOption(arduinoBaudOption);
    parser.addOption(bufferDelayOption);
    parser.addOption(influxdbAddrOption);
    parser.addOption(influxdbDatabaseOption);
    parser.addOption(influxdbUsernameOption);
    parser.addOption(influxdbPasswordOption);
    parser.addOption(influxdbTagsOption);
    parser.addHelpOption();
    if (!parser.parse(app.arguments())) {
        app.exit(1);
    }

    if (parser.isSet("help")) {
        parser.showHelp();
    }

    Arduino arduino(parser.value(arduinoDeviceOption), parser.value(arduinoBaudOption).toInt());
    Buffer buffer(parser.value(bufferDelayOption).toInt() * 1000);
    InfluxDB influxdb(
        parser.value(influxdbAddrOption),
        parser.value(influxdbDatabaseOption),
        parser.value(influxdbUsernameOption),
        parser.value(influxdbPasswordOption),
        parser.value(influxdbTagsOption)
    );

    QObject::connect(&arduino, &Arduino::dataReceived, &buffer, &Buffer::writeData);
    QObject::connect(&buffer, &Buffer::dataReceived, &influxdb, &InfluxDB::writeData);

    return app.exec();
}
