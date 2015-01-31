//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#include "apiquery.hpp"
#include <QFile>
#include <QUrl>
#include "apiqueryresult.hpp"
#include "configuration.hpp"
#include "exception.hpp"
#include "wikisite.hpp"

using namespace Smuggle;

void ApiQuery::ConstructUrl()
{
    //if (this->ActionPart.isEmpty())
    //    throw new Huggle::Exception("No action provided for api request", BOOST_CURRENT_FUNCTION);
    if (this->OverrideWiki.isEmpty())
    {
    //    this->URL = Configuration::GetProjectScriptURL(this->GetSite()) + "api.php?action=" + this->ActionPart;
    } else
    {
    //    this->URL = Configuration::GetURLProtocolPrefix(this->GetSite()) + this->OverrideWiki + "api.php?action=" + this->ActionPart;
    }
    if (this->Parameters.length() > 0)
        this->URL += "&" + this->Parameters;
    if (this->IsContinuous)
        this->URL += "&rawcontinue=1";
    switch (this->RequestFormat)
    {
        case XML:
            this->URL += "&format=xml";
            break;
        case JSON:
            this->URL += "&format=json";
            break;
        case PlainText:
        case Default:
            break;
    }
    this->URL += this->GetAssertPartSuffix();
}

QString ApiQuery::ConstructParameterLessUrl()
{
    QString url;
    if (this->ActionPart.isEmpty())
    {
    //    throw new Huggle::Exception("No action provided for api request", BOOST_CURRENT_FUNCTION);
    }
   // if (!this->OverrideWiki.size())
   //     url = Configuration::GetProjectScriptURL(this->GetSite()) + "api.php?action=" + this->ActionPart;
   // else
   //     url = Configuration::GetURLProtocolPrefix(this->GetSite()) + this->OverrideWiki + "api.php?action=" + this->ActionPart;
    if (this->IsContinuous)
        url += "&rawcontinue=1";
    switch (this->RequestFormat)
    {
        case XML:
            url += "&format=xml";
            break;
        case JSON:
            url += "&format=json";
            break;
        case PlainText:
        case Default:
            break;
    }
    return url + this->GetAssertPartSuffix();
}

QString ApiQuery::GetAssertPartSuffix()
{
    return "";
}

ApiQuery::ApiQuery()
{
    this->RequestFormat = XML;
    this->Type = QueryApi;
}

ApiQuery::ApiQuery(Action action)
{
    this->RequestFormat = XML;
    this->Type = QueryApi;
    this->SetAction(action);
}

ApiQuery::ApiQuery(Action action, WikiSite *site)
{
    this->RequestFormat = XML;
    this->Site = site;
    this->Type = QueryApi;
    this->SetAction(action);
}

ApiQuery::~ApiQuery()
{
    this->Kill();
}

Action ApiQuery::GetAction()
{
    return this->_action;
}

ApiQueryResult *ApiQuery::GetApiQueryResult()
{
    return (ApiQueryResult*)this->Result;
}

void ApiQuery::Finished()
{
    // don't even try to do anything if query was killed
    if (this->Status == StatusKilled)
        return;
    //if (this->Result == nullptr)
    //    throw new Huggle::NullPointerException("loc ApiQuery::Result", BOOST_CURRENT_FUNCTION);
    //if (this->reply == nullptr)
    //    throw new Huggle::NullPointerException("loc ApiQuery::reply", BOOST_CURRENT_FUNCTION);
    ApiQueryResult *result = (ApiQueryResult*)this->Result;
    result->Data += QString(this->reply->readAll());
    // now we need to check if request was successful or not
    if (this->reply->error())
    {
        this->Result->SetError(HUGGLE_EUNKNOWN, this->reply->errorString());
        this->reply->deleteLater();
        this->reply = NULL;
        this->Status = StatusDone;
        this->ProcessFailure();
        return;
    }
    this->reply->deleteLater();
    this->reply = NULL;
    if (!result->IsFailed() && this->RequestFormat == XML)
        result->Process();
    this->Status = StatusDone;
    this->ProcessCallback();
}

void ApiQuery::Process()
{
    if (this->Status != Smuggle::StatusNull)
    {
        return;
    }
    this->StartTime = QDateTime::currentDateTime();
    this->ThrowOnValidResult();
    if (!this->URL.size())
        this->ConstructUrl();
    this->Status = StatusProcessing;
    this->Result = new ApiQueryResult();
    QUrl url;
    if (this->UsingPOST)
    {
        this->URL = this->ConstructParameterLessUrl();
        url = QUrl::fromEncoded(this->URL.toUtf8());
    } else
    {
        url = QUrl::fromEncoded(this->URL.toUtf8());
    }
    QNetworkRequest request(url);
    //request.setRawHeader("User-Agent", Configuration::HuggleConfiguration->WebqueryAgent);
    if (this->UsingPOST)
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    if (this->UsingPOST)
    {
        this->reply = Query::NetworkManager->post(request, this->Parameters.toUtf8());
    } else
    {
        this->reply = Query::NetworkManager->get(request);
    }
    QObject::connect(this->reply, SIGNAL(finished()), this, SLOT(Finished()));
    QObject::connect(this->reply, SIGNAL(readyRead()), this, SLOT(ReadData()));
}

void ApiQuery::Kill()
{
    if (this->reply != NULL)
    {
        QObject::disconnect(this->reply, SIGNAL(finished()), this, SLOT(Finished()));
        QObject::disconnect(this->reply, SIGNAL(readyRead()), this, SLOT(ReadData()));
        if (this->Status == StatusProcessing)
        {
            this->Status = StatusKilled;
            this->disconnect(this->reply);
            this->reply->abort();
            this->reply->disconnect(this);
            this->reply->deleteLater();
            this->reply = NULL;
        }
    }
}

void ApiQuery::ReadData()
{
    // don't even try to do anything if query was killed
    if (this->Status == StatusKilled)
        return;
    //if (this->Result == nullptr)
    //    throw new Huggle::NullPointerException("loc ApiQuery::Result", BOOST_CURRENT_FUNCTION);
    //if (this->reply == nullptr)
    //    throw new Huggle::NullPointerException("loc ApiQuery::reply", BOOST_CURRENT_FUNCTION);
    this->Result->Data += QString(this->reply->readAll());
}

void ApiQuery::SetAction(const Action action)
{
    this->_action = action;
    switch (action)
    {
        case ActionClearHasMsg:
            this->ActionPart = "clearhasmsg";
            return;
        case ActionQuery:
            this->ActionPart = "query";
            this->IsContinuous = true;
            this->EnforceLogin = false;
            return;
        case ActionLogin:
            this->ActionPart = "login";
            this->EnforceLogin = false;
            return;
        case ActionLogout:
            this->ActionPart = "logout";
            return;
        case ActionTokens:
            this->ActionPart = "tokens";
            return;
        case ActionPurge:
            this->ActionPart = "purge";
            return;
        case ActionRollback:
            this->ActionPart = "rollback";
            this->EditingQuery = true;
            return;
        case ActionDelete:
            this->ActionPart = "delete";
            this->EditingQuery = true;
            return;
        case ActionUndelete:
            this->ActionPart = "undelete";
            this->EditingQuery = true;
            return;
        case ActionBlock:
            this->ActionPart = "block";
            this->EditingQuery = true;
            return;
        case ActionProtect:
            this->ActionPart = "protect";
            this->EditingQuery = true;
            return;
        case ActionEdit:
            this->ActionPart = "edit";
            this->EditingQuery = true;
            return;
        case ActionPatrol:
            this->ActionPart = "patrol";
            this->EditingQuery = true;
            return;
        case ActionReview: // FlaggedRevs
            this->ActionPart = "review";
            this->EditingQuery = true;
            return;
        case ActionUnwatch:
        case ActionWatch:
            this->ActionPart = "watch";
            this->EditingQuery = true;
            return;
    }
}

QString ApiQuery::DebugURL()
{
    if (this->HiddenQuery)
        return "Protected link";
    if (this->UsingPOST)
        return this->URL + " POST: " + this->Parameters;
    return this->URL;
}
