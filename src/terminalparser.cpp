//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

#include "terminalparser.hpp"
#include <QFile>
#include <iostream>
#include "configuration.hpp"
#include "core.hpp"
using namespace Smuggle;
using namespace std;

TerminalParser::TerminalParser(QStringList argv)
{
    this->args = argv;
}

TerminalParser::TerminalParser(int argc, char *argv[])
{
    int i=0;
    this->Silent = false;
    while (i<argc)
    {
        this->args.append(QString(argv[i]));
        ++i;
    }
}

bool TerminalParser::Init()
{
    int x = 1;
    while (x < this->args.count())
    {
        //bool valid = false;
        QString text = this->args.at(x);
        ++x;
        if (text == "-h" || text == "--help")
        {
            DisplayHelp();
            return true;
        }
    }
    return false;
}

static void DisplayVersion()
{
    cout << QString("Smuggle " + Configuration::Version).toStdString() << endl;
}

bool TerminalParser::Parse()
{
    int x = 1;
    while (x < this->args.count())
    {
        bool valid = false;
        QString text = this->args.at(x);
        if (!text.startsWith("--") && text.startsWith("-"))
        {
            text = text.mid(1);
            while (text.length())
            {
                if (this->ParseChar(text.at(0)))
                {
                    return true;
                }
                text = text.mid(1);
            }
            valid = true;
        }
        if (text == "--safe")
        {
            Configuration::SystemConfig_SafeMode = true;
            valid = true;
        }
        if (text == "--version")
        {
            DisplayVersion();
            return true;
        }
        if (!valid)
        {
            if (!this->Silent)
            {
                cerr << QString("This parameter isn't valid: " + text).toStdString() << endl;
            }
            return true;
        }
        ++x;
    }
    return false;
}

bool TerminalParser::ParseChar(QChar x)
{
    switch (x.toLatin1())
    {
        case 'h':
            //help
            DisplayHelp();
            //quit
            return true;
        case 'v':
            Configuration::Verbosity++;
            return false;
        case 'V':
            DisplayVersion();
            return true;
    }
    return false;
}

void TerminalParser::DisplayHelp()
{
    if (this->Silent)
    {
        return;
    }
    cout << "Smuggle\n\n"\
            "You can use following arguments to change the runtime settings:\n"\
            "  -v:              Increases verbosity\n"\
            "  --version | -V:  Display a version\n"\
            "  -h | --help:     Display this help\n\n"\
            "Note: every argument in [brackets] is optional\n"\
            "      but argument in <brackets> is required\n\n"\
            "Smuggle is open source, contribute at https://github.com/benapetr/smuggle" << endl;
}
