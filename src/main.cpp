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

    QCommandLineParser parser;
    parser.addOption(arduinoDeviceOption);
    parser.addOption(arduinoBaudOption);
    parser.addHelpOption();
    if (!parser.parse(app.arguments())) {
        app.exit(1);
    }

    if (parser.isSet("help")) {
        parser.showHelp();
    }

    Arduino arduino(parser.value(arduinoDeviceOption), parser.value(arduinoBaudOption).toInt());
    QObject::connect(&arduino, &Arduino::dataReceived, [](const QJsonObject &object) {
        qDebug("Data received: %s", QJsonDocument(object).toJson().constData());
    });

    return app.exec();
}