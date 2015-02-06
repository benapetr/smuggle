#-------------------------------------------------
#
# Project created by QtCreator 2015-01-30T14:55:56
#
#-------------------------------------------------

QT       += core xml webkit gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

TARGET = smuggle
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    configuration.cpp \
    core.cpp \
    wikisite.cpp \
    terminalparser.cpp \
    queryresult.cpp \
    querypool.cpp \
    query.cpp \
    localization.cpp \
    gc_thread.cpp \
    gc.cpp \
    apiqueryresult.cpp \
    apiquery.cpp \
    collectable_smartptr.cpp \
    collectable.cpp \
    mediawikiobject.cpp \
    version.cpp \
    exception.cpp \
    sqlite/sqlite3.c \
    syslog.cpp \
    swordlog.cpp \
    swsql.cpp \
    aboutform.cpp \
    openwikiform.cpp \
    wikilist.cpp \
    generic.cpp \
    importpages.cpp \
    wikitool.cpp \
    sleeper.cpp \
    webbrowser.cpp \
    wikipage.cpp \
    retrievequery.cpp \
    webquery.cpp

HEADERS  += mainwindow.hpp \
    configuration.hpp \
    core.hpp \
    wikisite.hpp \
    terminalparser.hpp \
    queryresult.hpp \
    querypool.hpp \
    query.hpp \
    localization.hpp \
    gc_thread.hpp \
    gc.hpp \
    apiqueryresult.hpp \
    apiquery.hpp \
    definitions.hpp \
    collectable_smartptr.hpp \
    collectable.hpp \
    mediawikiobject.hpp \
    version.hpp \
    exception.hpp \
    sqlite/sqlite3.h \
    syslog.hpp \
    swordlog.hpp \
    swsql.hpp \
    aboutform.hpp \
    openwikiform.hpp \
    wikilist.hpp \
    generic.hpp \
    importpages.hpp \
    wikitool.hpp \
    sleeper.hpp \
    webbrowser.hpp \
    wikipage.hpp \
    retrievequery.hpp \
    webquery.hpp

FORMS    += mainwindow.ui \
    logs.ui \
    aboutform.ui \
    openwikiform.ui \
    wikilist.ui \
    importpages.ui \
    webbrowser.ui

LIBS += -ldl

RESOURCES += \
    sources.qrc

DISTFILES +=
