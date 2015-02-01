//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#include "configuration.hpp"
#include "core.hpp"
#include "mainwindow.hpp"
#include "openwikiform.hpp"
#include "ui_mainwindow.h"
#include "wikilist.hpp"
#include "wikisite.hpp"
#include "swordlog.hpp"
#include "syslog.hpp"
#include <QMutex>
#include <QTimer>

using namespace Smuggle;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    this->ui->setupUi(this);
    this->fLogs = new SwLog(this);
    this->fWL = new WikiList(this);
    this->addDockWidget(Qt::BottomDockWidgetArea, this->fLogs);
    this->addDockWidget(Qt::LeftDockWidgetArea, this->fWL);
    this->loop = new QTimer(this);
    connect(this->loop, SIGNAL(timeout()), this, SLOT(OnLogs()));
    this->loop->start(200);
}

MainWindow::~MainWindow()
{
    delete this->fLogs;
    delete this->fWL;
    delete this->ui;
}

void MainWindow::RefreshWiki()
{

}

void Smuggle::MainWindow::on_actionOpen_datafile_triggered()
{

}

void Smuggle::MainWindow::on_actionExit_triggered()
{
    Core::Shutdown();
}

void Smuggle::MainWindow::on_actionManage_wikis_triggered()
{

}

void Smuggle::MainWindow::on_actionUpdate_content_triggered()
{

}

void Smuggle::MainWindow::on_actionCreate_wiki_triggered()
{
    OpenWikiForm *form = new OpenWikiForm(this);
    form->setAttribute(Qt::WA_DeleteOnClose);
    form->show();
}

void MainWindow::OnLogs()
{
    Syslog::Logs->lUnwrittenLogs->lock();
    if (Syslog::Logs->UnwrittenLogs.count() > 0)
    {
        int c = 0;
        while (c < Syslog::Logs->UnwrittenLogs.count())
        {
            this->fLogs->InsertText(Syslog::Logs->UnwrittenLogs.at(c));
            c++;
        }
        Syslog::Logs->UnwrittenLogs.clear();
    }
    Syslog::Logs->lUnwrittenLogs->unlock();
    this->fLogs->Render();
}
