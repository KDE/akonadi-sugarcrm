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

#ifndef SUGARSESSION_H
#define SUGARSESSION_H

#include <QObject>

class Sugarsoap;

class SugarSession : public QObject
{
    Q_OBJECT

    friend class SugarJob;
    friend class ModuleDebugInterface;
    friend class ModuleHandler;

public:
    enum RequiredAction {
        None,
        ReLogin,
        NewLogin
    };

    explicit SugarSession( QObject* parent = 0 );

    ~SugarSession();

    RequiredAction setSessionParameters( const QString &username, const QString &password,
                                         const QString &host );

    void createSoapInterface();

    QString sessionId() const;

    QString userName() const;
    QString password() const;
    QString host() const;

    void logout();

private:
    void setSessionId( const QString &sessionId );
    Sugarsoap *soap();

private:
    class Private;
    Private *const d;
};

#endif
