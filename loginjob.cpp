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

#include "loginjob.h"

#include <KDebug>

using namespace Sugar;

LoginJob::LoginJob( SugarSession *session, QObject *parent )
    : SugarJob( session, parent )
{
    if ( !sessionId().isEmpty() ) {
        kWarning() << "Creating Login job when there is a valid session ID. Will not change anything";
    }
}

LoginJob::~LoginJob()
{
}

void LoginJob::startSugarTask()
{
    // Don't have to do anything, base class already handles login
    emitResult();
}

#include "loginjob.moc"
