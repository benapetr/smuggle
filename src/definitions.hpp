//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

typedef char byte_ht;

#define SMUGGLE_VERSION                  "1.0.0"

// Version of mediawiki that we do support
#define SMUGGLE_SUPPORTED_MEDIAWIKI_VERSION "1.10"

// we are using translatewiki and if this is not defined there is a huge overhead of Qt code
#ifndef QT_NO_TRANSLATION
    #define QT_NO_TRANSLATION
#endif

#if defined _WIN64 || defined _WIN32
    #define SMUGGLE_WIN
#endif

// comment this out to disable multithreaded garbage collector
// this can be useful for debugging as multithreaded GC is not able to delete Qt objects, so if your code
// is crashing with it only, it means your code suck and need a fix in destructor :))
#ifdef __APPLE__
    #include <cstddef>
    #include "TargetConditionals.h"
    #ifdef TARGET_OS_MAC
// fixme
// this is needed on mac, who knows why, gets a cookie :o
namespace std { typedef decltype(nullptr) nullptr_t; }
        #define SMUGGLE_MACX true
        #define SMUGGLE_NO_MT_GC
    #endif
#endif
// this is a nasty hack that will disable multi threaded gc on MacOS as we had some report that
// it has problems there (need to be fixed though)
#ifndef SMUGGLE_NO_MT_GC
    #define SMUGGLE_USE_MT_GC               "mt"
#endif

#include <QObject>

#if QT_VERSION >= 0x050000
    #define SMUGGLE_QTV5
#endif

//! Path where the extensions are located
#define EXTENSION_PATH                  "extensions"

// stolen from boost/current_function.hpp
// enhanced by Petr Bena, keep the enhancement in case you wanted to update this

#ifndef __LINE__
    #define __LINE__ 0
#endif

#ifndef __FILE__
    #define __FILE__ "{unknown}"
#endif

#define HUGGLE_FILE_LINE QString("@") + QString(__FILE__) + QString(":") + QString::number(__LINE__)

#ifndef BOOST_CURRENT_FUNCTION
    #if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600))
        #define BOOST_CURRENT_FUNCTION QString(__PRETTY_FUNCTION__) + HUGGLE_FILE_LINE
    #elif defined(__DMC__) && (__DMC__ >= 0x810)
        #define BOOST_CURRENT_FUNCTION QString(__PRETTY_FUNCTION__) + HUGGLE_FILE_LINE
    #elif defined(__FUNCSIG__)
        #define BOOST_CURRENT_FUNCTION QString(__FUNCSIG__) + HUGGLE_FILE_LINE
    #elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
        #define BOOST_CURRENT_FUNCTION QString(__FUNCTION__) + HUGGLE_FILE_LINE
    #elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
        #define BOOST_CURRENT_FUNCTION QString(__FUNC__) + HUGGLE_FILE_LINE
    #elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
        #define BOOST_CURRENT_FUNCTION QString(__func__) + HUGGLE_FILE_LINE
    #else
        #define BOOST_CURRENT_FUNCTION QString("(unknown)") + HUGGLE_FILE_LINE
    #endif
#endif

//! Revid of edit that doesn't exist
#define WIKI_UNKNOWN_REVID -1

#endif // CONFIG_H
