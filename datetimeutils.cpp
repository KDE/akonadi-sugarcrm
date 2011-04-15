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

#include "datetimeutils.h"

#include <KGlobal>
#include <KLocale>

#include <QDateTime>

#define TIMESTAMPFORMAT QLatin1String( "yyyy-MM-dd hh:mm:ss" )

static QDateTime fromString( const QString &serverTimestamp )
{
    QDateTime dt = QDateTime::fromString( serverTimestamp, TIMESTAMPFORMAT );
    dt.setTimeSpec( Qt::UTC );
    return dt;
}

QString DateTimeUtils::formatTimestamp( const QString &serverTimestamp )
{
    const QDateTime dt = fromString( serverTimestamp ).toLocalTime();
    return KGlobal::locale()->formatDateTime( dt, KLocale::ShortDate, true );
}

QString DateTimeUtils::formatTimestampLong( const QString &serverTimestamp )
{
    const QDateTime dt = fromString( serverTimestamp ).toLocalTime();
    return KGlobal::locale()->formatDateTime( dt, KLocale::LongDate, true );
}

QString DateTimeUtils::currentTimestamp()
{
    return QDateTime::currentDateTime().toUTC().toString( TIMESTAMPFORMAT );
}
