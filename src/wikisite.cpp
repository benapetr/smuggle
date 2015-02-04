//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

#include "wikisite.hpp"
#include "configuration.hpp"
#include "exception.hpp"
#include "swsql.hpp"
//#include "syslog.hpp"
using namespace Smuggle;

QList<WikiSite*> WikiSite::Sites;

WikiPageNS *WikiSite::Unknown = new WikiPageNS(0, "", "");

WikiPageNS::WikiPageNS(int id, QString name, QString canonical_name)
{
    QString lc = canonical_name.toLower().replace("_", " ");
    QString lw = name.toLower();
    this->Talk = (lc.startsWith("talk") || lc.contains(" talk") || lw.startsWith("talk") || lw.contains(" talk"));
    this->ID = id;
    this->CanonicalName = canonical_name;
    this->Name = name;
}

WikiPageNS::WikiPageNS(const WikiPageNS &k)
{
    this->CanonicalName = k.CanonicalName;
    this->ID = k.ID;
    this->Name = k.Name;
    this->Talk = k.Talk;
}

WikiPageNS::WikiPageNS(WikiPageNS *k)
{
    this->CanonicalName = k->CanonicalName;
    this->ID = k->ID;
    this->Name = k->Name;
    this->Talk = k->Talk;
}

WikiPageNS::~WikiPageNS()
{

}

WikiSite::WikiSite(const WikiSite &w)
{
    QList<int> k_ = w.NamespaceList.keys();
    foreach (int x, k_)
        this->NamespaceList.insert(x, new WikiPageNS(w.NamespaceList[x]));
    this->LongPath = w.LongPath;
    this->IRCChannel = w.IRCChannel;
    this->Name = w.Name;
    this->OAuthURL = w.OAuthURL;
    this->ScriptPath = w.ScriptPath;
    this->SupportHttps = w.SupportHttps;
    this->SupportOAuth = w.SupportOAuth;
    this->IsRightToLeft = false;
    this->Datafile = w.Datafile;
    this->MediawikiVersion = w.MediawikiVersion;
    this->URL = w.URL;
    this->HANChannel = w.HANChannel;
    this->IsRightToLeft = w.IsRightToLeft;
    this->IsInitialized = w.IsInitialized;
    this->Unknown = w.Unknown;
    this->WhiteList = w.WhiteList;
}

WikiSite::WikiSite(WikiSite *w)
{
    QList<int> k_ = w->NamespaceList.keys();
    foreach (int x, k_)
        this->NamespaceList.insert(x, new WikiPageNS(w->NamespaceList[x]));
    this->LongPath = w->LongPath;
    this->IRCChannel = w->IRCChannel;
    this->Name = w->Name;
    this->OAuthURL = w->OAuthURL;
    this->WhiteList = w->WhiteList;
    this->MediawikiVersion = w->MediawikiVersion;
    this->URL = w->URL;
    this->HANChannel = w->HANChannel;
    this->IsRightToLeft = w->IsRightToLeft;
    this->Datafile = w->Datafile;
    this->Unknown = w->Unknown;
    this->IsInitialized = w->IsInitialized;
    this->SupportOAuth = w->SupportOAuth;
    this->SupportHttps = w->SupportHttps;
    this->ScriptPath = w->ScriptPath;
}

WikiSite::WikiSite(SwSql *df, QString name, QString url)
{
    this->LongPath = "wiki/";
    this->Name = name;
    this->URL = url;
    this->ScriptPath = "w/";
    this->OAuthURL = url + "w/index.php?title=Special:MWOAuth";
    this->SupportHttps = true;
    this->IsInitialized = false;
    this->Datafile = df;
    this->SupportOAuth = true;
    this->IRCChannel = "#test.wikipedia";
    this->IsRightToLeft = false;
    this->WhiteList = "test.wikipedia";
    Sites.append(this);
}

WikiSite::WikiSite(SwSql *df, QString name, QString url, QString path, QString script, bool https, bool oauth, QString channel, QString wl, QString han, bool isrtl)
{
    this->IRCChannel = channel;
    this->LongPath = path;
    this->Name = name;
    this->SupportHttps = https;
    this->HANChannel = han;
    this->OAuthURL = url + "w/index.php?title=Special:MWOAuth";
    this->ScriptPath = script;
    this->Datafile = df;
    this->IsInitialized = false;
    this->URL = url;
    this->IsRightToLeft = isrtl;
    this->SupportOAuth = oauth;
    this->WhiteList = wl;
    Sites.append(this);
}

WikiSite::~WikiSite()
{
    Sites.removeAll(this);
    this->ClearNS();
}

int WikiSite::LastNS()
{
    if (this->NamespaceList.isEmpty())
        return -1;
    QList<int> ns = this->NamespaceList.keys();
    qSort(ns);
    return ns.last();
}

WikiPageNS *WikiSite::RetrieveNSFromTitle(QString title)
{
    WikiPageNS *dns_ = NULL;
    QString lct_ = title.toLower();
    lct_ = lct_.replace("_", " ");
    foreach(WikiPageNS *ns_, this->NamespaceList)
    {
        if (ns_->GetName().isEmpty())
            dns_ = ns_;
        else if (lct_.startsWith(ns_->GetName().toLower() + ":"))
            return ns_;
    }
    // let's try canonical names
    foreach(WikiPageNS *ns_, this->NamespaceList)
    {
        if (ns_->GetName().isEmpty())
            continue;
        else if (lct_.startsWith(ns_->GetCanonicalName().toLower() + ":"))
            return ns_;
    }
    if (!dns_)
        return WikiSite::Unknown;
    return dns_;
}

WikiPageNS *WikiSite::RetrieveNSByCanonicalName(QString CanonicalName)
{
    WikiPageNS *dns_ = NULL;
    // let's try canonical names
    foreach(WikiPageNS *ns_, this->NamespaceList)
    {
        if (ns_->GetName().isEmpty())
            dns_ = ns_;
        else if (CanonicalName == ns_->GetCanonicalName())
            return ns_;
    }
    if (!dns_)
        return WikiSite::Unknown;
    return dns_;
}

void WikiSite::InsertNS(WikiPageNS *Ns)
{
    if (this->NamespaceList.contains(Ns->GetID()))
    {
        //Syslog::HuggleLogs->WarningLog("Ignoring multiple definitions of namespace " + QString::number(Ns->GetID()) + " mw bug?");
        return;
    }
    this->NamespaceList.insert(Ns->GetID(), Ns);
}

void WikiSite::RemoveNS(int ns)
{
    if (this->NamespaceList.contains(ns))
    {
        WikiPageNS *n = this->NamespaceList[ns];
        this->NamespaceList.remove(ns);
        delete n;
    }
}

void WikiSite::ClearNS()
{
    QList<int> list = this->NamespaceList.keys();
    foreach (int id, list)
    {
        this->RemoveNS(id);
    }
}
