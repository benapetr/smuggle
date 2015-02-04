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
#include "ui_wikilist.h"

using namespace Smuggle;

WikiList::WikiList(QWidget *parent) : QDockWidget(parent), ui(new Ui::WikiList)
{
    this->ui->setupUi(this);
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
                                                  "deleted != 0;");
    if (sql->InError)
    {
        Generic::MessageBox("Error", "Unable to retrieve list of pages: " + site->Datafile->LastError);
        delete sql;
        return;
    }
    int p = 0;
    while (p < sql->Count())
    {
        SwRow row = sql->GetRow(p++);
        int last = this->ui->tableWidget->rowCount();
        this->ui->tableWidget->insertRow(last);
        this->ui->tableWidget->setItem(last, 0, new QTableWidgetItem(row.GetField(0).toString()));
    }

    delete sql;
}

void Smuggle::WikiList::on_comboBox_currentIndexChanged(int index)
{
    if (index < 0)
        return;
    this->SelectWiki(index);
}
