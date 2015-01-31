//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#include "swsql.hpp"
#include "exception.hpp"
#include "syslog.hpp"
#include <QFile>

using namespace Smuggle;

SwSql *SwSql::Default = NULL;
QList<SwSql*> SwSql::Datafiles;

QString SwSql::GetSQL(QString name)
{
    QFile *f = new QFile(":/text/sql/" + name);
    if (!f->open(QIODevice::ReadOnly))
    {
        throw new Smuggle::Exception("Failed to open " + name);
    }
    QString d = f->readAll();
    f->close();
    delete f;
    return d;
}

SwSql::SwSql(QString path)
{
    this->file = path;
    SwSql::Datafiles.append(this);
    bool init = !QFile().exists(this->file);
    int rc = sqlite3_open(this->file.toUtf8().data(), &this->db);
    if (rc != 0)
    {
        Syslog::Logs->ErrorLog("Unable to open " + this->file + " error code " + QString::number(rc));
        return;
    }
    if (init && !this->ExecuteNonQueryLineByLine(GetSQL("init.sql")))
    {
        Syslog::Logs->ErrorLog("Failed to initialize " + this->file);
    }
}

SwSql::~SwSql()
{
    SwSql::Datafiles.removeAll(this);
    sqlite3_close(this->db);
}

QString SwSql::GetPath()
{
    return this->file;
}

bool SwSql::ExecuteNonQueryLineByLine(QString sql)
{
    QStringList lines = sql.split('\n');
    while (lines.count())
    {
        if (lines.at(0).trimmed().startsWith("--"))
            goto next;
        if (lines.at(0).trimmed().isEmpty())
            goto next;
        if (!this->ExecuteNonQuery(lines.at(0)))
            return false;
        next:
        lines.removeAt(0);
    }
    return true;
}

bool SwSql::ExecuteNonQuery(QString sql)
{
    sqlite3_stmt *statement;
    int x = sqlite3_prepare_v2(this->db, sql.toUtf8().constData(), sql.length() + 1, &statement, NULL);
    if (!this->Evaluate(x))
        return false;
    x = sqlite3_step(statement);
    return this->Evaluate(x);
}

bool SwSql::Evaluate(int data)
{
    switch (data)
    {
        case SQLITE_OK:
            return true;
        case SQLITE_DONE:
            return true;
    }
    Syslog::Logs->ErrorLog("Unknown result");
    return false;
}
