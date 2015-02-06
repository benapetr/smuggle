//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#include "exception.hpp"
#include "webbrowser.hpp"
#include "ui_webbrowser.h"
#include <QVBoxLayout>
#include <QWebView>

using namespace Smuggle;

WebBrowser::WebBrowser(QWidget *parent) : QWidget(parent), ui(new Ui::WebBrowser)
{
    this->ui->setupUi(this);
    this->currentWeb = NULL;
    this->CreateNewBrowserTab("New page", 0);
}

WebBrowser::~WebBrowser()
{
    delete this->ui;
}

void WebBrowser::CreateNewBrowserTab(QString title, int index)
{
    QWidget *tab = new QWidget(this);
    QWebView *web = new QWebView(this);
    this->lBrowsers.append(web);
    QVBoxLayout *lay = new QVBoxLayout(tab);
    lay->setSizeConstraint(QLayout::SetNoConstraint);
    tab->setLayout(lay);
    lay->setSpacing(0);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->addWidget(web);
    this->ui->tabWidget->insertTab(index, tab, title);
    this->ui->tabWidget->setCurrentIndex(index);
    this->currentWeb = web;
}

QWebView *WebBrowser::SelectedWeb()
{
    if (!this->currentWeb)
        throw new Smuggle::Exception("Invalid web", BOOST_CURRENT_FUNCTION);

    return this->currentWeb;
}

void WebBrowser::SetTitle(QString page)
{
    this->ui->tabWidget->setTabText(this->ui->tabWidget->currentIndex(), page);
}

bool WebBrowser::CloseTab(int index)
{
    if (index < 0)
        index = this->ui->tabWidget->currentIndex();
    if (index >= this->ui->tabWidget->count()-1)
        return false;

    QWebView *web = (QWebView*)this->ui->tabWidget->widget(index)->layout()->itemAt(0)->widget();

    if (this->currentWeb == web)
    {
        if (this->ui->tabWidget->currentIndex() != 0)
            this->ui->tabWidget->setCurrentIndex(0);
        else if (this->ui->tabWidget->count() == 2)
        {
            // there is no way to switch to any other tab - this is a last one
            return false;
        } else
        {
            this->ui->tabWidget->setCurrentIndex(1);
        }
    }

    this->lBrowsers.removeAll(web);
    this->ui->tabWidget->removeTab(index);
    delete web;
}

int WebBrowser::CurrentIndex()
{
    return this->ui->tabWidget->currentIndex();
}

void Smuggle::WebBrowser::on_tabWidget_currentChanged(int index)
{
    int in = this->ui->tabWidget->count() - 1;
    if (index == in)
    {
        // we need to create a new browser window
        this->CreateNewBrowserTab("New tab", in);
    } else
    {
        this->currentWeb = (QWebView*)this->ui->tabWidget->widget(index)->layout()->itemAt(0)->widget();
        if (!this->currentWeb)
            throw new Smuggle::Exception("Invalid browser pointer", BOOST_CURRENT_FUNCTION);
    }
}
