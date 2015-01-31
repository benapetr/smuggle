//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#ifndef GC_H
#define GC_H

#include "definitions.hpp"

#include <QThread>
#include <QMutex>
#include <QList>

// some macros so that people hate us
#define GC_DECREF(collectable) if (collectable) collectable->DecRef(); collectable = NULL
#define GC_DECNAMEDREF(collectable, consumer) if(collectable) collectable->UnregisterConsumer(consumer); collectable = NULL
#define GC_LIMIT 60

namespace Smuggle
{
    class Collectable;
    class GC_t;

    //! Garbage collector that can be used to collect some objects

    //! Every object must be derived from Collectable, otherwise it
    //! must not be handled by garbage collector
    class GC
    {
        public:
            /*!
             * \brief Collect will safely decrement a reference of an object and set the pointer to NULL
             * \param object
             */
            //static void Collect(Collectable **object);
            //static void Collect(Collectable **object, int ConsumerID);
            static GC *gc;

            GC();
            ~GC();
            //! Function that walks through the list and delete these that can be deleted
            void DeleteOld();
            void Start();
            void Stop();
            bool IsRunning();
            //! List of all managed queries that qgc keeps track of
            QList<Collectable*> list;
            //! QMutex that is used to lock the GC::list object

            //! This lock needs to be aquired every time when you need to access this list
            //! from any thread during runtime
            QMutex * Lock;
        private:
            GC_t *gc_t;
    };
}

#endif // GC_H
