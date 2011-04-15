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

#include "conflictresolvedialog.h"

#include <akonadi/abstractdifferencesreporter.h>
#include <akonadi/differencesalgorithminterface.h>
#include <akonadi/item.h>

#include <KColorScheme>
#include <KLocale>
#include <KPushButton>
#include <KTextBrowser>

#include <QVBoxLayout>

using namespace Akonadi;

static inline QString textToHTML( const QString &text )
{
  return Qt::convertFromPlainText( text );
}

class HtmlDifferencesReporter : public AbstractDifferencesReporter
{
  public:
    HtmlDifferencesReporter()
    {
    }

    QString toHtml() const
    {
      return header() + mContent + footer();
    }

    void setPropertyNameTitle( const QString &title )
    {
      mNameTitle = title;
    }

    void setLeftPropertyValueTitle( const QString &title )
    {
      mLeftTitle = title;
    }

    void setRightPropertyValueTitle( const QString &title )
    {
      mRightTitle = title;
    }

    void addProperty( Mode mode, const QString &name, const QString &leftValue, const QString &rightValue )
    {
      switch ( mode ) {
        case NormalMode:
          mContent.append( QString::fromLatin1( "<tr><td align=\"right\"><b>%1:</b></td><td>%2</td><td></td><td>%3</td></tr>" )
                                              .arg( name )
                                              .arg( textToHTML( leftValue ) )
                                              .arg( textToHTML( rightValue ) ) );
         break;
        case ConflictMode:
          mContent.append( QString::fromLatin1( "<tr><td align=\"right\"><b>%1:</b></td><td bgcolor=\"#ff8686\">%2</td><td></td><td bgcolor=\"#ff8686\">%3</td></tr>" )
                                              .arg( name )
                                              .arg( textToHTML( leftValue ) )
                                              .arg( textToHTML( rightValue ) ) );
         break;
       case AdditionalLeftMode:
         mContent.append( QString::fromLatin1( "<tr><td align=\"right\"><b>%1:</b></td><td bgcolor=\"#9cff83\">%2</td><td></td><td></td></tr>" )
                                             .arg( name )
                                             .arg( textToHTML( leftValue ) ) );
         break;
       case AdditionalRightMode:
         mContent.append( QString::fromLatin1( "<tr><td align=\"right\"><b>%1:</b></td><td></td><td></td><td bgcolor=\"#9cff83\">%2</td></tr>" )
                                             .arg( name )
                                             .arg( textToHTML( rightValue ) ) );
         break;
      }
    }

  private:
    QString header() const
    {
      QString header = QLatin1String( "<html>" );
      header += QString::fromLatin1( "<body text=\"%1\" bgcolor=\"%2\">" )
                                   .arg( KColorScheme( QPalette::Active, KColorScheme::View ).foreground().color().name() )
                                   .arg( KColorScheme( QPalette::Active, KColorScheme::View ).background().color().name() );
      header += QLatin1String( "<center><table>" );
      header += QString::fromLatin1( "<tr><th align=\"center\">%1</th><th align=\"center\">%2</th><td>&nbsp;</td><th align=\"center\">%3</th></tr>" )
                                   .arg( mNameTitle )
                                   .arg( mLeftTitle )
                                   .arg( mRightTitle );

      return header;
    }

    QString footer() const
    {
      return QLatin1String( "</table></center>"
                            "</body>"
                            "</html>" );
    }

    QString mContent;
    QString mNameTitle;
    QString mLeftTitle;
    QString mRightTitle;
};

class ConflictResolveDialog::Private
{
    ConflictResolveDialog *const q;
public:
    explicit Private( ConflictResolveDialog *parent )
        : q( parent ), mStrategy( ConflictHandler::UseLocalItem ), mDiffInterface( 0 ), mView( 0 )
    {
    }

public:
    ConflictHandler::ResolveStrategy mStrategy;

    Item mLocalItem;
    Item mOtherItem;

    DifferencesAlgorithmInterface *mDiffInterface;

    KTextBrowser *mView;

public: // slots
    void useLocalItem();
    void useOtherItem();
    void useBothItems();
    void createReport();
};

void ConflictResolveDialog::Private::useLocalItem()
{
    mStrategy = ConflictHandler::UseLocalItem;
    q->accept();
}

void ConflictResolveDialog::Private::useOtherItem()
{
    mStrategy = ConflictHandler::UseOtherItem;
    q->accept();
}

void ConflictResolveDialog::Private::useBothItems()
{
    mStrategy = ConflictHandler::UseBothItems;
    q->accept();
}

void ConflictResolveDialog::Private::createReport()
{
    Q_ASSERT( mDiffInterface != 0 );

    HtmlDifferencesReporter reporter;

    mDiffInterface->compare( &reporter, mLocalItem, mOtherItem );

    mView->setHtml( reporter.toHtml() );
}

ConflictResolveDialog::ConflictResolveDialog( QWidget *parent )
    : KDialog( parent ), d( new Private( this ) )
{
    setButtons( User1 | User2 | User3 );
    setDefaultButton( User3 );

    button( User3 )->setText( i18nc( "@action:button", "Take left one" ) );
    button( User2 )->setText( i18nc( "@action:button", "Take right one" ) );
    button( User1 )->setText( i18nc( "@action:button", "Keep both" ) );

    connect( this, SIGNAL( user1Clicked() ), SLOT( useBothItems() ) );
    connect( this, SIGNAL( user2Clicked() ), SLOT( useOtherItem() ) );
    connect( this, SIGNAL( user3Clicked() ), SLOT( useLocalItem() ) );

    QWidget *widget = new QWidget( this );
    QVBoxLayout *box = new QVBoxLayout( widget );

    d->mView = new KTextBrowser( widget );
    box->addWidget( d->mView );

    setMainWidget( widget );

    QMetaObject::invokeMethod( this, "createReport", Qt::QueuedConnection );
}

ConflictResolveDialog::~ConflictResolveDialog()
{
    delete d;
}

void ConflictResolveDialog::setConflictingItems( const Item &localItem, const Item &otherItem )
{
    d->mLocalItem = localItem;
    d->mOtherItem = otherItem;
}

void ConflictResolveDialog::setDifferencesInterface( DifferencesAlgorithmInterface *interface )
{
    d->mDiffInterface = interface;
}

ConflictHandler::ResolveStrategy ConflictResolveDialog::resolveStrategy() const
{
    return d->mStrategy;
}


#include "conflictresolvedialog.moc"
