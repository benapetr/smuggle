//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#ifndef QUERYPOOL_HPP
#define QUERYPOOL_HPP

#include "definitions.hpp"

#include <QList>

namespace Smuggle
{
    class ApiQuery;
    class EditQuery;
    class Message;
    class WikiEdit;
    class Query;

    //! Pool of all queries that are monitored by huggle itself

    //! Not every query is contained here, only these that are managed by something
    //! it's generally a good idea to insert every query to this pool
    class QueryPool
    {
        public:
            static QueryPool *SmugglePool;
            QueryPool();
            ~QueryPool();

            /*!
             * \brief Insert a query to internal list of running queries, so that they can be watched
             * This will insert it to a process list in main form
             * \param item Query that is about to be inserted to list of running queries
             */
            void AppendQuery(Query* item);
            void CheckQueries();
            int RunningQueriesGetCount();
        private:
            //! List of all running queries
            QList<Query*> RunningQueries;
    };
}

#endif // QUERYPOOL_HPP
