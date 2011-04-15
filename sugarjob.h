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

#ifndef SUGARJOB_H
#define SUGARJOB_H

#include <KJob>

class KDSoapMessage;
class SugarSession;
class Sugarsoap;
class TNS__Set_entry_result;

class SugarJob : public KJob
{
    Q_OBJECT

public:
    enum Errors {
        LoginError = KJob::UserDefinedError + 1,
        SoapError,
        InvalidContextError,
        TaskError
    };

    explicit SugarJob( SugarSession* session, QObject* parent = 0);

    ~SugarJob();

    void start();

public Q_SLOTS:
    void restart();

protected:
    virtual void startSugarTask() = 0;

    bool handleLoginError( const KDSoapMessage &fault );

    QString sessionId() const;
    Sugarsoap *soap();

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT( d, void startLogin() )
    Q_PRIVATE_SLOT( d, void startTask() )
    Q_PRIVATE_SLOT( d, void loginDone( const TNS__Set_entry_result &callResult ) )
    Q_PRIVATE_SLOT( d, void loginError( const KDSoapMessage &fault ) )
};

#endif
