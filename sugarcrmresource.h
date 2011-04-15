/****************************************************************************
** Copyright (C) 2010, 2011 Klaralvdalens Datakonsult AB.  All rights reserved.
**
** This file is part of the Akonadi Resource for SugarCRM.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 and version 3 as published by the
** Free Software Foundation and appearing in the file LICENSE.GPL included.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** Contact info@kdab.com if any conditions of this licensing are not
** clear to you.
**
**********************************************************************/

#ifndef SUGARCRMRESOURCE_H
#define SUGARCRMRESOURCE_H

#include <akonadi/resourcebase.h>

#include <QStringList>

class ConflictHandler;
class KJob;
class ModuleDebugInterface;
class ModuleHandler;
class SugarSession;

template <typename U, typename V> class QHash;

class SugarCRMResource : public Akonadi::ResourceBase, public Akonadi::AgentBase::Observer
{
    friend class ModuleDebugInterface;
    friend class ResourceDebugInterface;

    Q_OBJECT

public:
    SugarCRMResource( const QString &id );
    ~SugarCRMResource();

public Q_SLOTS:
    virtual void configure( WId windowId );

protected:
    SugarSession *mSession;

    QStringList mAvailableModules;
    typedef QHash<QString, ModuleHandler*> ModuleHandlerHash;
    ModuleHandlerHash *mModuleHandlers;
    typedef QHash<QString, ModuleDebugInterface*> ModuleDebugInterfaceHash;
    ModuleDebugInterfaceHash *mModuleDebugInterfaces;

    ConflictHandler *mConflictHandler;

protected:
    void aboutToQuit();
    void doSetOnline( bool online );

    void itemAdded( const Akonadi::Item &item, const Akonadi::Collection &collection );
    void itemChanged( const Akonadi::Item &item, const QSet<QByteArray> &parts );
    void itemRemoved( const Akonadi::Item &item );

protected Q_SLOTS:
    void retrieveCollections();
    void retrieveItems( const Akonadi::Collection &col );
    bool retrieveItem( const Akonadi::Item &item, const QSet<QByteArray> &parts );

    void startExplicitLogin();
    void explicitLoginResult( KJob *job );

    void listModulesResult( KJob *job );

    void itemsReceived( const Akonadi::Item::List &items );
    void deletedReceived( const Akonadi::Item::List &items );
    void listEntriesResult( KJob *job );

    void createEntryResult( KJob *job );

    void deleteEntryResult( KJob *job );

    void fetchEntryResult( KJob *job );

    void updateEntryResult( KJob *job );

    void commitChange( const Akonadi::Item &item );

    void updateOnBackend( const Akonadi::Item &item );

    void userOrHostChanged( const QString &user, const QString &host );

private:
    void updateItem( const Akonadi::Item &item, ModuleHandler *handler );

    bool handleLoginError( KJob *job );
};

#endif
