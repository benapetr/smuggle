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
#include "generic.hpp"
#include "importpages.hpp"
#include "mainwindow.hpp"
#include "openwikiform.hpp"
#include "ui_mainwindow.h"
#include "webbrowser.hpp"
#include "wikilist.hpp"
#include "wikisite.hpp"
#include "wikitool.hpp"
#include "swordlog.hpp"
#include "syslog.hpp"
#include <QFile>
#include <QMutex>
#include <QWebView>
#include <QTimer>

using namespace Smuggle;

MainWindow *MainWindow::Window;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    this->ui->setupUi(this);
    this->fLogs = new SwLog(this);
    this->fWeb = new WebBrowser(this);
    this->fWL = new WikiList(this);
    this->addDockWidget(Qt::BottomDockWidgetArea, this->fLogs);
    this->addDockWidget(Qt::LeftDockWidgetArea, this->fWL);
    this->setCentralWidget(this->fWeb);
    this->CurrentSite = NULL;
    this->loop = new QTimer(this);
    Window = this;
    connect(this->loop, SIGNAL(timeout()), this, SLOT(OnLogs()));
    this->RefreshWiki();
    this->loop->start(200);
    QFile *layout;
    if (QFile().exists(Configuration::GetConfigurationPath() + "mainwindow_state"))
    {
        layout = new QFile(Configuration::GetConfigurationPath() + "mainwindow_state");
        if (!layout->open(QIODevice::ReadOnly))
            Syslog::Logs->ErrorLog("Unable to restore window states");
        else if (!this->restoreState(layout->readAll()))
            DEBUG1("Failed to restore state");

        layout->close();
        delete layout;
    }
    if (QFile().exists(Configuration::GetConfigurationPath() + "mainwindow_geometry"))
    {
        DEBUG1("Loading geometry");
        layout = new QFile(Configuration::GetConfigurationPath() + "mainwindow_geometry");
        if (!layout->open(QIODevice::ReadOnly))
            Syslog::Logs->ErrorLog("Unable to restore geometry");
        else if (!this->restoreGeometry(layout->readAll()))
            DEBUG1("Failed to restore layout");
        layout->close();
        delete layout;
    }
    this->fWeb->SelectedWeb()->setHtml(Generic::LoadResource("html/welcome.htm"));
}

MainWindow::~MainWindow()
{
    delete this->fLogs;
    delete this->fWL;
    delete this->ui;
}

void MainWindow::RefreshWiki()
{
    this->fWL->Refresh();
}

QWebView *MainWindow::GetBrowser()
{
    return this->fWeb->SelectedWeb();
}

void Smuggle::MainWindow::on_actionOpen_datafile_triggered()
{

}

void Smuggle::MainWindow::on_actionExit_triggered()
{
    this->Quit();
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

void Smuggle::MainWindow::on_actionImport_pages_triggered()
{
    if (this->CurrentSite == NULL)
    {
        Generic::pMessageBox(this, "No wiki", "No wiki is currently selected in list of wikis");
        return;
    }
    if (!this->CurrentSite->IsInitialized)
    {
        Generic::pMessageBox(this, "Error", "This wiki is not initialized", MessageBoxStyleError);
        return;
    }
    ImportPages *p_wn = new ImportPages(this, CurrentSite);
    p_wn->setAttribute(Qt::WA_DeleteOnClose);
    p_wn->show();
}

void Smuggle::MainWindow::on_actionUpdate_meta_information_triggered()
{
    WikiTool::UpdateMeta(this->CurrentSite);
}

void MainWindow::Quit()
{
    QFile *layout = new QFile(Configuration::GetConfigurationPath() + "mainwindow_state");
    if (!layout->open(QIODevice::ReadWrite | QIODevice::Truncate))
        Syslog::Logs->ErrorLog("Unable to write state to a config file");
    else
        layout->write(this->saveState());
    layout->close();
    delete layout;
    layout = new QFile(Configuration::GetConfigurationPath() + "mainwindow_geometry");
    if (!layout->open(QIODevice::ReadWrite | QIODevice::Truncate))
        Syslog::Logs->ErrorLog("Unable to write geometry to a config file");
    else
    layout->write(this->saveGeometry());
    layout->close();
    delete layout;
    Core::Shutdown();
}

void Smuggle::MainWindow::on_actionDelete_wiki_triggered()
{
    if (!this->CurrentSite)
        return;

    //if (Generic::pMessageBox(this, "Delete", "If you delete this wiki you will permanently lose all its data", MessageBoxStyleQuestion) == Qt::)
    QString wiki = QString::number(this->CurrentSite->ID);
    QString sql = "BEGIN;\n"\
                  "DELETE FROM wiki WHERE id = " + wiki + ";\n"\
                  "DELETE FROM page WHERE wiki = " + wiki + ";\n"\
                  "DELETE FROM revision WHERE wiki = " + wiki + ";\n"\
                  "DELETE FROM namespaces WHERE wiki = " + wiki + ";\n"\
                  "DELETE FROM links WHERE wiki = " + wiki + ";\n"\
                  "DELETE FROM text WHERE wiki = " + wiki + ";\nCOMMIT;";
    if (!this->CurrentSite->Datafile->ExecuteNonQuery(sql))
    {
        Generic::pMessageBox(this, "Error", "Failed to delete wiki: " + this->CurrentSite->Datafile->LastError);
        return;
    }
    delete this->CurrentSite;
    this->CurrentSite = NULL;
    this->RefreshWiki();
}
