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

#ifndef DELETEENTRYJOB_H
#define DELETEENTRYJOB_H

#include "sugarjob.h"

namespace Akonadi {
    class Item;
}

class TNS__Set_entry_result;

class DeleteEntryJob : public SugarJob
{
    Q_OBJECT

public:
    DeleteEntryJob( const Akonadi::Item &item, SugarSession *session, QObject *parent = 0 );

    ~DeleteEntryJob();

    Akonadi::Item item() const;

protected:
    void startSugarTask();

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT( d, void setEntryDone( const TNS__Set_entry_result &callResult ) )
    Q_PRIVATE_SLOT( d, void setEntryError( const KDSoapMessage &fault ) )
};

#endif
