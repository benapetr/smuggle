//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#ifndef RETRIEVEQUERY_H
#define RETRIEVEQUERY_H

#include "query.hpp"
#include "mediawikiobject.hpp"
#include <QString>

namespace Smuggle
{
    class ApiQuery;
    class WebQuery;
    class WikiSite;
    class RetrieveQuery : public Query, public MediaWikiObject
    {
        public:
            RetrieveQuery(WikiSite *site, QString page);
            ~RetrieveQuery();
            bool IsProcessed();
            QString GetFailureReason();
            void SetError(QString text);
            void Process();
            void RetrieveHtml();
            void Finish();
            bool IsFailed();
            QString PageName();
            QString revid_text;
        private:
            QString error;
            ApiQuery *qText;
            WebQuery *qOriginal;
            QString title;
    };
}

#endif // RETRIEVEQUERY_H
