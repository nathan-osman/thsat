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

#include <QJsonDocument>

#include "arduino.h"

const int RetryInterval = 30 * 1000;

Arduino::Arduino(const QString &device, qint32 baudRate)
{
    connect(&mSerialPort, &QSerialPort::readyRead, this, &Arduino::onReadyRead);
    connect(&mSerialPort, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error), this, &Arduino::onError);
    connect(&mRetryTimer, &QTimer::timeout, this, &Arduino::onTimeout);

    mSerialPort.setPortName(device);
    mSerialPort.setBaudRate(baudRate);
    mRetryTimer.setSingleShot(true);

    init();
}

void Arduino::onReadyRead()
{
    mBuffer.append(mSerialPort.readAll());
    forever {
        int index = mBuffer.indexOf('\n');
        if (index == -1) {
            break;
        }
        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(mBuffer, &error);
        if (error.error == QJsonParseError::NoError) {
            emit dataReceived(document.object());
        }
        mBuffer.remove(0, index + 1);
    }
}

void Arduino::onError(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError) {
        qCritical("communication error, retrying shortly");
        mRetryTimer.start(RetryInterval);
    }
}

void Arduino::onTimeout()
{
    init();
}

void Arduino::init()
{
    mBuffer.clear();
    if (!mSerialPort.open(QIODevice::ReadOnly)) {
        qCritical("unable to connect; retrying shortly");
        mRetryTimer.start(RetryInterval);
    }
}
