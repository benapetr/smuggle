//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#include "wikilist.hpp"
#include "wikisite.hpp"
#include "mainwindow.hpp"
#include "swsql.hpp"
#include "generic.hpp"
#include "syslog.hpp"
#include "ui_wikilist.h"
#include <QWebView>

using namespace Smuggle;

WikiList::WikiList(QWidget *parent) : QDockWidget(parent), ui(new Ui::WikiList)
{
    this->ui->setupUi(this);
    this->ui->tableWidget->setColumnCount(1);
    this->ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    this->ui->tableWidget->horizontalHeader()->setVisible(false);
    this->ui->tableWidget->verticalHeader()->setVisible(false);
    this->ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->ui->tableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->ui->tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->ui->tableWidget->setShowGrid(false);
}

WikiList::~WikiList()
{
    delete this->ui;
}

void WikiList::Refresh()
{
    this->ui->comboBox->clear();
    foreach (WikiSite *xx, WikiSite::Sites)
        this->ui->comboBox->addItem(xx->Name);
    this->ui->comboBox->setCurrentIndex(0);
    this->SelectWiki(0);
}

void WikiList::SelectWiki(int in)
{
    if (!WikiSite::Sites.count())
        return;

    WikiSite *site = WikiSite::Sites.at(in);
    MainWindow::Window->CurrentSite = site;
    this->ui->tableWidget->clear();
    // get a list of all pages
    SqlResult *sql = site->Datafile->ExecuteQuery("SELECT name FROM page WHERE wiki = " + QString::number(site->ID) + " AND "\
                                                  "deleted == 0 AND downloaded == 1 ORDER by name ASC;");
    if (sql->InError)
    {
        Generic::MessageBox("Error", "Unable to retrieve list of pages: " + site->Datafile->LastError);
        delete sql;
        return;
    }
    while (this->ui->tableWidget->rowCount() > 0)
        this->ui->tableWidget->removeRow(0);
    int p = 0;
    while (p < sql->Count())
    {
        SwRow row = sql->GetRow(p++);
        int last = this->ui->tableWidget->rowCount();
        this->ui->tableWidget->insertRow(last);
        this->ui->tableWidget->setItem(last, 0, new QTableWidgetItem(row.GetField(0).toString()));
    }
    delete sql;
    this->ui->tableWidget->resizeColumnsToContents();
    this->ui->tableWidget->resizeRowsToContents();
}

void WikiList::Open(QString page)
{
    if (MainWindow::Window->CurrentSite == NULL)
        return;

    QString sid = QString::number(MainWindow::Window->CurrentSite->ID);

    // download a text from db
    SwSql *db = MainWindow::Window->CurrentSite->Datafile;
    SqlResult *result = db->ExecuteQuery_Bind("SELECT preferred_rev_id FROM page where name = ? AND wiki = " +
                                              sid + ";", page);
    if (result ->InError)
    {
        Syslog::Logs->ErrorLog(db->LastError);
        delete result;
        return;
    }
    if (result->Count() == 0)
    {
        Syslog::Logs->ErrorLog("There is no such a page " + page + " in db");
        delete result;
        return;
    }
    int preferred_text = result->GetRow(0).GetField(0).toInt();
    delete result;
    result = db->ExecuteQuery("SELECT html FROM text WHERE wiki = " + sid +
                              " AND revid = "+ QString::number(preferred_text) + ";");
    if (result->InError)
    {
        Syslog::Logs->ErrorLog(db->LastError);
        delete result;
        return;
    }
    if (result->Count() == 0)
    {
        Syslog::Logs->ErrorLog("There is no text for revision " + QString::number(preferred_text) + " in db");
        delete result;
        return;
    }
    QString html = Generic::MakePage(result->GetRow(0).GetField(0).toString());
    MainWindow::Window->SetWebPageTitle(page);
    MainWindow::Window->GetBrowser()->setHtml(html);
}

void Smuggle::WikiList::on_comboBox_currentIndexChanged(int index)
{
    if (index < 0)
        return;
    this->SelectWiki(index);
}

void Smuggle::WikiList::on_tableWidget_clicked(const QModelIndex &index)
{
    QString title = this->ui->tableWidget->item(index.row(), index.column())->text();
    if (!title.isEmpty())
        this->Open(title);
}
