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

namespace Smuggle
{
    class WikiSite;

    class ImportPages : public QDialog
    {
            Q_OBJECT

        public:
            explicit ImportPages(QWidget *parent = 0, WikiSite *ws = NULL);
            ~ImportPages();
            void InsertPage(WikiPage page, bool present);
            void FinishLoad();
            int LastNS;
            QString LastTitle;
            WikiSite *site;
            int Limit;
            QList<WikiPage> Pages;
            QHash<QString, QCheckBox*> CheckBoxes;
        private slots:
            void on_pushButton_2_clicked();
            void on_pushButton_3_clicked();

        private:
            void Enable(bool value);
            Ui::ImportPages *ui;
    };
}

#endif // IMPORTPAGES_HPP
