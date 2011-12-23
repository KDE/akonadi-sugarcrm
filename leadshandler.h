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

//Added leads handler (mostly copied from ContactsHandler) with modifications outlined below -- Eric Newberry Dec 22, 2011

#pragma once

#include "modulehandler.h"
#include "accessorpair.h"

template <typename U, typename V> class QHash;

class LeadsHandler : public ModuleHandler
{
public:
    LeadsHandler( SugarSession *session );

    ~LeadsHandler();

    QStringList supportedFields() const;

    Akonadi::Collection collection() const;

    void listEntries( const ListEntriesScope &scope );

    bool setEntry( const Akonadi::Item &item );

    Akonadi::Item itemFromEntry( const TNS__Entry_value &entry, const Akonadi::Collection &parentCollection );

    bool needBackendChange( const Akonadi::Item &item, const QSet<QByteArray> &modifiedParts ) const;

    void compare( Akonadi::AbstractDifferencesReporter *reporter,
                  const Akonadi::Item &leftItem, const Akonadi::Item &rightItem );

private:
    inline bool isAddressValue( const QString& value ) const
    { return ( isAltAddressValue( value ) || isPrimaryAddressValue( value ) );}
    inline bool isAltAddressValue( const QString& value ) const
    { return value.startsWith( QString( "alt_address_" ) );}
    inline bool isPrimaryAddressValue( const QString& value ) const
    { return value.startsWith( QString( "primary_address_" ) );}

    QString adjustedTime( const QString datetime ) const;

    typedef QHash<QString, AccessorPair*> AccessorHash;
    AccessorHash *mAccessors;
};