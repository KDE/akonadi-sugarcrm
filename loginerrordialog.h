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

#ifndef LOGINERRORDIALOG_H
#define LOGINERRORDIALOG_H

#include <KDialog>

class KJob;
class SugarSession;

class LoginErrorDialog : public KDialog
{
    Q_OBJECT

public:
    LoginErrorDialog( KJob *job, SugarSession *session, QWidget *parent = 0 );

    ~LoginErrorDialog();

Q_SIGNALS:
    void userOrHostChanged( const QString &user, const QString &host );

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT( d, void changeConfig() )
    Q_PRIVATE_SLOT( d, void cancel() )
};

#endif
