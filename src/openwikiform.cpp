//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#include "openwikiform.hpp"
#include "swsql.hpp"
#include "generic.hpp"
#include "wikisite.hpp"
#include "ui_openwikiform.h"

using namespace Smuggle;

OpenWikiForm::OpenWikiForm(QWidget *parent) : QDialog(parent), ui(new Ui::OpenWikiForm)
{
    this->ui->setupUi(this);
    foreach (SwSql *file, SwSql::Datafiles)
        this->ui->comboBox->addItem(file->GetPath());
    this->ui->comboBox->setCurrentIndex(0);
}

OpenWikiForm::~OpenWikiForm()
{
    delete this->ui;
}

void OpenWikiForm::on_pushButton_clicked()
{
    // get a datafile first
    SwSql *datafile = NULL;
    foreach (SwSql *xx, SwSql::Datafiles)
    {
        if (xx->GetPath() == this->ui->comboBox->currentText())
        {
            datafile = xx;
        }
    }
    if (!datafile)
    {
        Generic::MessageBox("Invalid datafile", "Datafile not found");
        return;
    }
    QString name = this->ui->lineEdit->text();
    QString url = this->ui->lineEdit_2->text().trimmed().toLower();
    // check if there isn't already wiki with this name
    foreach (WikiSite *site, WikiSite::Sites)
    {
        if (site->Datafile == datafile && site->Name == name)
        {
            Generic::pMessageBox(this, "Error", "You already have a wiki with this name in this datafile");
        }
    }
    if (url.startsWith("https://"))
        url = url.mid(8);
    if (url.startsWith("http://"))
        url = url.mid(7);
    WikiSite *site = new WikiSite(datafile, name, url);
    if (this->ui->checkBox->isChecked())
    {

    }

}