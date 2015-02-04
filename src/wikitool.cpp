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
#include "swsql.hpp"
#include "wikisite.hpp"
#include "wikitool.hpp"

using namespace Smuggle;

static void UpdateMeta_Finish(Query *result)
{
    ApiQuery *x = (ApiQuery*)result;
    WikiSite *site = x->GetSite();
    QString SQL = "BEGIN;\n";
    SQL += "DELETE FROM namespaces WHERE wiki = " + QString::number(site->ID) + ";\n";
    QList<ApiQueryResultNode*> ns = x->GetApiQueryResult()->GetNodes("ns");
    if (ns.count() < 1)
    {
            Syslog::Logs->WarningLog(QString("Mediawiki of ") + x->GetSite()->Name + " provided no information about namespaces");
    } else
    {
        // let's prepare a NS list
        site->ClearNS();
        int index = 0;
        while (index < ns.count())
        {
            ApiQueryResultNode *node = ns.at(index);
            index++;
            if (!node->Attributes.contains("id") || !node->Attributes.contains("canonical"))
                continue;
            site->InsertNS(new WikiPageNS(node->GetAttribute("id").toInt(), node->Value, node->GetAttribute("canonical")));
            SQL += "INSERT INTO namespaces(wiki, namespace_id, name, canonical, is_talk) VALUES(" +
                    QString::number(site->ID) + ", '"
                    + node->GetAttribute("id") + "', '"
                    + SwSql::Escape(node->Value) + "', '"
                    + SwSql::Escape(node->GetAttribute("canonical")) + "', 0);\n";
        }
    }
    x->GetSite()->IsInitialized = true;
    SQL += QString("UPDATE wiki SET wiki_init = 1, time_init = strftime('%s','now')")
            + " WHERE id = "
            + QString::number(x->GetSite()->ID) + ";\nCOMMIT;";
    if (!x->GetSite()->Datafile->ExecuteNonQuery(SQL))
    {
        Syslog::Logs->ErrorLog("Unable to update table wiki: " + x->GetSite()->Datafile->LastError);
        Syslog::Logs->ErrorLog(SQL);
    }
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

    LOG("Updating information for " + wiki->Name + " please wait...");

    ApiQuery *query = new ApiQuery(ActionQuery, wiki);
    query->Parameters = "meta=siteinfo&siprop=" + QUrl::toPercentEncoding("namespaces|general");
    query->FailureCallback = (Callback)UpdateMeta_Fail;
    query->callback = (Callback)UpdateMeta_Finish;
    query->Process();
}


void WikiTool::DownloadPageFromTitle(WikiSite *wiki, QString title)
{

}
