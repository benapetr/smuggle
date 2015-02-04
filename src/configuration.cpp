//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#include "configuration.hpp"
#include "generic.hpp"
#include "wikisite.hpp"
#include <QDir>

using namespace Smuggle;

bool Configuration::SystemConfig_SafeMode = false;
QString Configuration::Version = "1.0.0";
unsigned int Configuration::Verbosity = 0;
QString Configuration::HomePath;

QString Configuration::GetLocalDBPath()
{
    return Configuration::HomePath + "/" + "db/";
}

QString Configuration::GetURLProtocolPrefix(WikiSite *s)
{
    if (s && !s->SupportHttps)
        return "http://";

    return "https://";
}

QString Configuration::GetProjectURL(WikiSite *Project)
{
    if (!Project->URL.endsWith("/"))
        return Configuration::GetURLProtocolPrefix(Project) + Project->URL + "/";
    return Configuration::GetURLProtocolPrefix(Project) + Project->URL;
}

QString Configuration::GetConfigurationPath()
{
    QString path = Generic::SanitizePath(Configuration::HomePath + QDir::separator() + "Configuration" + QDir::separator());
    QDir conf(path);
    if (!conf.exists())
    {
        conf.mkpath(path);
    }
    return path;
}

QString Configuration::GetProjectWikiURL(WikiSite *Project)
{
    return Configuration::GetProjectURL(Project) + Project->LongPath;
}

QString Configuration::GetProjectScriptURL(WikiSite *Project)
{
    return Configuration::GetProjectURL(Project) + Project->ScriptPath;
}
