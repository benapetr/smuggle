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
#include "configuration.hpp"
#include "retrievequery.hpp"
#include "swsql.hpp"
#include "syslog.hpp"
#include "wikisite.hpp"

using namespace Smuggle;

RetrieveQuery::RetrieveQuery(WikiSite *site, QString page) : MediaWikiObject(site)
{
    this->title = page;
    this->qText = NULL;
}

RetrieveQuery::~RetrieveQuery()
{
    GC_DECREF(this->qText);
}

bool RetrieveQuery::IsProcessed()
{
    if (!this->qText)
        return false;

    if (!this->qText->IsProcessed())
        return false;

    return true;
}

QString RetrieveQuery::GetFailureReason()
{
    if (!this->error.isEmpty())
        return this->error;

    if (this->qText && this->qText->IsFailed())
        return this->qText->GetFailureReason();

    return "";
}

void RetrieveQuery::SetError(QString text)
{
    this->error = text;
    this->ProcessFailure();
}

static void FinishText(Query *query)
{
    ApiQuery *x = (ApiQuery*)query;
    RetrieveQuery *q = (RetrieveQuery*) x->CallbackOwner;
    ApiQueryResultNode *revision = x->GetApiQueryResult()->GetNode("rev");
    QString revid, user, summary, timestamp, content;
    QString model;
    SqlResult *sql = NULL;
    QStringList pos;
    QString SQL = "BEGIN;\n";
    if (!revision)
    {
        q->SetError("No revision was returned by wiki");
        goto exit;
    }
    // let's update all data for this page
    revid = revision->GetAttribute("revid", "-1");
    if (revid == "-1")
    {
        q->SetError("No revision was returned by wiki");
        goto exit;
    }
    user = revision->GetAttribute("user");
    timestamp = revision->GetAttribute("timestamp");
    content = revision->GetAttribute("contenformat");
    model = revision->GetAttribute("contentmodel");
    int page_id;
    // Check if there is already this revision in DB
    sql = q->GetSite()->Datafile->ExecuteQuery("SELECT rev_id FROM revision WHERE wiki = " + QString::number(q->GetSite()->ID) + " AND rev_id = " +
                                               revid + ";");
    if (sql->InError)
    {
        q->SetError("Unable to select revision from revision table: " + q->GetSite()->Datafile->LastError);
        goto exit;
    }
    if (sql->Count() > 0)
    {
        // we already got this revision in db
        goto exit;
    }
    // Get information about the page from DB
    delete sql;
    sql = q->GetSite()->Datafile->ExecuteQuery_Bind("SELECT id FROM page WHERE wiki = "  + QString::number(q->GetSite()->ID) + " AND name = ?",
                                                    q->PageName());

    if (sql->InError)
    {
        q->SetError("Unable to select page from page table: " + q->GetSite()->Datafile->LastError);
        goto exit;
    }

    if (sql->Count() != 1)
    {
        q->SetError("Unable to select page from page table: multiple rows returned");
        goto exit;
    }

    page_id = sql->GetRow(0).GetField(0).toInt();

    summary = revision->GetAttribute("comment");

    // let's insert to text table
    pos << revision->Value;
    pos << summary; // summary is second parameter to that sql
    SQL += "INSERT INTO text (wiki, revid, text, html) VALUES (" +
                 QString::number(q->GetSite()->ID) + ", " +
                 revid + ", ?, '');\n";

    if (sql->InError)
    {
        q->SetError("Unable to insert into text table: " + q->GetSite()->Datafile->LastError);
        goto exit;
    }

    SQL += "INSERT INTO revision(rev_id, rev_page, rev_text_id, rev_comment, rev_user_text, rev_timestamp, rev_minor_edit, rev_deleted, rev_len,"\
            "rev_parent_id, rev_sha1, rev_content_model, rev_content_format, wiki, downloaded)"\
            "VALUES (" + revid // revid
            + ", " + QString::number(page_id) // rev_page
            + ", last_insert_rowid(), ?, '', 0, 0, 0, 0, 0, '', '" + SwSql::Escape(model) + "', '" + SwSql::Escape(content) + "', "
            + QString::number(q->GetSite()->ID) + ", 1 );";

    // Commit and run
    SQL += "COMMIT;";
    delete sql;
    sql = q->GetSite()->Datafile->ExecuteQuery_Bind(SQL, pos);
    if (sql->InError)
    {
        DEBUG(SQL, 2);
        q->SetError("Unable to run statement: " + q->GetSite()->Datafile->LastError);
        goto exit;
    }

    q->Finish();
    exit:
    if (sql)
        delete sql;
    x->DecRef();
}

static void ErrorText(Query *query)
{
    ApiQuery *x = (ApiQuery*)query;
    ((RetrieveQuery*) x->CallbackOwner)->SetError(x->GetFailureReason());
    x->DecRef();
}

void RetrieveQuery::Process()
{
    this->qText = new ApiQuery(ActionQuery, this->GetSite());
    this->qText->Parameters = "prop=revisions&titles=" + QUrl::toPercentEncoding(this->title) + "&rvprop=ids|timestamp|user|comment|content&rvlimit=1";
    this->qText->CallbackOwner = this;
    this->qText->FailureCallback = (Callback) ErrorText;
    this->qText->callback = (Callback) FinishText;
    this->qText->Process();
}

void RetrieveQuery::Finish()
{
    this->ProcessCallback();
}

bool RetrieveQuery::IsFailed()
{
    if (!this->error.isEmpty())
        return true;
    if (this->qText && this->qText->IsFailed())
        return true;

    return false;
}

QString RetrieveQuery::PageName()
{
    return this->title;
}

