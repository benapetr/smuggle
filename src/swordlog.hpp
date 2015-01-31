//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#ifndef HUGGLELOG_H
#define HUGGLELOG_H

#include "definitions.hpp"

#include <QString>
#include <QList>
#include <QDockWidget>
#include "syslog.hpp"
class QMutex;

namespace Ui
{
    class HuggleLog;
}

namespace Smuggle
{
    class SwLog_Line;

    //! This window contains all the messages that are stored in ring log
    class SwLog : public QDockWidget
    {
            Q_OBJECT
        public:
            explicit SwLog(QWidget *parent = 0);
            ~SwLog();
            void InsertText(HuggleLog_Line line);
            QString Format(HuggleLog_Line line);
            void Render();
            bool Modified;

        private:
            QMutex *lock;
            QList<HuggleLog_Line> Text;
            Ui::HuggleLog *ui;
    };
}

#endif // HUGGLELOG_H
