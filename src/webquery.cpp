//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#include "exception.hpp"
#include "syslog.hpp"
#include "webquery.hpp"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

using namespace Smuggle;

WebQuery::WebQuery()
{
    this->URL = "";
    this->reply = NULL;
    this->Parameters = "";
}

WebQuery::~WebQuery()
{
    if (this->reply != NULL)
    {
        QObject::disconnect(this->reply, SIGNAL(finished()), this, SLOT(Finished()));
        QObject::disconnect(this->reply, SIGNAL(readyRead()), this, SLOT(ReadData()));
        this->disconnect(this->reply);
        this->reply->abort();
        this->reply->disconnect(this);
        this->reply->deleteLater();
        this->reply = NULL;
    }
}

void WebQuery::Process()
{
    this->StartTime = QDateTime::currentDateTime();
    if (this->URL.isEmpty())
    {
        this->Result = new QueryResult(true);
        this->Result->SetError("You provided invalid url");
        this->Status = StatusInError;
        return;
    }
    this->ThrowOnValidResult();
    this->Status = StatusProcessing;
    this->Result = new QueryResult();

    QUrl url = QUrl::fromEncoded(this->URL.toUtf8());
    QNetworkRequest request(url);
    //if (this->UsingPOST)
    //{
    //    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    //    this->reply = Query::NetworkManager->post(request, this->Parameters.toUtf8());
    //} else
    //{
        this->reply = Query::NetworkManager->get(request);
    //}
    QObject::connect(this->reply, SIGNAL(finished()), this, SLOT(Finished()));
    QObject::connect(this->reply, SIGNAL(readyRead()), this, SLOT(ReadData()));
}

void WebQuery::Kill()
{

}

void WebQuery::ReadData()
{
    if (this->Status == StatusKilled)
        return;
    if (this->Result == NULL)
        throw new Smuggle::NullPointerException("loc WebserverQuery::Result", BOOST_CURRENT_FUNCTION);
    if (this->reply == NULL)
        throw new Smuggle::NullPointerException("loc WebserverQuery::reply", BOOST_CURRENT_FUNCTION);
    this->Result->Data += QString(this->reply->readAll());
}

void WebQuery::Finished()
{
    // don't even try to do anything if query was killed
    if (this->Status == StatusKilled)
        return;
    if (this->Result == NULL)
        throw new Smuggle::NullPointerException("loc WebserverQuery::Result", BOOST_CURRENT_FUNCTION);
    if (this->reply == NULL)
        throw new Smuggle::NullPointerException("loc WebserverQuery::reply", BOOST_CURRENT_FUNCTION);
    this->Result->Data += QString(this->reply->readAll());
    // now we need to check if request was successful or not
    if (this->reply->error())
    {
        this->Result->SetError(reply->errorString());
        this->reply->deleteLater();
        this->reply = NULL;
        this->Status = StatusDone;
        this->ProcessFailure();
        return;
    }
    this->reply->deleteLater();
    this->reply = NULL;
    this->Status = StatusDone;
    this->ProcessCallback();
}
