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
#include "generic.hpp"
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

// this macro makes it easy to run SQL statement and do rollback on failure
// this one rollback if query fails with a given error reason
#define SMUGGLE_ASSERT_ROLLBACK(query, error) if (!q->GetSite()->Datafile->ExecuteNonQuery(query))\
                                              {\
                                                  q->SetError(QString(error) + ": " + q->GetSite()->Datafile->LastError);\
                                                  goto do_rollback;\
                                              }

// this one rollbacks if query with bound values fail with a given reason
#define SMUGGLE_ASSERT_BOUND_ROLLBACK(query, error, value) delete sql;\
                                                    sql = q->GetSite()->Datafile->ExecuteQuery_Bind(query, value);\
                                                    if (sql->InError)\
                                                    {\
                                                        q->SetError(QString(error) + ": " + q->GetSite()->Datafile->LastError);\
                                                        goto do_rollback;\
                                                    }

static void FinishText(Query *query)
{
    ApiQuery *x = (ApiQuery*)query;
    RetrieveQuery *q = (RetrieveQuery*) x->CallbackOwner;
    ApiQueryResultNode *revision = x->GetApiQueryResult()->GetNode("rev");
    QString revid, user, summary, timestamp, content, sid;
    QString model;
    SqlResult *sql = NULL;
    bool precached = false;
    QString SQL;
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
    // precache this
    sid = QString::number(q->GetSite()->ID);
    int page_id;
    // Check if there is already this revision in DB
    sql = q->GetSite()->Datafile->ExecuteQuery("SELECT rev_id, downloaded FROM revision WHERE wiki = " + sid + " AND rev_id = " +
                                               revid + ";");
    if (sql->InError)
    {
        q->SetError("Unable to select revision from revision table: " + q->GetSite()->Datafile->LastError);
        goto exit;
    }
    // if this revision is already downloaded
    if (sql->Count() > 0)
    {
        if (Generic::SafeBool(sql->GetRow(0).GetField(1).toInt()))
            goto exit;
        else
            precached = true;
    }
    // Get information about the page from DB
    delete sql;
    sql = q->GetSite()->Datafile->ExecuteQuery_Bind("SELECT id FROM page WHERE wiki = "  + sid + " AND name = ?",
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

    // start transaction
    SMUGGLE_ASSERT_ROLLBACK("BEGIN;", "Unable to start a transaction");

    // let's insert to text table
    SQL = "INSERT INTO text (wiki, revid, text, html) VALUES (" + sid + ", " + revid + ", ?, '');";
    SMUGGLE_ASSERT_BOUND_ROLLBACK(SQL, "Unable to insert to text table", revision->Value);

    if (!precached)
    {
        // insert into revision table
        SQL = "INSERT INTO revision(rev_id, rev_page, rev_text_id, rev_comment, rev_user_text, rev_timestamp, rev_minor_edit, rev_deleted, rev_len,"\
                "rev_parent_id, rev_sha1, rev_content_model, rev_content_format, wiki, downloaded)"\
                "VALUES (" + revid // revid
                + ", " + QString::number(page_id) // rev_page
                + ", last_insert_rowid(), ?, '', 0, 0, 0, 0, 0, '', '" + SwSql::Escape(model) + "', '" + SwSql::Escape(content) + "', "
                + sid + ", 1 );";
        SMUGGLE_ASSERT_BOUND_ROLLBACK(SQL, "Unable to insert to revision table", summary);
    } else
    {
        // update the revision table
        SQL = "UPDATE revision SET downloaded = 1, rev_text_id = last_insert_rowid(), rev_content_model = '" + SwSql::Escape(model) + "', "\
              "rev_content_format = '" + SwSql::Escape(content) + "' WHERE wiki = " + sid + " AND rev_id = " + revid + ";";
        SMUGGLE_ASSERT_BOUND_ROLLBACK(SQL, "Unable to update revision table", summary);
    }

    // now we need to update the page table
    SQL = "UPDATE page SET preferred_rev_id = " + revid + ", current_revision = " + revid + ", last_revision = " + revid + ", " +
          "downloaded = 1, deleted = 0, wiki = " + sid + ", last_updated = strftime('%s','now')" +
          " WHERE " + "id = " + QString::number(page_id) + ";";
    SMUGGLE_ASSERT_ROLLBACK(SQL, "Unable to update the page table");
    SMUGGLE_ASSERT_ROLLBACK("COMMIT;", "Unable to commit");
    q->Finish();
    goto exit;

    do_rollback:
        if (!q->GetSite()->Datafile->ExecuteNonQuery("ROLLBACK;"))
        {
            Syslog::Logs->ErrorLog("Rollback failure: " + q->GetSite()->Datafile->LastError);
            goto exit;
        }

    exit:
        // do some memory cleanup here
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
    this->qText->IncRef();
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

