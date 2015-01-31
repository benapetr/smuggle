//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

namespace Ui
{
    class MainWindow;
}

namespace Smuggle
{
    class SwLog;
    class MainWindow : public QMainWindow
    {
            Q_OBJECT
        public:
            explicit MainWindow(QWidget *parent = 0);
            ~MainWindow();
        private slots:
            void on_actionOpen_datafile_triggered();
            void on_actionExit_triggered();
            void on_actionManage_wikis_triggered();
            void on_actionUpdate_content_triggered();
            void on_actionCreate_wiki_triggered();
        private:
            SwLog *fLogs;
            Ui::MainWindow *ui;
    };
}

#endif // MAINWINDOW_HPP