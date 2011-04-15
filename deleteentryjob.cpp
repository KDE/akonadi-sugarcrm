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

#include "deleteentryjob.h"

#include "modulehandler.h"
#include "sugarsoap.h"

#include <KDSoapMessage.h>

#include <akonadi/collection.h>
#include <akonadi/item.h>

#include <KDebug>

#include <QStringList>

using namespace Akonadi;

class DeleteEntryJob::Private
{
    DeleteEntryJob *const q;

public:
    explicit Private( DeleteEntryJob *parent, const Item &item )
        : q( parent ), mItem( item )
    {
    }

public:
    const Item mItem;

public: // slots
    void setEntryDone( const TNS__Set_entry_result &callResult );
    void setEntryError( const KDSoapMessage &fault );
};

void DeleteEntryJob::Private::setEntryDone( const TNS__Set_entry_result &callResult )
{
    Q_UNUSED( callResult );
    kDebug() << "Entry" << mItem.remoteId() << "deleted from module"
             << mItem.parentCollection().remoteId();
    q->emitResult();
}

void DeleteEntryJob::Private::setEntryError( const KDSoapMessage &fault )
{
    if ( !q->handleLoginError( fault ) ) {
        kWarning() << "Delete Entry Error:" << fault.faultAsString();

        q->setError( SugarJob::SoapError );
        q->setErrorText( fault.faultAsString() );
        q->emitResult();
    }
}

DeleteEntryJob::DeleteEntryJob( const Akonadi::Item &item, SugarSession *session, QObject *parent )
    : SugarJob( session, parent ), d( new Private( this, item ) )
{
    connect( soap(), SIGNAL( set_entryDone( TNS__Set_entry_result ) ),
             this,  SLOT( setEntryDone( TNS__Set_entry_result ) ) );
    connect( soap(), SIGNAL( set_entryError( KDSoapMessage ) ),
             this,  SLOT( setEntryError( KDSoapMessage ) ) );
}

DeleteEntryJob::~DeleteEntryJob()
{
    delete d;
}

Item DeleteEntryJob::item() const
{
    return d->mItem;
}

void DeleteEntryJob::startSugarTask()
{
    Q_ASSERT( d->mItem.isValid() );
    Q_ASSERT( d->mItem.parentCollection().isValid() );

    // delete just required identifier and "deleted" field
    // no need for type specific code
    TNS__Name_value idField;
    idField.setName( QLatin1String( "id" ) );
    idField.setValue( d->mItem.remoteId() );

    TNS__Name_value deletedField;
    deletedField.setName( QLatin1String( "deleted" ) );
    deletedField.setValue( QLatin1String( "1" ) );

    TNS__Name_value_list valueList;
    valueList.setItems( QList<TNS__Name_value>() << idField << deletedField );

    soap()->asyncSet_entry( sessionId(), d->mItem.parentCollection().remoteId(), valueList );
}

#include "deleteentryjob.moc"