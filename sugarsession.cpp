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

#include "sugarsession.h"

#include "sugarsoap.h"

#include <KUrl>

static QString endPointFromHostString( const QString &host )
{
    KUrl url( host );
    url.setFileName( QLatin1String( "soap.php" ) );
    url.setQuery( QString() );

    return url.url();
}

class SugarSession::Private
{
    SugarSession *const q;
public:
    explicit Private( SugarSession *parent )
        : q( parent ), mSoap( 0 )
    {
    }

public:
    QString mSessionId;
    QString mUserName;
    QString mPassword;
    QString mHost;
    Sugarsoap *mSoap;
};

SugarSession::SugarSession( QObject* parent )
    : QObject( parent ), d( new Private( this ) )
{
}

SugarSession::~SugarSession()
{
    delete d;
}

SugarSession::RequiredAction SugarSession::setSessionParameters( const QString &username, const QString &password, const QString &host )
{
    RequiredAction result = None;
    if ( d->mSoap != 0 ) {
        if ( host != d->mHost || username != d->mUserName ) {
            result = NewLogin;
        } else if ( password != d->mPassword ) {
            result = ReLogin;
        }
    } else {
        result = NewLogin;
    }

    d->mUserName = username;
    d->mPassword = password;
    d->mHost = host;

    return result;
}

void SugarSession::createSoapInterface()
{
    d->mSoap->disconnect();
    d->mSoap->deleteLater();

    d->mSoap = new Sugarsoap;
    d->mSoap->setEndPoint( endPointFromHostString( d->mHost ) );
}

QString SugarSession::sessionId() const
{
    return d->mSessionId;
}

QString SugarSession::userName() const
{
    return d->mUserName;
}

QString SugarSession::password() const
{
    return d->mPassword;
}

QString SugarSession::host() const
{
    return d->mHost;
}

void SugarSession::logout()
{
    if ( !d->mSessionId.isEmpty() && d->mSoap != 0 ) {
        d->mSoap->logout( d->mSessionId );
    }
    d->mSessionId = QString();
}

void SugarSession::setSessionId( const QString &sessionId )
{
    d->mSessionId = sessionId;
}

Sugarsoap *SugarSession::soap()
{
    return d->mSoap;
}

#include "sugarsession.moc"
