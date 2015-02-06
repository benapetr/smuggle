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
#include <string.h>

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

QString SwSql::Escape(QString input)
{
    return input.replace("'", "''");
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
    if (init)
    {
        Syslog::Logs->Log("Initializing " + this->file);
        if(!this->ExecuteNonQuery(GetSQL("init.sql")))
        {
            Syslog::Logs->ErrorLog("Failed to initialize " + this->file);
        }
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

qint64 SwSql::LastRow()
{
    return sqlite3_last_insert_rowid(this->db);
}

bool SwSql::ExecuteNonQuery(QString sql)
{
    char *error;
    this->LastStatement = sql;
    int x = sqlite3_exec(this->db, sql.toUtf8().constData(), NULL, NULL, &error);
    if (!this->Evaluate(x))
    {
        this->LastError = QString(error);
        return false;
    }
    return true;
}

static QString StringFromUnsignedChar( const unsigned char *str )
{
    std::string temp = std::string(reinterpret_cast<const char*>(str));
    return QString::fromUtf8(temp.c_str());
}

SqlResult *SwSql::ExecuteQuery(QString sql)
{
    return this->ExecuteQuery_Bind(sql, QStringList());
}

SqlResult *SwSql::ExecuteQuery_Bind(QString sql, QString parameter)
{
    QStringList list;
    list << parameter;
    return this->ExecuteQuery_Bind(sql, list);
}

SqlResult *SwSql::ExecuteQuery_Bind(QString sql, QStringList parameter)
{
    SqlResult *result = new SqlResult();
    sqlite3_stmt *statement;
    int current_parameter = 1;
    this->LastStatement = sql;
    int x = sqlite3_prepare_v2(this->db, sql.toUtf8().constData(), sql.length() + 1, &statement, NULL);
    if (!this->Evaluate(x))
        goto on_error;
    foreach (QString text, parameter)
    {
        x = sqlite3_bind_text(statement, current_parameter++, text.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        if (!this->Evaluate(x))
            goto on_error;
    }
    x = sqlite3_step(statement);
    while (x == SQLITE_ROW)
    {
        int column_count = sqlite3_column_count(statement);
        QList<QVariant> row;
        int value = 0;
        while (value < column_count)
        {
            int t = sqlite3_column_type(statement, value);
            switch (t)
            {
                case SQLITE_INTEGER:
                    row.append(QVariant(sqlite3_column_int(statement, value)));
                    break;
                case SQLITE_FLOAT:
                    row.append(QVariant(sqlite3_column_double(statement, value)));
                    break;
                case SQLITE_TEXT:
                    row.append(QVariant(StringFromUnsignedChar(sqlite3_column_text(statement, value))));
                    break;
                default:
                    throw new Exception("Unknown data type: " + QString::number(t));
            }
            value++;
        }
        x = sqlite3_step(statement);
        result->columns = column_count;
        result->Rows.append(SwRow(row));
    }

    if (!this->Evaluate(x))
        goto on_error;

    sqlite3_finalize(statement);
    return result;

    on_error:
        sqlite3_finalize(statement);
        this->LastError = QString(sqlite3_errmsg(this->db));
        result->InError = true;
        return result;
}

bool SwSql::Evaluate(int data)
{
    switch (data)
    {
        case SQLITE_OK:
        case SQLITE_ROW:
        case SQLITE_DONE:
            return true;
        case SQLITE_ERROR:
            return false;
    }
    Syslog::Logs->ErrorLog("Unknown result");
    return false;
}

SqlResult::SqlResult()
{
    this->columns = 0;
    this->InError = false;
}

SqlResult::~SqlResult()
{

}

int SqlResult::GetColumns()
{
    return this->columns;
}

SwRow SqlResult::GetRow(unsigned int rowid)
{
    if (rowid >= (unsigned int)this->Rows.count())
        throw new Exception("Too large");


    return this->Rows.at(rowid);
}

int SqlResult::Count()
{
    return this->Rows.count();
}

SwRow::SwRow(QList<QVariant> rx)
{
    this->data = rx;
}

SwRow::~SwRow()
{

}

QVariant SwRow::GetField(unsigned int column)
{
    if (column >= (unsigned int)this->data.count())
    {
        // there is no such column in here
        throw new Smuggle::Exception("Invalid column");
    }

    return this->data.at(column);
}

int SwRow::Columns()
{
    return this->data.count();
}
