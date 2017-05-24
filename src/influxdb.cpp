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
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>

#include "influxdb.h"

InfluxDB::InfluxDB(const QString &addr, const QString &database, const QString &username, const QString &password, const QString &tags)
    : mAddr(addr),
      mDatabase(database),
      mUsername(username),
      mPassword(password),
      mTags(tags.length() ? "," + tags : "")
{
    connect(&mNetworkAccessManager, &QNetworkAccessManager::finished, this, &InfluxDB::onFinished);

    init();
}

void InfluxDB::writeData(const QJsonObject &object)
{
    QUrlQuery query;
    query.addQueryItem("db", mDatabase);
    query.addQueryItem("u", mUsername);
    query.addQueryItem("p", mPassword);
    QUrl url(mAddr);
    url.setPath("/write");
    url.setQuery(query);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    mNetworkAccessManager.post(request, generateData(object));
}

void InfluxDB::onFinished(QNetworkReply *reply)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode != 200 && statusCode != 204) {
        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error == QJsonParseError::NoError) {
            qCritical("received error: %s", document.object().value("error").toString().toUtf8().constData());
        } else {
            qCritical("received status code: %d", statusCode);
        }
    }
    reply->deleteLater();
}

void InfluxDB::init()
{
    QUrl url(mAddr);
    url.setPath("/query");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QUrlQuery query;
    query.addQueryItem("q", QString("CREATE DATABASE %1").arg(mDatabase));
    mNetworkAccessManager.post(request, query.toString(QUrl::FullyEncoded).toUtf8());
}

QByteArray InfluxDB::generateData(const QJsonObject &object) const
{
    QByteArray data;
    for (auto i = object.constBegin(); i != object.constEnd(); ++i) {
        data.append(i.key() + mTags + " value=" + i.value().toString() + "\n");
    }
    return data;
}
