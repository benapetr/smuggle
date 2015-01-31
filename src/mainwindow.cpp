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
#include "swordlog.hpp"
#include "syslog.hpp"

using namespace Smuggle;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    this->ui->setupUi(this);
    this->fLogs = new SwLog(this);
    this->addDockWidget(Qt::BottomDockWidgetArea, this->fLogs);

}

MainWindow::~MainWindow()
{
    delete this->ui;
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
