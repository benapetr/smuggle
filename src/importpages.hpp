//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#ifndef IMPORTPAGES_HPP
#define IMPORTPAGES_HPP

#include <QDialog>
#include <QHash>
#include "wikipage.hpp"

namespace Ui
{
    class ImportPages;
}

class QCheckBox;
class QTimer;

namespace Smuggle
{
    class RetrieveQuery;
    class WikiSite;

    class ImportPages : public QDialog
    {
            Q_OBJECT

        public:
            explicit ImportPages(QWidget *parent = 0, WikiSite *ws = NULL);
            ~ImportPages();
            void InsertPage(WikiPage page, bool present);
            void FinishLoad();
            void Reload();
            int LastNS;
            QString LastTitle;
            WikiSite *site;
            int Limit;
            QList<WikiPage> Pages;
            QHash<QString, QCheckBox*> CheckBoxes;
        private slots:
            void on_pushButton_2_clicked();
            void on_pushButton_3_clicked();
            void on_pushButton_clicked();
            void OnTick();
        private:
            void Enable(bool value);
            void DownloadNext();
            bool busy;
            QList<RetrieveQuery*> Downloads;
            QTimer *t;
            Ui::ImportPages *ui;
            int currentPage;
    };
}

#endif // IMPORTPAGES_HPP
