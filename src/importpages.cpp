//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#include "importpages.hpp"
#include "ui_importpages.h"
#include "swsql.hpp"
#include "wikisite.hpp"

using namespace Smuggle;

ImportPages::ImportPages(QWidget *parent, WikiSite *ws) : QDialog(parent), ui(new Ui::ImportPages)
{
    this->ui->setupUi(this);
    this->site = ws;
    this->ui->label->setText("Wiki: " + ws->Name + "\nPlease select pages you want to import using following tabs");
    QStringList header;
    header << "Page name" << "Status" << "Local version" << "Remote version" << "Download / update";
    this->ui->tableWidget->setColumnCount(5);
    this->ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    this->ui->tableWidget->setHorizontalHeaderLabels(header);
    this->ui->tableWidget->verticalHeader()->setVisible(false);
    this->ui->tableWidget->horizontalHeader()->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->ui->tableWidget->setColumnWidth(0, 80);
    this->ui->tableWidget->setColumnWidth(1, 80);
    this->ui->tableWidget->setColumnWidth(2, 150);
    this->ui->tableWidget->setColumnWidth(3, 150);
    this->ui->tableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->ui->tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->ui->tableWidget->setShowGrid(false);
}

ImportPages::~ImportPages()
{
    delete this->ui;
}

void Smuggle::ImportPages::on_pushButton_2_clicked()
{

}
