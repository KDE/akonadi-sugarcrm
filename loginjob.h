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

#ifndef LOGINJOB_H
#define LOGINJOB_H

#include "sugarjob.h"

namespace Sugar {

class LoginJob : public SugarJob
{
    Q_OBJECT

public:
    explicit LoginJob( SugarSession *session, QObject *parent = 0 );

    ~LoginJob();

protected:
    void startSugarTask();
};

}

#endif
