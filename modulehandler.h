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

#ifndef MODULEHANDLER_H
#define MODULEHANDLER_H

#include <akonadi/differencesalgorithminterface.h>
#include <akonadi/item.h>

namespace Akonadi
{
    class Collection;
}

class SugarSession;
class Sugarsoap;
class TNS__Entry_list;
class TNS__Entry_value;

class ListEntriesScope
{
public:
    ListEntriesScope();
    ListEntriesScope( const QString &timestamp );

    bool isUpdateScope() const;

    void setOffset( int offset );

    int offset() const;

    void fetchDeleted();

    int deleted() const;

    QString query( const QString &module ) const;

private:
    int mOffset;
    QString mUpdateTimestamp;
    bool mGetDeleted;
};

class ModuleHandler : public Akonadi::DifferencesAlgorithmInterface
{
public:
    explicit ModuleHandler( const QString &moduleName, SugarSession *session );

    virtual ~ModuleHandler();

    QString moduleName() const;

    QString latestTimestamp() const;
    void resetLatestTimestamp();

    virtual QStringList supportedFields() const = 0;

    virtual Akonadi::Collection collection() const = 0;

    virtual void listEntries( const ListEntriesScope &scope ) = 0;

    virtual bool setEntry( const Akonadi::Item &item ) = 0;

    virtual bool getEntry( const Akonadi::Item &item );

    virtual Akonadi::Item itemFromEntry( const TNS__Entry_value &entry,
                                         const Akonadi::Collection &parentCollection ) = 0;

    Akonadi::Item::List itemsFromListEntriesResponse( const TNS__Entry_list &entryList,
                                                      const Akonadi::Collection &parentCollection );

    virtual bool needBackendChange( const Akonadi::Item &item, const QSet<QByteArray> &modifiedParts ) const;

protected:
    SugarSession *mSession;
    QString mModuleName;
    QString mLatestTimestamp;

protected:
    static QString formatDate( const QString &dateString );
    static QByteArray partIdFromPayloadPart( const char *part );

    QString sessionId() const;
    Sugarsoap *soap() const;
};

#endif
