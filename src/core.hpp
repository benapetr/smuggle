//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#ifndef CORE_H
#define CORE_H

#include "swsql.hpp"

namespace Smuggle
{
    class Core
    {
        public:
            static void Shutdown();
            static void Init();
            static void LoadWikisFromDB(SwSql *file);
    };
}

#endif // CORE_H
