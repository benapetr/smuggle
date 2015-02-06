//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#ifndef WEBQUERY_HPP
#define WEBQUERY_HPP

#include "query.hpp"
#include <QString>

class QNetworkReply;

namespace Smuggle
{
    class WebQuery : public QObject, public Query
    {
            Q_OBJECT
        public:
            WebQuery();
            ~WebQuery();
            //! This is an url of api request, you probably don't want to change it unless
            //! you want to construct whole api request yourself
            QString URL;
            //! Parameters for action, for example page title
            QString Parameters;
            //! Run
            void Process();
            //! Terminate the query
            void Kill();
        private:
            QNetworkReply *reply;
        private slots:
            void ReadData();
            void Finished();
    };
}

#endif // WEBQUERY_HPP
