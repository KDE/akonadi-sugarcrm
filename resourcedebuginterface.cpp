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

#include "resourcedebuginterface.h"

#include "modulehandler.h"
#include "sugarcrmresource.h"

ResourceDebugInterface::ResourceDebugInterface( SugarCRMResource *resource )
    : QObject( resource ), mResource( resource )
{
}

ResourceDebugInterface::~ResourceDebugInterface()
{
}

void ResourceDebugInterface::setKDSoapDebugEnabled( bool enable )
{
    qputenv( "KDSOAP_DEBUG", ( enable ? "1" : "0" ) );
}

bool ResourceDebugInterface::kdSoapDebugEnabled() const
{
    const QByteArray value = qgetenv( "KDSOAP_DEBUG" );
    return value.toInt() != 0;
}

QStringList ResourceDebugInterface::availableModules() const
{
    return mResource->mAvailableModules;
}

QStringList ResourceDebugInterface::supportedModules() const
{
    return mResource->mModuleHandlers->keys();
}

#include "resourcedebuginterface.moc"
