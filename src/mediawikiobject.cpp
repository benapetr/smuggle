//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#include "mediawikiobject.hpp"
#include "configuration.hpp"
#include "wikisite.hpp"
using namespace Smuggle;

MediaWikiObject::MediaWikiObject()
{
    this->Site = NULL;
}

MediaWikiObject::MediaWikiObject(WikiSite *site)
{
    this->Site = site;
}

MediaWikiObject::MediaWikiObject(MediaWikiObject *m)
{
    this->Site = m->Site;
}

MediaWikiObject::MediaWikiObject(const MediaWikiObject &m)
{
    this->Site = m.Site;
}

MediaWikiObject::~MediaWikiObject()
{

}

WikiSite *MediaWikiObject::GetSite()
{
    return this->Site;
}
