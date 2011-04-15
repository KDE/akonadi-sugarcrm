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

#include "fetchentryjob.h"

#include "modulehandler.h"
#include "sugarsoap.h"

#include <KDSoapMessage.h>

#include <akonadi/item.h>

#include <KDebug>
#include <KLocale>

#include <QStringList>

using namespace Akonadi;

class FetchEntryJob::Private
{
    FetchEntryJob *const q;

public:
    explicit Private( FetchEntryJob *parent, const Item &item )
        : q( parent ), mItem( item ), mHandler( 0 )
    {
    }

public:
    Item mItem;
    ModuleHandler *mHandler;

public: // slots
    void getEntryDone( const TNS__Get_entry_result &callResult );
    void getEntryError( const KDSoapMessage &fault );
};

void FetchEntryJob::Private::getEntryDone( const TNS__Get_entry_result &callResult )
{
    const QList<TNS__Entry_value> entries = callResult.entry_list().items();
    Q_ASSERT( entries.count() == 1 );
    const Akonadi::Item remoteItem = mHandler->itemFromEntry( entries.first(), mItem.parentCollection() );

    Item item = remoteItem;
    item.setId( mItem.id() );
    item.setRevision( mItem.revision() );
    mItem = item;
    kDebug() << "Fetched" << mHandler->moduleName()
             << "Entry" << mItem.remoteId()
             << "with revision" << mItem.remoteRevision();

    q->emitResult();
}

void FetchEntryJob::Private::getEntryError( const KDSoapMessage &fault )
{
    if ( !q->handleLoginError( fault ) ) {
        kWarning() << "Fetch Entry Error:" << fault.faultAsString();

        q->setError( SugarJob::SoapError );
        q->setErrorText( fault.faultAsString() );
        q->emitResult();
    }
}

FetchEntryJob::FetchEntryJob( const Akonadi::Item &item, SugarSession *session, QObject *parent )
    : SugarJob( session, parent ), d( new Private( this, item ) )
{
    connect( soap(), SIGNAL( get_entryDone( TNS__Get_entry_result ) ),
             this,  SLOT( getEntryDone( TNS__Get_entry_result ) ) );
    connect( soap(), SIGNAL( get_entryError( KDSoapMessage ) ),
             this,  SLOT( getEntryError( KDSoapMessage ) ) );
}

FetchEntryJob::~FetchEntryJob()
{
    delete d;
}

void FetchEntryJob::setModule( ModuleHandler *handler )
{
    d->mHandler = handler;
}

Item FetchEntryJob::item() const
{
    return d->mItem;
}

void FetchEntryJob::startSugarTask()
{
    Q_ASSERT( d->mItem.isValid() );
    Q_ASSERT( d->mHandler != 0 );

    if ( !d->mHandler->getEntry( d->mItem ) ) {
        setError( SugarJob::InvalidContextError );
        setErrorText( i18nc( "@info:status", "Attempting to fetch a malformed item from folder %1",
                             d->mHandler->moduleName() ) );
        emitResult();
    }
}

#include "fetchentryjob.moc"