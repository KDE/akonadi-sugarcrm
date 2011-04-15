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

#include "listmodulesjob.h"

#include "sugarsoap.h"

#include <KDSoapMessage.h>

#include <KDebug>

#include <QStringList>

class ListModulesJob::Private
{
    ListModulesJob *const q;

public:
    explicit Private( ListModulesJob *parent ) : q( parent )
    {
    }

public:
    QStringList mModules;

public: // slots
    void listModulesDone( const TNS__Module_list &callResult );
    void listModulesError( const KDSoapMessage &fault );
};

void ListModulesJob::Private::listModulesDone( const TNS__Module_list &callResult )
{
    const TNS__Select_fields moduleNames = callResult.modules();
    mModules = moduleNames.items();

    kDebug() << "Got" << mModules.count() << "available modules";

    q->emitResult();
}

void ListModulesJob::Private::listModulesError( const KDSoapMessage &fault )
{
    if ( !q->handleLoginError( fault ) ) {
        kWarning() << "List Modules Error:" << fault.faultAsString();

        q->setError( SugarJob::SoapError );
        q->setErrorText( fault.faultAsString() );
        q->emitResult();
    }
}

ListModulesJob::ListModulesJob( SugarSession *session, QObject *parent )
    : SugarJob( session, parent ), d( new Private( this ) )
{
    connect( soap(), SIGNAL( get_available_modulesDone( TNS__Module_list ) ),
             this,  SLOT( listModulesDone( TNS__Module_list ) ) );
    connect( soap(), SIGNAL( get_available_modulesError( KDSoapMessage ) ),
             this,  SLOT( listModulesError( KDSoapMessage ) ) );
}

ListModulesJob::~ListModulesJob()
{
    delete d;
}

QStringList ListModulesJob::modules() const
{
    return d->mModules;
}

void ListModulesJob::startSugarTask()
{
    soap()->asyncGet_available_modules( sessionId() );
}

#include "listmodulesjob.moc"