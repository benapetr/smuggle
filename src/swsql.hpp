//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#ifndef SWSQL_HPP
#define SWSQL_HPP

#include <QList>
#include <QVariant>
#include <QString>
#include "sqlite3.h"

namespace Smuggle
{
    class SwRow
    {
        public:
            SwRow(QList<QVariant> rx);
            ~SwRow();
            QVariant GetField(unsigned int column);
            int Columns();
        private:
            QList<QVariant> data;
    };

    class SwSql;

    class SqlResult
    {
        public:
            SqlResult();
            ~SqlResult();
            int GetColumns();
            SwRow GetRow(unsigned int rowid);
            int Count();
            bool InError;
        private:
            QList<SwRow> Rows;
            int columns;
            friend class SwSql;
    };

    class SwSql
    {
        public:
            static QList<SwSql*> Datafiles;
            static SwSql* Default;
            static QString GetSQL(QString name);

            SwSql(QString path);
            ~SwSql();
            QString GetPath();
            bool ExecuteNonQueryLineByLine(QString sql);
            bool ExecuteNonQuery(QString sql);
            SqlResult *ExecuteQuery(QString sql);
            sqlite3 *db;
            QString LastError;
        private:
            bool Evaluate(int data);
            QString file;
    };
}

#endif // SWSQL_HPP
