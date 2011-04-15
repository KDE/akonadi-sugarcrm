/*
    Copyright (c) 2010 KDAB
    Author: Tobias Koenig <tokoe@kde.org>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

// temporary "copy" from kdepimlibs/akonadi/conflicthandling

#ifndef CONFLICTRESOLVEDIALOG_H
#define CONFLICTRESOLVEDIALOG_H

#include "conflicthandler.h"

#include <KDialog>

class ConflictResolveDialog : public KDialog
{
  Q_OBJECT

  public:
    /**
     * Creates a new conflict resolve dialog.
     *
     * @param parent The parent widget.
     */
    explicit ConflictResolveDialog( QWidget *parent = 0 );

    ~ConflictResolveDialog();

    /**
     * Sets the items that causes the conflict.
     *
     * @param localItem The local item which causes the conflict.
     * @param otherItem The conflicting item from the Akonadi storage.
     *
     * @note Both items need the full payload set.
     */
    void setConflictingItems( const Akonadi::Item &localItem, const Akonadi::Item &otherItem );

    void setDifferencesInterface( Akonadi::DifferencesAlgorithmInterface *interface );

    /**
     * Returns the resolve strategy the user choose.
     */
    ConflictHandler::ResolveStrategy resolveStrategy() const;

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT( d, void useLocalItem() )
    Q_PRIVATE_SLOT( d, void useOtherItem() )
    Q_PRIVATE_SLOT( d, void useBothItems() )
    Q_PRIVATE_SLOT( d, void createReport() )
};

#endif
