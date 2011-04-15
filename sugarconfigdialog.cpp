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

#include "sugarconfigdialog.h"

#include "settings.h"

SugarConfigDialog::SugarConfigDialog( Settings *settings, QWidget *parent )
    : QDialog( parent )
{
    mUi.setupUi( this );

    mUi.host->setText( settings->host() );
    mUi.user->setText( settings->user() );
    mUi.password->setText( settings->password() );
}

SugarConfigDialog::~SugarConfigDialog()
{
}

QString SugarConfigDialog::host() const
{
    return mUi.host->text();
}

QString SugarConfigDialog::user() const
{
    return mUi.user->text();
}

QString SugarConfigDialog::password() const
{
    return mUi.password->text();
}

#include "sugarconfigdialog.moc"
