//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QString>

namespace Smuggle
{
    class WikiSite;

    class Configuration
    {
        public:
            static QString GetLocalDBPath();
            //! Return a full url like http://en.wikipedia.org/wiki/
            static QString GetProjectWikiURL(WikiSite *Project);
            //! Return a script url like http://en.wikipedia.org/w/
            static QString GetProjectScriptURL(WikiSite *Project);
            static QString GetProjectURL(WikiSite *Project);
            static QString GetConfigurationPath();
            //! Return a prefix for url
            static QString GetURLProtocolPrefix(WikiSite *s = NULL);
            static bool SystemConfig_SafeMode;
            static QString HomePath;
            static QString Version;
            static unsigned int Verbosity;
    };
}

#endif // CONFIGURATION_H
