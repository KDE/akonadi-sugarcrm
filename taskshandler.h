/****************************************************************************
** Copyright (C) 2010, 2011 Klaralvdalens Datakonsult AB.  All rights reserved.
** Copyright (C) 2011-2012 Eric Newberry <ericnewberry@mirametrics.com>. All Rights Reserved.
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

#pragma once

#include "modulehandler.h"
#include "taskaccessorpair.h"

template <typename U, typename V> class QHash;

class TasksHandler : public ModuleHandler
{
public:
    TasksHandler( SugarSession *session );

    ~TasksHandler();

    QStringList supportedFields() const;

    Akonadi::Collection collection() const;

    void listEntries( const ListEntriesScope &scope );

    bool setEntry( const Akonadi::Item &item );

    Akonadi::Item itemFromEntry( const TNS__Entry_value &entry, const Akonadi::Collection &parentCollection );

    void compare( Akonadi::AbstractDifferencesReporter *reporter,
                  const Akonadi::Item &leftItem, const Akonadi::Item &rightItem );

private:
    QString adjustedTime( const QString datetime ) const;

    typedef QHash<QString, TaskAccessorPair*> AccessorHash;
    AccessorHash *mAccessors;
};
