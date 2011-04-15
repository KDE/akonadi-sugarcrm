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

#include "modulehandler.h"

#include "datetimeutils.h"
#include "sugarsession.h"
#include "sugarsoap.h"

#include <KLocale>

#include <QInputDialog>
#include <QStringList>

ListEntriesScope::ListEntriesScope()
    : mOffset( 0 ),
      mGetDeleted( false )
{
}

ListEntriesScope::ListEntriesScope( const QString &timestamp )
    : mOffset( 0 ),
      mUpdateTimestamp( timestamp ),
      mGetDeleted( false )
{
}

bool ListEntriesScope::isUpdateScope() const
{
    return !mUpdateTimestamp.isEmpty();
}

void ListEntriesScope::setOffset( int offset )
{
    mOffset = offset;
}

int ListEntriesScope::offset() const
{
    return mOffset;
}

void ListEntriesScope::fetchDeleted()
{
    mGetDeleted = true;
}

int ListEntriesScope::deleted() const
{
    return mGetDeleted ? 1 : 0;
}

QString ListEntriesScope::query( const QString &module ) const
{
    if ( mUpdateTimestamp.isEmpty() ) {
        return QLatin1String( "" );
    }

    return module + QLatin1String( ".date_modified >= '") + mUpdateTimestamp + QLatin1String( "'" );
}

ModuleHandler::ModuleHandler( const QString &moduleName, SugarSession *session )
    : mSession( session ), mModuleName( moduleName )
{
    Q_ASSERT( mSession != 0 );
}

ModuleHandler::~ModuleHandler()
{
}

QString ModuleHandler::moduleName() const
{
    return mModuleName;
}

QString ModuleHandler::latestTimestamp() const
{
    return mLatestTimestamp;
}

void ModuleHandler::resetLatestTimestamp()
{
    mLatestTimestamp = QString();
}

bool ModuleHandler::getEntry( const Akonadi::Item &item )
{
    if ( item.remoteId().isEmpty() ) {
        kError() << "Item remoteId is empty. id=" << item.id();
        return false;
    }

    TNS__Select_fields selectedFields;
    selectedFields.setItems( supportedFields() );

    soap()->asyncGet_entry( sessionId(), mModuleName, item.remoteId(), selectedFields );
    return true;
}

Akonadi::Item::List ModuleHandler::itemsFromListEntriesResponse( const TNS__Entry_list &entryList, const Akonadi::Collection &parentCollection )
{
    Akonadi::Item::List items;

    Q_FOREACH( const TNS__Entry_value &entry, entryList.items() ) {
        const Akonadi::Item item = itemFromEntry( entry, parentCollection );
        if ( !item.remoteId().isEmpty() ) {
            items << item;
            if ( mLatestTimestamp.isNull() || item.remoteRevision() > mLatestTimestamp ) {
                mLatestTimestamp = item.remoteRevision();
            }
        }
    }

    return items;
}

bool ModuleHandler::needBackendChange( const Akonadi::Item &item, const QSet<QByteArray> &modifiedParts ) const
{
    Q_UNUSED( item );

    return modifiedParts.contains( partIdFromPayloadPart( Akonadi::Item::FullPayload ) );
}

QString ModuleHandler::formatDate( const QString &dateString )
{
    return DateTimeUtils::formatTimestamp( dateString );
}

QByteArray ModuleHandler::partIdFromPayloadPart( const char *part )
{
    return QByteArray( "PLD:" ) + part;
}

QString ModuleHandler::sessionId() const
{
    return mSession->sessionId();
}

Sugarsoap *ModuleHandler::soap() const
{
    return mSession->soap();
}
