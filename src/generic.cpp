//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

#include "generic.hpp"
#include <QMessageBox>
#include "apiquery.hpp"
#include "apiqueryresult.hpp"
#include "configuration.hpp"
#include "exception.hpp"
#include "localization.hpp"

using namespace Smuggle;

// we need to preload this thing so that we don't need to create this string so frequently and toast teh PC
static QString options_ = QUrl::toPercentEncoding("timestamp|user|comment|content");

QString Generic::Bool2String(bool b)
{
    if (b)
    {
        return "true";
    }
    return "false";
}

bool Generic::SafeBool(QString value, bool defaultvalue)
{
    if (value.toLower() == "true")
    {
        return true;
    }
    return defaultvalue;
}

QString Generic::SanitizePath(QString name)
{
    QString new_name = name;
    while (new_name.contains("//"))
        new_name = new_name.replace("//", "/");
#ifdef HUGGLE_WIN
    return new_name.replace("/", "\\");
#else
    return name;
#endif // HUGGLE_WIN
}

int Generic::MessageBox(QString title, QString text, MessageBoxStyle st, bool enforce_stop, QWidget *parent)
{
    QMessageBox *mb = new QMessageBox(parent);
    mb->setWindowTitle(title);
    mb->setText(text);
    int return_value = -1;
    switch (st)
    {
        case MessageBoxStyleQuestion:
            mb->setIcon(QMessageBox::Question);
            mb->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            mb->setDefaultButton(QMessageBox::Yes);
            return_value = mb->exec();
            break;
        case MessageBoxStyleNormal:
            mb->setIcon(QMessageBox::Information);
            goto exec;
        case MessageBoxStyleError:
            mb->setIcon(QMessageBox::Critical);
            goto exec;
        case MessageBoxStyleWarning:
            mb->setIcon(QMessageBox::Warning);
            goto exec;
    }
    delete mb;
    return return_value;
    exec:
        if (enforce_stop)
        {
            return_value = mb->exec();
            delete mb;
        }
        else
        {
            mb->setAttribute(Qt::WA_DeleteOnClose, true);
            mb->show();
        }
        return return_value;
}

int Generic::pMessageBox(QWidget *parent, QString title, QString text, MessageBoxStyle st, bool enforce_stop)
{
    return Generic::MessageBox(title, text, st, enforce_stop, parent);
}

bool Generic::SafeBool(int value)
{
    return value != 0;
}

QString Generic::LoadResource(QString path)
{
    QFile *file = new QFile(":/text/" + path);
    if (!file->open(QIODevice::ReadOnly))
    {
        throw new Smuggle::Exception("Failed to open " + path);
    }
    QString t = file->readAll();
    file->close();
    delete file;
    return t;
}
