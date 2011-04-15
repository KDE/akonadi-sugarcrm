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

#ifndef RESOURCEDEBUGINTERFACE_H
#define RESOURCEDEBUGINTERFACE_H

#include <QObject>

class QStringList;
class SugarCRMResource;

class ResourceDebugInterface : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.kdab.SugarCRM.ResourceDebug")

    public:
        explicit ResourceDebugInterface( SugarCRMResource *resource );
        ~ResourceDebugInterface();

    public Q_SLOTS:
        Q_SCRIPTABLE void setKDSoapDebugEnabled( bool enable );
        Q_SCRIPTABLE bool kdSoapDebugEnabled() const;
        Q_SCRIPTABLE QStringList availableModules() const;
        Q_SCRIPTABLE QStringList supportedModules() const;

    private:
        SugarCRMResource *const mResource;
};

#endif
