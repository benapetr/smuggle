//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

#ifndef APIQUERY_H
#define APIQUERY_H

#include "definitions.hpp"

#include <QString>
#include <QObject>
#include <QtNetwork>
#include "collectable_smartptr.hpp"
#include "query.hpp"
#include "mediawikiobject.hpp"

namespace Smuggle
{
    class ApiQueryResult;
    class WikiSite;

    enum Action
    {
        ActionClearHasMsg,
        ActionQuery,
        ActionLogin,
        ActionLogout,
        ActionTokens,
        ActionPurge,
        ActionRollback,
        ActionDelete,
        ActionUndelete,
        ActionBlock,
        ActionPatrol,
        ActionReview, // FlaggedRevs
        ActionProtect,
        ActionEdit,
        ActionUnwatch,
        ActionWatch
    };

    //! Format in which the result will be returned
    enum Format
    {
        XML,
        JSON,
        PlainText,
        Default
    };

    //! This class can be used to execute any kind of api query on any wiki
    class ApiQuery : public QObject, public Query, public MediaWikiObject
    {
            Q_OBJECT
        public:
            //! Creates a new instance of this class and set the defaults
            explicit ApiQuery();
            explicit ApiQuery(Action action);
            explicit ApiQuery(Action action, WikiSite *site);
            ~ApiQuery();
            Action GetAction();
            ApiQueryResult *GetApiQueryResult();
            //! Run
            void Process();
            //! Change the action type
            void SetAction(const Action action);
            //! Set the raw action type, you should not use this unless you have to
            void SetAction(const QString action);
            QString DebugURL();
            //! Terminate the query
            void Kill();
            //! Get a query target as a string
            QString QueryTargetToString();
            //! Returns a type of query as a string
            QString QueryTypeToString();
            bool EnforceLogin;
            //! Whether the query is going to edit any data in wiki
            bool EditingQuery;
            //! Whether the query will submit parameters using POST data
            bool UsingPOST;
            //! This is a requested format in which the result should be written in
            Format RequestFormat;
            bool IsContinuous;
            //! This is an url of api request, you probably don't want to change it unless
            //! you want to construct whole api request yourself
            QString URL;
            //! Parameters for action, for example page title
            QString Parameters;
            //! This is optional property which contains a label of target this query is for
            QString Target;
            //! You can change this to url of different wiki than a project
            QString OverrideWiki;
        private slots:
            void ReadData();
            void Finished();
        private:
            Action _action;
            //! Generate api url
            void ConstructUrl();
            QString ConstructParameterLessUrl();
            QString GetAssertPartSuffix();
            //! Check if return format is supported by huggle
            bool FormatIsCurrentlySupported();
            QString ActionPart;
            //! Reply from qnet
            QNetworkReply *reply;
    };

    inline bool ApiQuery::FormatIsCurrentlySupported()
    {
        // other formats will be supported later
        return (this->RequestFormat == XML);
    }

    inline void ApiQuery::SetAction(const QString action)
    {
        this->ActionPart = action;
    }

    inline QString ApiQuery::QueryTargetToString()
    {
        return this->Target;
    }

    inline QString ApiQuery::QueryTypeToString()
    {
        return "ApiQuery (" + this->ActionPart + ")";
    }
}

#endif // APIQUERY_H
