//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#include "querypool.hpp"
#include <QtXml>
#include "configuration.hpp"
#include "exception.hpp"
#include "query.hpp"
#include "wikisite.hpp"

using namespace Smuggle;

QueryPool *QueryPool::SmugglePool = NULL;

QueryPool::QueryPool()
{

}

QueryPool::~QueryPool()
{
    while(this->RunningQueries.count() != 0)
    {
        this->RunningQueries.at(0)->DecRef();
        this->RunningQueries.removeAt(0);
    }
}

void QueryPool::AppendQuery(Query *item)
{
    item->IncRef();
    this->RunningQueries.append(item);
}

void QueryPool::CheckQueries()
{
    int curr = 0;
    while (curr < this->RunningQueries.count())
    {
        Query *q = this->RunningQueries.at(curr);
        if (q->IsProcessed())
        {
            this->RunningQueries.removeAt(curr);
            q->DecRef();
        } else
        {
            curr++;
        }
    }
}

int QueryPool::RunningQueriesGetCount()
{
    return this->RunningQueries.count();
}

