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

#ifndef SUGARCONFIGDIALOG_H
#define SUGARCONFIGDIALOG_H

#include "ui_sugarconfigdialog.h"

class Settings;

class SugarConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SugarConfigDialog( Settings *settings, QWidget *parent = 0 );

    ~SugarConfigDialog();

    QString host() const;
    QString user() const;
    QString password() const;

private:
    Ui_SugarConfigDialog mUi;
};

#endif
