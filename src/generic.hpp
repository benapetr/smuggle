//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

#ifndef GENERIC_HPP
#define GENERIC_HPP

#include "definitions.hpp"

#include <QAbstractSocket>
#include <QString>

#define EvaluatePageErrorReason_Missing 0
#define EvaluatePageErrorReason_Unknown 1
#define EvaluatePageErrorReason_NULL    2
#define EvaluatePageErrorReason_NoRevs  3
#define EvaluatePageErrorReason_Running 4

#ifdef MessageBox
    // fix GCC for windows headers port
    #undef MessageBox
#endif

class QWidget;

namespace Smuggle
{
    class ApiQuery;
    class WikiSite;

    enum MessageBoxStyle
    {
        MessageBoxStyleNormal,
        MessageBoxStyleQuestion,
        MessageBoxStyleWarning,
        MessageBoxStyleError
    };

    //! Generic requests that are frequently issued to wiki
    namespace Generic
    {
        /*!
         * \brief Bool2String Convert a bool to string
         * \param b bool
         * \return string
         */
        QString Bool2String(bool b);
        bool SafeBool(int value);
        bool SafeBool(QString value, bool defaultvalue = false);
        int pMessageBox(QWidget *parent, QString title, QString text, MessageBoxStyle st = MessageBoxStyleNormal, bool enforce_stop = false);
        QString SanitizePath(QString name);
        /*!
         * \brief MessageBox Display a message box
         * \param title Title of message box
         * \param text What is displayed in a message
         */
        int MessageBox(QString title, QString text, MessageBoxStyle st = MessageBoxStyleNormal, bool enforce_stop = false, QWidget *parent = NULL);
    }
}

#endif // GENERIC_HPP
