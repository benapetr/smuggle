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
#include "apiqueryresult.hpp"
#include "syslog.hpp"
#include "wikisite.hpp"
#include "wikitool.hpp"

using namespace Smuggle;

static void UpdateMeta_Finish(Query *result)
{
    ApiQuery *x = (ApiQuery*)result;
    x->GetSite()->IsInitialized = true;
    x->DecRef();
}

static void UpdateMeta_Fail(Query *result)
{
    ApiQuery *x = (ApiQuery*)result;
    Syslog::Logs->ErrorLog("Failed to process meta information for " + x->GetSite()->Name + ": " + x->GetFailureReason());
    x->DecRef();
}

void WikiTool::UpdateMeta(WikiSite *wiki)
{
    if (!wiki)
        return;

    ApiQuery *query = new ApiQuery(ActionQuery, wiki);
    query->Parameters = "meta=siteinfo&siprop=" + QUrl::toPercentEncoding("namespaces|general");
    query->FailureCallback = (Callback)UpdateMeta_Fail;
    query->callback = (Callback)UpdateMeta_Finish;
    query->Process();
}
