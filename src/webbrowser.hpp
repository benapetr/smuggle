//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#ifndef WEBBROWSER_HPP
#define WEBBROWSER_HPP

#include <QWidget>

namespace Ui
{
    class WebBrowser;
}

namespace Smuggle
{
    class WebBrowser : public QWidget
    {
            Q_OBJECT
        public:
            explicit WebBrowser(QWidget *parent = 0);
            ~WebBrowser();

        private:
            Ui::WebBrowser *ui;
    };
}

#endif // WEBBROWSER_HPP
