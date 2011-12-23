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

//Put AccessorPair class in a seperate file -- Eric Newberry Dec 22, 2011

#pragma once

#include <kabc/addressee.h>
#include <kabc/address.h>

#include <QHash>

typedef QString (*valueGetter)( const KABC::Addressee& );
typedef void (*valueSetter)( const QString&, KABC::Addressee&);
typedef void (*addressSetter)( const QString&, KABC::Address& );

class AccessorPair
{
public:
    AccessorPair( valueGetter get, valueSetter set, const QString &name );

    AccessorPair( valueGetter get, addressSetter set, const QString &name );
    
public:
    valueGetter getter;
    union tsetter {
        valueSetter vSetter;
        addressSetter aSetter;
    } setter;
    const QString diffName;
};