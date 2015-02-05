//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#include "core.hpp"
#include "mainwindow.hpp"
#include "terminalparser.hpp"
#include <QApplication>

using namespace Smuggle;

int main(int argc, char *argv[])
{
    QApplication::setApplicationName("Smuggle");
    QApplication::setOrganizationName("Wikimedia");
    TerminalParser *parser = new TerminalParser(argc, argv);
    if (parser->Parse())
    {
        delete parser;
        return 0;
    }
    delete parser;
    QApplication a(argc, argv);
    Core::Init();
    Smuggle::MainWindow w;
    w.show();

    return a.exec();
}
