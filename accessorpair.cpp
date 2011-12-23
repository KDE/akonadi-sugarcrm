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

#include "accessorpair.h"

AccessorPair::AccessorPair( valueGetter get, valueSetter set, const QString &name )
    : getter( get ), diffName( name )
{
    setter.vSetter = set;
}

AccessorPair::AccessorPair( valueGetter get, addressSetter set, const QString &name )
    : getter( get ), diffName( name )
{
    setter.aSetter = set;
}