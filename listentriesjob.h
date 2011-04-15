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

#ifndef LISTENTRIESJOB_H
#define LISTENTRIESJOB_H

#include "sugarjob.h"

#include <akonadi/item.h>

namespace Akonadi {
    class Collection;
}

class ModuleHandler;
class TNS__Get_entry_list_result;

class ListEntriesJob : public SugarJob
{
    Q_OBJECT

public:
    ListEntriesJob( const Akonadi::Collection &collection, SugarSession *session, QObject *parent = 0 );

    ~ListEntriesJob();

    void setModule( ModuleHandler *handler );

Q_SIGNALS:
    void itemsReceived( const Akonadi::Item::List &items );
    void deletedReceived( const Akonadi::Item::List &items );

protected:
    void startSugarTask();

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT( d, void listEntriesDone( const TNS__Get_entry_list_result &callResult ) )
    Q_PRIVATE_SLOT( d, void listEntriesError( const KDSoapMessage &fault ) )
};

#endif
