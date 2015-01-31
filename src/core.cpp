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
#include "swsql.hpp"
#include "syslog.hpp"
#include "wikisite.hpp"
#include <QApplication>
#include <QDesktopServices>
#include <QDir>

using namespace Smuggle;

void Core::Shutdown()
{
    while (WikiSite::Sites.count() > 0)
        delete WikiSite::Sites.at(0);
    while (SwSql::Datafiles.count() > 0)
        delete SwSql::Datafiles.at(0);
    QApplication::exit();
}

void Core::Init()
{
#if QT_VERSION >= 0x050000
    Configuration::HomePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
    Configuration::HomePath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif

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
    Syslog::Logs->Log("Welcome to Smuggle! :)");
}

void Core::LoadWikisFromDB(SwSql *file)
{

}


