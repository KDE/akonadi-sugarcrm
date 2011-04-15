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

#include "updateentryjob.h"

#include "modulehandler.h"
#include "sugarsoap.h"

#include <KDSoapMessage.h>

#include <akonadi/item.h>

#include <KDebug>
#include <KLocale>

#include <QStringList>

using namespace Akonadi;

class UpdateEntryJob::Private
{
    UpdateEntryJob *const q;

public:
    enum Stage {
        Init,
        GetEntry,
        UpdateEntry,
        GetRevision
    };

    explicit Private( UpdateEntryJob *parent, const Item &item )
        : q( parent ), mItem( item ), mHandler( 0 ), mStage( Init )
    {
    }

public:
    Item mItem;
    ModuleHandler *mHandler;

    Item mConflictItem;

    Stage mStage;

public: // slots
    void getEntryDone( const TNS__Get_entry_result &callResult );
    void getEntryError( const KDSoapMessage &fault );
    void setEntryDone( const TNS__Set_entry_result &callResult );
    void setEntryError( const KDSoapMessage &fault );
    void getRevisionDone( const TNS__Get_entry_result &callResult );
    void getRevisionError( const KDSoapMessage &fault );
};

void UpdateEntryJob::Private::getEntryDone( const TNS__Get_entry_result &callResult )
{
    // check if this is our signal
    if ( mStage != GetEntry ) {
        return;
    }

    const QList<TNS__Entry_value> entries = callResult.entry_list().items();
    Q_ASSERT( entries.count() == 1 );
    const Akonadi::Item remoteItem = mHandler->itemFromEntry( entries.first(), mItem.parentCollection() );

    kDebug() << "remote=" << remoteItem.remoteRevision()
             << "local="  << mItem.remoteRevision();
    bool hasConflict = false;
    if ( mItem.remoteRevision().isEmpty() ) {
        kWarning() << "local item (id=" << mItem.id()
                   << ", remoteId=" << mItem.remoteId()
                   << ") in collection=" << mHandler->moduleName()
                   << "does not have remoteRevision";
        hasConflict = !remoteItem.remoteRevision().isEmpty();
    } else if ( remoteItem.remoteRevision().isEmpty() ) {
        kWarning() << "remote item (id=" << remoteItem.id()
                   << ", remoteId=" << remoteItem.remoteId()
                   << ") in collection=" << mHandler->moduleName()
                   << "does not have remoteRevision";
    } else {
        // remoteRevision is an ISO date, so string comparisons are accurate for < or >
        hasConflict = ( remoteItem.remoteRevision() > mItem.remoteRevision() );
    }

    if ( hasConflict ) {
        mConflictItem = remoteItem;
        q->setError( UpdateEntryJob::ConflictError );
        q->setErrorText( i18nc( "info:status parameter is module name",
                                "%1 entry on server is newer than the one the local update was based on",
                                mHandler->moduleName() ) );
        q->emitResult();
    } else {
        mStage = UpdateEntry;

        mHandler->setEntry( mItem );
    }
}

void UpdateEntryJob::Private::getEntryError( const KDSoapMessage &fault )
{
    // check if this is our signal
    if ( mStage != GetEntry ) {
        return;
    }

    if ( !q->handleLoginError( fault ) ) {
        kWarning() << "Update Entry Error:" << fault.faultAsString();

        q->setError( SugarJob::SoapError );
        q->setErrorText( fault.faultAsString() );
        q->emitResult();
    }
}

void UpdateEntryJob::Private::setEntryDone( const TNS__Set_entry_result &callResult )
{
    kDebug() << "Updated entry" << callResult.id() << "in module" << mHandler->moduleName();
    mItem.setRemoteId( callResult.id() );

    mStage = Private::GetRevision;

    TNS__Select_fields selectedFields;
    selectedFields.setItems( QStringList() << QLatin1String( "date_modified" ) );

    q->soap()->asyncGet_entry( q->sessionId(), mHandler->moduleName(), mItem.remoteId(), selectedFields );
}

void UpdateEntryJob::Private::setEntryError( const KDSoapMessage &fault )
{
    if ( !q->handleLoginError( fault ) ) {
        kWarning() << "Update Entry Error:" << fault.faultAsString();

        q->setError( SugarJob::SoapError );
        q->setErrorText( fault.faultAsString() );
        q->emitResult();
    }
}

void UpdateEntryJob::Private::getRevisionDone( const TNS__Get_entry_result &callResult )
{
    // check if this is our signal
    if ( mStage != GetRevision ) {
        return;
    }

    const QList<TNS__Entry_value> entries = callResult.entry_list().items();
    Q_ASSERT( entries.count() == 1 );
    const Akonadi::Item remoteItem = mHandler->itemFromEntry( entries.first(), mItem.parentCollection() );

    mItem.setRemoteRevision( remoteItem.remoteRevision() );
    kDebug() << "Got remote revision" << mItem.remoteRevision();

    q->emitResult();
}

void UpdateEntryJob::Private::getRevisionError( const KDSoapMessage &fault )
{
    // check if this is our signal
    if ( mStage != GetRevision ) {
        return;
    }

    kWarning() << "Error when getting remote revision:" << fault.faultAsString();

    // the item has been added we just don't have a server side datetime
    q->emitResult();
}

UpdateEntryJob::UpdateEntryJob( const Akonadi::Item &item, SugarSession *session, QObject *parent )
    : SugarJob( session, parent ), d( new Private( this, item ) )
{
    connect( soap(), SIGNAL( get_entryDone( TNS__Get_entry_result ) ),
             this,  SLOT( getEntryDone( TNS__Get_entry_result ) ) );
    connect( soap(), SIGNAL( get_entryError( KDSoapMessage ) ),
             this,  SLOT( getEntryError( KDSoapMessage ) ) );

    connect( soap(), SIGNAL( set_entryDone( TNS__Set_entry_result ) ),
             this,  SLOT( setEntryDone( TNS__Set_entry_result ) ) );
    connect( soap(), SIGNAL( set_entryError( KDSoapMessage ) ),
             this,  SLOT( setEntryError( KDSoapMessage ) ) );

    connect( soap(), SIGNAL( get_entryDone( TNS__Get_entry_result ) ),
             this,  SLOT( getRevisionDone( TNS__Get_entry_result ) ) );
    connect( soap(), SIGNAL( get_entryError( KDSoapMessage ) ),
             this,  SLOT( getRevisionError( KDSoapMessage ) ) );
}

UpdateEntryJob::~UpdateEntryJob()
{
    delete d;
}

void UpdateEntryJob::setModule( ModuleHandler *handler )
{
    d->mHandler = handler;
}

ModuleHandler *UpdateEntryJob::module() const
{
    return d->mHandler;
}

Item UpdateEntryJob::item() const
{
    return d->mItem;
}

Item UpdateEntryJob::conflictItem() const
{
    return d->mConflictItem;
}

void UpdateEntryJob::startSugarTask()
{
    Q_ASSERT( d->mItem.isValid() );
    Q_ASSERT( d->mHandler != 0 );

    d->mStage = Private::GetEntry;

    if ( !d->mHandler->getEntry( d->mItem ) ) {
        setError( SugarJob::InvalidContextError );
        setErrorText( i18nc( "@info:status", "Attempting to modify a malformed item in folder %1",
                             d->mHandler->moduleName() ) );
        emitResult();
    }
}

#include "updateentryjob.moc"