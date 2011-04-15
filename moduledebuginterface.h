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

#ifndef MODULEDEBUGINTERFACE_H
#define MODULEDEBUGINTERFACE_H

#include <QObject>
#include <QStringList>

class SugarCRMResource;

class ModuleDebugInterface : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.kdab.SugarCRM.ModuleDebug")

    public:
        ModuleDebugInterface( const QString &moduleName, SugarCRMResource *resource );
        ~ModuleDebugInterface();

    public Q_SLOTS:
        Q_SCRIPTABLE QStringList availableFields() const;
        Q_SCRIPTABLE QStringList supportedFields() const;
        Q_SCRIPTABLE QString lastestTimestamp() const;
        Q_SCRIPTABLE void resetLatestTimestamp();

    private:
        const QString mModuleName;
        SugarCRMResource *const mResource;

        mutable QStringList mAvailableFields;
};

#endif
