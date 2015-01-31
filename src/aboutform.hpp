//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#ifndef ABOUTFORM_HPP
#define ABOUTFORM_HPP

#include <QDialog>

namespace Ui
{
    class AboutForm;
}

namespace Smuggle
{
    class AboutForm : public QDialog
    {
            Q_OBJECT

        public:
            explicit AboutForm(QWidget *parent = 0);
            ~AboutForm();

        private:
            Ui::AboutForm *ui;
    };
}

#endif // ABOUTFORM_HPP
