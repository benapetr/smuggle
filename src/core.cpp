//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#include "core.hpp"
#include "configuration.hpp"
#include "query.hpp"
#include "swsql.hpp"
#include "syslog.hpp"
#include "wikisite.hpp"
#include "generic.hpp"
#include "gc.hpp"
#include "sleeper.hpp"
#include <QApplication>
#include <QNetworkAccessManager>
#include <QDesktopServices>
#include <QDir>

using namespace Smuggle;

void Core::Shutdown()
{
    while (WikiSite::Sites.count() > 0)
        delete WikiSite::Sites.at(0);
    while (SwSql::Datafiles.count() > 0)
        delete SwSql::Datafiles.at(0);
    Query::NetworkManager->deleteLater();
    GC::gc->Stop();
    Syslog::Logs->Log("SHUTDOWN: waiting for garbage collector to finish");
    while(GC::gc->IsRunning())
        Sleeper::usleep(200);
    // last garbage removal
    GC::gc->DeleteOld();
    delete GC::gc;
    QApplication::exit();
}

void Core::Init()
{
#if QT_VERSION >= 0x050000
    Configuration::HomePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
    Configuration::HomePath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif

    GC::gc = new GC();

    if (!QDir().exists(Configuration::GetLocalDBPath()))
    {
        if (!QDir().mkpath(Configuration::GetLocalDBPath()))
        {
            Syslog::Logs->ErrorLog("Unable to create " + Configuration::GetLocalDBPath());
        }
    } else
    {
        // load all db files
    }
    if (SwSql::Default == NULL)
        SwSql::Default = new SwSql(Configuration::GetLocalDBPath() + "default.db");
    foreach(SwSql *file, SwSql::Datafiles)
        Core::LoadWikisFromDB(file);

    Query::NetworkManager = new QNetworkAccessManager();

    Syslog::Logs->Log("Welcome to Smuggle! :)");
}

void Core::LoadWikisFromDB(SwSql *file)
{
    SqlResult *result = file->ExecuteQuery("SELECT id, name, url, uw, uapis, ssl, wiki_init FROM wiki;");
    int x = 0;
    QString error;
    if (result->InError)
    {
        error = "Unable to read from " + file->GetPath();
        goto error_exit;
    }
    if (result->Count() == 0)
        goto exit;
    if (result->GetColumns() != 7)
    {
        error = "Wrong number of columns in wiki table";
        goto error_exit;
    }
    while (x < result->Count())
    {
        SwRow row = result->GetRow(x++);
        WikiSite *site = new WikiSite(file, row.GetField(1).toString(),
                                      row.GetField(2).toString(),
                                      row.GetField(3).toString(),
                                      row.GetField(4).toString(), false, false, "", "", "", false);
        site->ID = row.GetField(0).toInt();
        site->SupportHttps = Generic::SafeBool(row.GetField(5).toInt());
        site->IsInitialized = Generic::SafeBool(row.GetField(6).toInt());
        if (site->IsInitialized)
        {
            // get namespaces
            SqlResult *ns = file->ExecuteQuery("SELECT namespace_id, name, canonical FROM namespaces WHERE wiki = " +
                                               QString::number(site->ID) + ";");
            if (ns->InError)
            {
                Syslog::Logs->ErrorLog("Unable to read namespace information for " + site->Name + " " + file->LastError);
                delete ns;
                continue;
            }
            if (ns->Count() < 1)
            {
                delete ns;
                continue;
            }
            if (ns->GetColumns() != 3)
            {
                delete ns;
                Syslog::Logs->ErrorLog("Unable to read namespace information for " + site->Name + " wrong number of columns");
                continue;
            }
            int ns_id = 0;
            while (ns_id < ns->Count())
            {
                SwRow ns_info = ns->GetRow(ns_id++);
                site->InsertNS(new WikiPageNS(ns_info.GetField(0).toInt(),
                                              ns_info.GetField(1).toString(),
                                              ns_info.GetField(2).toString()));
            }
            delete ns;
        }
    }

    goto exit;

    error_exit:
        Syslog::Logs->ErrorLog(error);
    exit:
        delete result;
}


