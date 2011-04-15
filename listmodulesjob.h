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

#ifndef LISTMODULESJOB_H
#define LISTMODULESJOB_H

#include "sugarjob.h"

class QStringList;
class TNS__Module_list;

class ListModulesJob : public SugarJob
{
    Q_OBJECT

public:
    explicit ListModulesJob( SugarSession *session, QObject *parent = 0 );

    ~ListModulesJob();

    QStringList modules() const;

protected:
    void startSugarTask();

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT( d, void listModulesDone( const TNS__Module_list &callResult ) )
    Q_PRIVATE_SLOT( d, void listModulesError( const KDSoapMessage &fault ) )
};

#endif
