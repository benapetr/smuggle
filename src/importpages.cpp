//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#include <QCheckBox>
#include "apiquery.hpp"
#include "apiqueryresult.hpp"
#include "exception.hpp"
#include "importpages.hpp"
#include "ui_importpages.h"
#include "generic.hpp"
#include "syslog.hpp"
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
    this->ui->tableWidget->setColumnWidth(4, 280);
    this->ui->tableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->ui->tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->ui->tableWidget->setShowGrid(false);
    this->ui->tableWidget_2->setVisible(false);
    this->Limit = 0;
    SqlResult *result = ws->Datafile->ExecuteQuery("SELECT name, downloaded FROM page WHERE wiki = " + QString::number(site->ID) + ";");

    int n = 0;
    while (n < result->Count())
    {
        SwRow page = result->GetRow(n);
        n++;
        if (page.GetField(0).toString().isEmpty())
            continue;
        WikiPage wp = WikiPage(this->site, page.GetField(0).toString());
        bool present = Generic::SafeBool(page.GetField(1).toInt());
        this->InsertPage(wp, present);
    }
    this->ui->tableWidget->resizeColumnsToContents();
    this->ui->tableWidget->resizeRowsToContents();
}

ImportPages::~ImportPages()
{
    delete this->ui;
}

void ImportPages::InsertPage(WikiPage page, bool present)
{
    if (page.PageName.isEmpty())
        return;
    //! \todo Fix
    // temporary hack
    if (this->CheckBoxes.contains(page.PageName))
        return;
    this->Pages.append(page);
    int last = this->ui->tableWidget->rowCount();
    this->ui->tableWidget->insertRow(last);
    this->ui->tableWidget->setItem(last, 0, new QTableWidgetItem(page.PageName));
    if (!present)
        this->ui->tableWidget->setItem(last, 1, new QTableWidgetItem("Not downloaded yet"));
    else
        this->ui->tableWidget->setItem(last, 1, new QTableWidgetItem("Downloaded"));
    this->ui->tableWidget->setItem(last, 2, new QTableWidgetItem("N / A"));
    this->ui->tableWidget->setItem(last, 3, new QTableWidgetItem("N / A"));
    QCheckBox *x = new QCheckBox(this);
    if (this->CheckBoxes.contains(page.PageName))
        throw new Smuggle::Exception("There is already this page in checkbox list");
    this->CheckBoxes.insert(page.PageName, x);
    this->ui->tableWidget->setCellWidget(last, 4, x);
}

static void Pages_OnFail(Query *query)
{
    ApiQuery *qr = (ApiQuery*)query;
    Syslog::Logs->ErrorLog("failed to get a list of pages: " + qr->GetFailureReason());
    qr->DecRef();
}

void ImportPages::FinishLoad()
{
    this->ui->tableWidget->resizeColumnsToContents();
    this->ui->tableWidget->resizeRowsToContents();
    this->Enable(true);
}

static void Pages_Finish(Query *query)
{
    ApiQuery *qr = (ApiQuery*)query;
    ImportPages *f = (ImportPages*)query->CallbackOwner;
    QList<ApiQueryResultNode*> pages = qr->GetApiQueryResult()->GetNodes("p");
    QString title;
    if (!pages.count())
        goto check_ns;
    foreach (ApiQueryResultNode *node, pages)
    {
        if (pages.last() == node && !f->LastTitle.isEmpty() && f->LastTitle == node->GetAttribute("title"))
            goto check_ns;
        if (f->Limit <= f->Pages.count())
            goto done;
        f->LastTitle = node->GetAttribute("title");
        title = node->GetAttribute("title");
        if (title.isEmpty())
            continue;
        QString ns = node->GetAttribute("ns");
        WikiPage page(f->site, title);
        // check if this page is already in DB now
        QStringList params;
        params.append(title);
        SqlResult *sql = f->site->Datafile->ExecuteQuery_Bind(QString("SELECT name, downloaded FROM page WHERE wiki = ") +
                                                              QString::number(f->site->ID) + " AND name = ?;",  params);
        if (sql->InError)
        {
            Syslog::Logs->ErrorLog("Unable to get page data from DB: " + f->site->Datafile->LastError);
            delete sql;
            continue;
        }
        if (!sql->Count())
        {
            // there is no such a page in DB so far
            SqlResult *sqlx = f->site->Datafile->ExecuteQuery_Bind(QString("INSERT INTO page(name, wiki, namespace, downloaded) VALUES") +
                                                                   "(?, " + QString::number(f->site->ID) + ", " +
                                                                   ns + ", 0);", params);
            if (sqlx->InError)
            {
                Syslog::Logs->ErrorLog("Unable to insert page data to DB: " + f->site->Datafile->LastError);
                delete sqlx;
                goto exit;
            }
            delete sqlx;
            f->InsertPage(page, false);
        } else
        {
            // update the page
        }
        title = page.RootName();
        delete sql;
    }

    // check if this really is end or if we need more data from wiki
    if (!title.isEmpty() && f->Limit > f->Pages.count())
    {
        ApiQuery *xx = new ApiQuery(ActionQuery, f->site);
        xx->Parameters = "list=allpages&apnamespace=" +
                QString::number(f->LastNS) + "&apfrom=" + QUrl::toPercentEncoding(title);
        xx->FailureCallback = (Callback)  Pages_OnFail;
        xx->CallbackOwner = f;
        xx->callback = (Callback)  Pages_Finish;
        xx->Process();
        goto exit;
    }

    check_ns:
        // check if last namespace was really last
        if (f->LastNS < f->site->LastNS())
        {
            // find a next NS
            int ns = 0;
            QList<int> spaces = qr->GetSite()->NamespaceList.keys();
            qSort(spaces);
            while (ns < spaces.count())
            {
                if ((f->LastNS == 0 || spaces.at(ns) == f->LastNS) && spaces.count() > ns+1)
                {
                    // nope it's not a last one
                    f->LastTitle = "";
                    f->LastNS = spaces.at(ns+1);
                    ApiQuery *xx = new ApiQuery(ActionQuery, f->site);
                    xx->Parameters = "list=allpages&apnamespace=" +
                            QString::number(f->LastNS);
                    xx->FailureCallback = (Callback)  Pages_OnFail;
                    xx->CallbackOwner = f;
                    xx->callback = (Callback)  Pages_Finish;
                    xx->Process();
                    goto exit;
                }
                ns++;
            }
        }

    done:
        f->FinishLoad();

    exit:
    qr->DecRef();
}

void Smuggle::ImportPages::on_pushButton_2_clicked()
{
    this->Limit = this->ui->spinBox->value();
    this->Enable(false);
    ApiQuery *xx = new ApiQuery(ActionQuery, this->site);
    this->LastTitle = "";
    this->LastNS = 0;
    xx->Parameters = "list=allpages";
    xx->FailureCallback = (Callback)  Pages_OnFail;
    xx->CallbackOwner = this;
    xx->callback = (Callback)  Pages_Finish;
    xx->Process();
}

void ImportPages::Enable(bool value)
{
    this->ui->pushButton->setEnabled(value);
    this->ui->pushButton_2->setEnabled(value);
}

void Smuggle::ImportPages::on_pushButton_3_clicked()
{

}
