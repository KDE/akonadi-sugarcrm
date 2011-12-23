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

//Added leads handler (mostly copied from ContactsHandler) with modifications outlined below -- Eric Newberry Dec 22, 2011

#include "leadshandler.h"

#include "sugarsession.h"
#include "sugarsoap.h"

#include <akonadi/kabc/contactparts.h>

#include <akonadi/abstractdifferencesreporter.h>
#include <akonadi/collection.h>

#include <kabc/addressee.h>
#include <kabc/address.h>

#include <KLocale>

#include <QHash>

#include "accessorpair.h"

typedef QString (*valueGetter)( const KABC::Addressee& );
typedef void (*valueSetter)( const QString&, KABC::Addressee&);
typedef void (*addressSetter)( const QString&, KABC::Address& );

static QString getFirstName( const KABC::Addressee &addressee )
{
    return addressee.givenName();
}

static void setFirstName( const QString &value, KABC::Addressee &addressee )
{
    addressee.setGivenName( value );
}

static QString getLastName( const KABC::Addressee &addressee )
{
    return addressee.familyName();
}

static void setLastName( const QString &value, KABC::Addressee &addressee )
{
    addressee.setFamilyName( value );
}

static QString getTitle( const KABC::Addressee &addressee )
{
    return addressee.title();
}

static void setTitle( const QString &value, KABC::Addressee &addressee )
{
    addressee.setTitle( value );
}

static QString getDepartment( const KABC::Addressee &addressee )
{
    return addressee.department();
}

static void setDepartment( const QString &value, KABC::Addressee &addressee )
{
    addressee.setDepartment( value );
}

static QString getAccountName( const KABC::Addressee &addressee )
{
    return addressee.organization();
}

static void setAccountName( const QString &value, KABC::Addressee &addressee )
{
    addressee.setOrganization( value );
}

static QString getAccountId( const KABC::Addressee &addressee )
{
    return addressee.custom( "SUGARCRM", "X-AccountId" );
}

static void setAccountId( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom( "SUGARCRM", "X-AccountId", value );
}

static QString getAssistantName( const KABC::Addressee &addressee )
{
    return addressee.custom( "KADDRESSBOOK", "X-AssistantsName" );
}

static void setAssitantName( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom( "KADDRESSBOOK", "X-AssistantsName", value );
}

static QString getAssistantPhone( const KABC::Addressee &addressee )
{
    return addressee.custom( "SUGARCRM", "X-AssistantsPhone" );
}

static void setAssistantPhone( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom( "SUGARCRM", "X-AssistantsPhone", value );
}

static QString getLeadSourceName( const KABC::Addressee &addressee )
{
    return addressee.custom( "SUGARCRM", "X-LeadSourceName" );
}

static void setLeadSourceName(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "SUGARCRM", "X-LeadSourceName", value );
}

static QString getCampaignName( const KABC::Addressee &addressee )
{
    return addressee.custom( "SUGARCRM", "X-CampaignName" );
}

static void setCampaignName(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "SUGARCRM", "X-CampaignName",value );
}

static QString getCampaignId( const KABC::Addressee &addressee )
{
    return addressee.custom( "SUGARCRM", "X-CampaignId" );
}

static void setCampaignId(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "SUGARCRM", "X-CampaignId",value );
}

static QString getAssignedUserName( const KABC::Addressee &addressee )
{
    return addressee.custom( "SUGARCRM", "X-AssignedUserName" );
}

static void setAssignedUserName(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "SUGARCRM", "X-AssignedUserName", value );
}

static QString getAssignedUserId( const KABC::Addressee &addressee )
{
    return addressee.custom( "SUGARCRM", "X-AssignedUserId" );
}

static void setAssignedUserId(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "SUGARCRM", "X-AssignedUserId", value );
}

static QString getReportsToUserName( const KABC::Addressee &addressee )
{
    return addressee.custom( "SUGARCRM", "X-ReportsToUserName" );
}

static void setReportsToUserName(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "SUGARCRM", "X-ReportsToUserName", value );
}

static QString getReportsToUserId( const KABC::Addressee &addressee )
{
    return addressee.custom( "SUGARCRM", "X-ReportsToUserId" );
}

static void setReportsToUserId(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "SUGARCRM", "X-ReportsToUserId", value );
}

static QString getModifiedByName( const KABC::Addressee &addressee )
{
    return addressee.custom( "SUGARCRM", "X-ModifiedByName" );
}

static void setModifiedByName(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "SUGARCRM", "X-ModifiedByName", value );
}

static QString getDateModified( const KABC::Addressee &addressee )
{
    return addressee.custom( "SUGARCRM", "X-DateModified" );
}

static void setDateModified(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "SUGARCRM", "X-DateModified", value );
}

static QString getModifiedUserName( const KABC::Addressee &addressee )
{
    return addressee.custom( "SUGARCRM", "X-ModifiedUserName" );
}

static void setModifiedUserName(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "SUGARCRM", "X-ModifiedUserName", value );
}

static QString getModifiedUserId( const KABC::Addressee &addressee )
{
    return addressee.custom( "SUGARCRM", "X-ModifiedUserId" );
}

static void setModifiedUserId(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "SUGARCRM", "X-ModifiedUserId", value );
}

static QString getDateCreated( const KABC::Addressee &addressee )
{
    return addressee.custom( "SUGARCRM", "X-DateCreated" );
}

static void setDateCreated(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "SUGARCRM", "X-DateCreated", value );
}

static void setContactId(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "SUGARCRM", "X-ContactId", value );
}

static QString getCreatedByName( const KABC::Addressee &addressee )
{
    return addressee.custom( "SUGARCRM", "X-CreatedByName" );
}

static void setCreatedByName(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "SUGARCRM", "X-CreatedByName", value );
}

static QString getCreatedById( const KABC::Addressee &addressee )
{
    return addressee.custom( "SUGARCRM", "X-CreatedById" );
}

static void setCreatedById(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "SUGARCRM", "X-CreatedById", value );
}

static QString getSalutation( const KABC::Addressee &addressee )
{
    return addressee.custom( "SUGARCRM", "X-Salutation" );
}

static void setSalutation(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "SUGARCRM", "X-Salutation", value );
}

static QString getOpportunityRoleFields( const KABC::Addressee &addressee )
{
    return addressee.custom( "SUGARCRM", "X-OpportunityRoleFields" );
}

static void setOpportunityRoleFields( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom( "SUGARCRM","X-OpportunityRoleFields", value );
}

static QString getCAcceptStatusFields( const KABC::Addressee &addressee )
{
    return addressee.custom( "SUGARCRM", "X-CacceptStatusFields" );
}

static void setCAcceptStatusFields( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom( "SUGARCRM","X-CacceptStatusFields", value );
}

static QString getMAcceptStatusFields( const KABC::Addressee &addressee )
{
    return addressee.custom( "SUGARCRM", "X-MacceptStatusFields" );
}

static void setMAcceptStatusFields( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom( "SUGARCRM","X-MacceptStatusFields", value );
}

static QString getDeleted( const KABC::Addressee &addressee )
{
    return addressee.custom( "SUGARCRM", "X-Deleted" );
}

static void setDeleted (const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom( "SUGARCRM","X-Deleted", value );
}

static QString getDoNotCall( const KABC::Addressee &addressee )
{
    return addressee.custom( "SUGARCRM", "X-DoNotCall" );
}

static void setDoNotCall ( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom( "SUGARCRM","X-DoNotCall", value );
}

static QString getNote( const KABC::Addressee &addressee )
{
    return addressee.note();
}

static void setNote( const QString &value, KABC::Addressee &addressee )
{
    addressee.setNote( value );
}

static QString getBirthday( const KABC::Addressee &addressee )
{
    return addressee.birthday().toString( QString("yyyy-MM-dd" ) );
}

static void setBirthday( const QString &value, KABC::Addressee &addressee )
{
    addressee.setBirthday( QDateTime::fromString( value, QString("yyyy-MM-dd" ) ) );
}

static QString getEmail1( const KABC::Addressee &addressee )
{
    return addressee.preferredEmail();
}

static void setEmail1( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertEmail( value, true );
}

static QString getEmail2( const KABC::Addressee &addressee )
{
    // preferred might not be the first one, so remove it and take the first of
    // the remaining instead of always taking the second one
    QStringList emails = addressee.emails();
    emails.removeAll( addressee.preferredEmail() );
    if ( emails.count() >= 1 ) {
        return emails[ 0 ];
    }

    return QString();
}

static void setEmail2( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertEmail( value, false );
}

static QString getHomePhone( const KABC::Addressee &addressee )
{
    return addressee.phoneNumber( KABC::PhoneNumber::Home ).number();
}

static void setHomePhone( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertPhoneNumber( KABC::PhoneNumber( value, KABC::PhoneNumber::Home ) );
}

static QString getWorkPhone( const KABC::Addressee &addressee )
{
    return addressee.phoneNumber( KABC::PhoneNumber::Work ).number();
}

static void setWorkPhone( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertPhoneNumber( KABC::PhoneNumber( value, KABC::PhoneNumber::Work ) );
}

static QString getMobilePhone( const KABC::Addressee &addressee )
{
    return addressee.phoneNumber( KABC::PhoneNumber::Cell ).number();
}

static void setMobilePhone( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertPhoneNumber( KABC::PhoneNumber( value, KABC::PhoneNumber::Cell ) );
}

// Pending (michel)
// we need to decide what other phone will be
static QString getOtherPhone( const KABC::Addressee &addressee )
{
    return addressee.phoneNumber( KABC::PhoneNumber::Car ).number();
}

static void setOtherPhone( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertPhoneNumber( KABC::PhoneNumber( value, KABC::PhoneNumber::Car ) );
}

static QString getFaxPhone( const KABC::Addressee &addressee )
{
    return addressee.phoneNumber( KABC::PhoneNumber::Work|KABC::PhoneNumber::Fax ).number();
}

static void setFaxPhone( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertPhoneNumber( KABC::PhoneNumber( value, KABC::PhoneNumber::Work|KABC::PhoneNumber::Fax ) );
}

static QString getPrimaryStreet( const KABC::Addressee &addressee )
{
    return addressee.address(KABC::Address::Work).street();
}

static void setPrimaryStreet(const QString &value, KABC::Address &address )
{
    address.setStreet( value );
}

static QString getPrimaryCity( const KABC::Addressee &addressee )
{
    return addressee.address(KABC::Address::Work|KABC::Address::Pref).locality();
}

static void setPrimaryCity(const QString &value, KABC::Address &address )
{
    address.setLocality( value );
}

static QString getPrimaryState( const KABC::Addressee &addressee )
{
    return addressee.address(KABC::Address::Work|KABC::Address::Pref).region();
}

static void setPrimaryState( const QString &value, KABC::Address &address )
{
    address.setRegion( value );
}

static QString getPrimaryPostalcode( const KABC::Addressee &addressee )
{
    return addressee.address(KABC::Address::Work|KABC::Address::Pref).postalCode();
}

static void setPrimaryPostalcode( const QString &value, KABC::Address &address )
{
    address.setPostalCode( value );
}

static QString getPrimaryCountry( const KABC::Addressee &addressee )
{
    return addressee.address(KABC::Address::Work|KABC::Address::Pref).country();
}

static void setPrimaryCountry( const QString &value, KABC::Address &address )
{
    address.setCountry( value );
}

static QString getOtherStreet( const KABC::Addressee &addressee )
{
    return addressee.address(KABC::Address::Home).street();
}

static void setOtherStreet(const QString &value, KABC::Address &address )
{
    address.setStreet( value );
}

static QString getOtherCity( const KABC::Addressee &addressee )
{
    return addressee.address(KABC::Address::Home).locality();
}

static void setOtherCity(const QString &value, KABC::Address &address )
{
    address.setLocality( value );
}

static QString getOtherState( const KABC::Addressee &addressee )
{
    return addressee.address(KABC::Address::Home).region();
}

static void setOtherState( const QString &value, KABC::Address &address )
{
    address.setRegion( value );
}

static QString getOtherPostalcode( const KABC::Addressee &addressee )
{
    return addressee.address(KABC::Address::Home).postalCode();
}

static void setOtherPostalcode( const QString &value, KABC::Address &address )
{
    address.setPostalCode( value );
}

static QString getOtherCountry( const KABC::Addressee &addressee )
{
    return addressee.address(KABC::Address::Home).country();
}

static void setOtherCountry( const QString &value, KABC::Address &address )
{
    address.setCountry( value );
}

//Start By EEMN
static QString getReferedBy( const KABC::Addressee &addressee )
{
    return addressee.custom("SUGARCRM", "X-ReferedBy");
}

static void setReferedBy( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom("SUGARCRM", "X-ReferedBy", value);
}

static QString getLeadSourceDesc( const KABC::Addressee &addressee )
{
    return addressee.custom("SUGARCRM", "X-LeadSourceDesc");
}

static void setLeadSourceDesc( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom("SUGARCRM", "X-LeadSourceDesc", value);
}

static QString getStatus( const KABC::Addressee &addressee )
{
    return addressee.custom("SUGARCRM", "X-Status");
}

static void setStatus( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom("SUGARCRM", "X-Status", value);
}

static QString getStatusDesc( const KABC::Addressee &addressee )
{
    return addressee.custom("SUGARCRM", "X-StatusDesc");
}

static void setStatusDesc( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom("SUGARCRM", "X-StatusDesc", value);
}

static QString getAccountDesc( const KABC::Addressee &addressee )
{
    return addressee.custom("SUGARCRM", "X-AccountDesc");
}

static void setAccountDesc( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom("SUGARCRM", "X-AccountDesc", value);
}

static QString getOpportunityId( const KABC::Addressee &addressee )
{
    return addressee.custom("SUGARCRM", "X-OpportunityId");
}

static void setOpportunityId( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom("SUGARCRM", "X-OpportunityId", value);
}

static QString getOpportunityName( const KABC::Addressee &addressee )
{
    return addressee.custom("SUGARCRM", "X-OpportunityName");
}

static void setOpportunityName( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom("SUGARCRM", "X-OpportunityName", value);
}

static QString getOpportunityAmount( const KABC::Addressee &addressee )
{
    return addressee.custom("SUGARCRM", "X-OpportunityAmount");
}

static void setOpportunityAmount( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom("SUGARCRM", "X-OpportunityAmount", value);
}

static QString getPortalName( const KABC::Addressee &addressee )
{
    return addressee.custom("SUGARCRM", "X-PortalName");
}

static void setPortalName( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom("SUGARCRM", "X-PortalName", value);
}

static QString getPortalApp( const KABC::Addressee &addressee )
{
    return addressee.custom("SUGARCRM", "X-PortalApp");
}

static void setPortalApp( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom("SUGARCRM", "X-PortalApp", value);
}

static QString getWebsite( const KABC::Addressee &addressee )
{
    return addressee.custom("SUGARCRM", "X-Website");
}

static void setWebsite( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom("SUGARCRM", "X-Website", value);
}
//End By EEMN

LeadsHandler::LeadsHandler( SugarSession *session )
    : ModuleHandler( QLatin1String( "Leads" ), session ),
      mAccessors( new AccessorHash )
{
    mAccessors->insert( QLatin1String( "first_name" ),
                        new AccessorPair( getFirstName, setFirstName,
                                          i18nc( "@item:intable", "First Name" ) ) );
    mAccessors->insert( QLatin1String( "last_name" ),
                        new AccessorPair( getLastName, setLastName,
                                          i18nc( "@item:intable", "Last Name" ) ) );
    mAccessors->insert( QLatin1String( "email1" ),
                        new AccessorPair( getEmail1, setEmail1,
                                          i18nc( "@item:intable", "Primary Email" ) ) );
    mAccessors->insert( QLatin1String( "email2" ),
                        new AccessorPair( getEmail2, setEmail2,
                                          i18nc( "@item:intable", "Other Email" ) ) );
    mAccessors->insert( QLatin1String( "title" ),
                        new AccessorPair( getTitle, setTitle,
                                          i18nc( "@item:intable job title", "Title" ) ) );
    mAccessors->insert( QLatin1String( "department" ),
                        new AccessorPair( getDepartment, setDepartment,
                                          i18nc( "@item:intable", "Department" ) ) );
    mAccessors->insert( QLatin1String( "account_name" ),
                        new AccessorPair( getAccountName, setAccountName,
                                          i18nc( "@item:intable", "Account Name" ) ) );
    mAccessors->insert( QLatin1String( "account_id" ),
                        new AccessorPair( getAccountId, setAccountId, QString() ) );
    mAccessors->insert( QLatin1String( "phone_home" ),
                        new AccessorPair( getHomePhone, setHomePhone,
                                          i18nc( "@item:intable", "Phone (Home)" ) ) );
    mAccessors->insert( QLatin1String( "phone_work" ),
                        new AccessorPair( getWorkPhone, setWorkPhone,
                                          i18nc( "@item:intable", "Phone (Office)" ) ) );
    mAccessors->insert( QLatin1String( "phone_mobile" ),
                        new AccessorPair( getMobilePhone, setMobilePhone,
                                          i18nc( "@item:intable", "Phone (Mobile)" ) ) );
    mAccessors->insert( QLatin1String( "phone_other" ),
                        new AccessorPair( getOtherPhone, setOtherPhone,
                                          i18nc( "@item:intable", "Phone (Other)" ) ) );
    mAccessors->insert( QLatin1String( "phone_fax" ),
                        new AccessorPair( getFaxPhone, setFaxPhone,
                                          i18nc( "@item:intable", "Fax" ) ) );
    mAccessors->insert( QLatin1String( "primary_address_street" ),
                        new AccessorPair( getPrimaryStreet, setPrimaryStreet, QString() ) );
    mAccessors->insert( QLatin1String( "primary_address_city" ),
                        new AccessorPair( getPrimaryCity, setPrimaryCity, QString() ) );
    mAccessors->insert( QLatin1String( "primary_address_state" ),
                        new AccessorPair( getPrimaryState, setPrimaryState, QString() ) );
    mAccessors->insert( QLatin1String( "primary_address_postalcode" ),
                        new AccessorPair( getPrimaryPostalcode, setPrimaryPostalcode, QString() ) );
    mAccessors->insert( QLatin1String( "primary_address_country" ),
                        new AccessorPair( getPrimaryCountry, setPrimaryCountry, QString() ) );
    mAccessors->insert( QLatin1String( "alt_address_street" ),
                        new AccessorPair( getOtherStreet, setOtherStreet, QString() ) );
    mAccessors->insert( QLatin1String( "alt_address_city" ),
                        new AccessorPair( getOtherCity, setOtherCity, QString() ) );
    mAccessors->insert( QLatin1String( "alt_address_state" ),
                        new AccessorPair( getOtherState, setOtherState, QString() ) );
    mAccessors->insert( QLatin1String( "alt_address_postalcode" ),
                        new AccessorPair( getOtherPostalcode, setOtherPostalcode, QString() ) );
    mAccessors->insert( QLatin1String( "alt_address_country" ),
                        new AccessorPair( getOtherCountry, setOtherCountry, QString() ) );
    mAccessors->insert( QLatin1String( "birthdate" ),
                        new AccessorPair( getBirthday, setBirthday,
                                          i18nc( "@item:intable", "Birthdate" ) ) );
    mAccessors->insert( QLatin1String( "description" ),
                        new AccessorPair( getNote, setNote,
                                          i18nc( "@item:intable", "Description" ) ) );
    mAccessors->insert( QLatin1String( "assistant" ),
                        new AccessorPair( getAssistantName, setAssitantName,
                                          i18nc( "@item:intable", "Assistant" ) ) );
    mAccessors->insert( QLatin1String( "assistant_phone" ),
                        new AccessorPair( getAssistantPhone, setAssistantPhone,
                                          i18nc( "@item:intable", "Assistant Phone" ) ) );
    mAccessors->insert( QLatin1String( "lead_source" ),
                        new AccessorPair( getLeadSourceName, setLeadSourceName,
                                          i18nc( "@item:intable", "Lead Source" ) ) );
    mAccessors->insert( QLatin1String( "campaign_name" ),
                        new AccessorPair( getCampaignName, setCampaignName,
                                          i18nc( "@item:intable", "Campaign" ) ) );
    mAccessors->insert( QLatin1String( "campaign_id" ),
                        new AccessorPair( getCampaignId, setCampaignId, QString() ) );
    mAccessors->insert( QLatin1String( "assigned_user_name" ),
                        new AccessorPair( getAssignedUserName, setAssignedUserName, QString() ) );
    mAccessors->insert( QLatin1String( "assigned_user_id" ),
                        new AccessorPair( getAssignedUserId, setAssignedUserId, QString() ) );
    mAccessors->insert( QLatin1String( "report_to_name" ),
                        new AccessorPair( getReportsToUserName, setReportsToUserName,
                                          i18nc( "@item:intable", "Reports To" ) ) );
    mAccessors->insert( QLatin1String( "reports_to_id" ),
                        new AccessorPair( getReportsToUserId, setReportsToUserId, QString() ) );
    mAccessors->insert( QLatin1String( "modified_by_name" ),
                        new AccessorPair( getModifiedByName, setModifiedByName, QString() ) );
    mAccessors->insert( QLatin1String( "date_modified" ),
                        new AccessorPair( getDateModified, setDateModified, QString() ) );
    mAccessors->insert( QLatin1String( "modified_user_id" ),
                        new AccessorPair( getModifiedUserId, setModifiedUserId, QString() ) );
    mAccessors->insert( QLatin1String( "modified_user_name" ),
                        new AccessorPair( getModifiedUserName, setModifiedUserName, QString() ) );
    mAccessors->insert( QLatin1String( "date_entered" ),
                        new AccessorPair( getDateCreated, setDateCreated, QString() ) );
    mAccessors->insert( QLatin1String( "id" ),
                        new AccessorPair( 0, setContactId, QString() ) );
    mAccessors->insert( QLatin1String( "created_by_name" ),
                        new AccessorPair( getCreatedByName, setCreatedByName, QString() ) );
    mAccessors->insert( QLatin1String( "created_by" ),
                        new AccessorPair( getCreatedById, setCreatedById, QString() ) );
    mAccessors->insert( QLatin1String( "salutation" ),
                        new AccessorPair( getSalutation, setSalutation,
                                          i18nc( "@item:intable", "Salutation" ) ) );
    mAccessors->insert( QLatin1String( "opportunity_role_fields" ),
                        new AccessorPair( getOpportunityRoleFields, setOpportunityRoleFields, QString() ) );
    mAccessors->insert( QLatin1String( "c_accept_status_fields" ),
                        new AccessorPair( getCAcceptStatusFields, setCAcceptStatusFields, QString() ) );
    mAccessors->insert( QLatin1String( "m_accept_status_fields" ),
                        new AccessorPair( getMAcceptStatusFields, setMAcceptStatusFields, QString() ) );
    mAccessors->insert( QLatin1String( "deleted" ),
                        new AccessorPair( getDeleted, setDeleted, QString() ) );
    mAccessors->insert( QLatin1String( "do_not_call" ),
                        new AccessorPair( getDoNotCall, setDoNotCall, QString() ) );
    //Start by EEMN
    mAccessors->insert( QLatin1String( "refered_by" ),
			new AccessorPair( getReferedBy, setReferedBy, QString() ) );
    mAccessors->insert( QLatin1String( "lead_source_description" ),
			new AccessorPair( getLeadSourceDesc, setLeadSourceDesc, QString() ) );
    mAccessors->insert( QLatin1String( "status" ),
			new AccessorPair( getStatus, setStatus, QString() ) );
    mAccessors->insert( QLatin1String( "status_description" ),
			new AccessorPair( getStatusDesc, setStatusDesc, QString() ) );
    mAccessors->insert( QLatin1String( "account_description" ),
			new AccessorPair( getAccountDesc, setAccountDesc, QString() ) );
    mAccessors->insert( QLatin1String( "opportunity_id" ),
			new AccessorPair( getOpportunityId, setOpportunityId, QString() ) );
    mAccessors->insert( QLatin1String( "opportunity_name" ),
			new AccessorPair( getOpportunityName, setOpportunityName, QString() ) );
    mAccessors->insert( QLatin1String( "opportunity_amount" ),
			new AccessorPair( getOpportunityAmount, setOpportunityAmount, QString() ) );
    mAccessors->insert( QLatin1String( "portal_name" ),
			new AccessorPair( getPortalName, setPortalName, QString() ) );
    mAccessors->insert( QLatin1String( "portal_app" ),
			new AccessorPair( getPortalApp, setPortalApp, QString() ) );
    mAccessors->insert( QLatin1String( "website" ),
			new AccessorPair( getWebsite, setWebsite, QString() ) );
    //End by EEMN
}

LeadsHandler::~LeadsHandler()
{
    qDeleteAll( *mAccessors );
    delete mAccessors;
}

QStringList LeadsHandler::supportedFields() const
{
    return mAccessors->keys();
}

Akonadi::Collection LeadsHandler::collection() const
{
    Akonadi::Collection contactCollection;
    contactCollection.setRemoteId( moduleName() );
    contactCollection.setContentMimeTypes( QStringList() << KABC::Addressee::mimeType() );
    contactCollection.setName( i18nc( "@item folder name", "Contacts" ) );
    contactCollection.setRights( Akonadi::Collection::CanChangeItem |
                                 Akonadi::Collection::CanCreateItem |
                                 Akonadi::Collection::CanDeleteItem );

    return contactCollection;
}

void LeadsHandler::listEntries( const ListEntriesScope &scope )
{
    const QString query = scope.query( QLatin1String( "leads" ) );
    const QString orderBy = QLatin1String( "leads.last_name" );
    const int offset = scope.offset();
    const int maxResults = 100;
    const int fetchDeleted = scope.deleted();

    TNS__Select_fields selectedFields;
    selectedFields.setItems( mAccessors->keys() );

    soap()->asyncGet_entry_list( sessionId(), moduleName(), query, orderBy, offset, selectedFields, maxResults, fetchDeleted );
}

bool LeadsHandler::setEntry( const Akonadi::Item &item )
{
    if ( !item.hasPayload<KABC::Addressee>() ) {
        kError() << "item (id=" << item.id() << ", remoteId=" << item.remoteId()
                 << ", mime=" << item.mimeType() << ") is missing Addressee payload";
        return false;
    }

    QList<TNS__Name_value> itemList;

    // if there is an id add it, otherwise skip this field
    // no id will result in the contact being added
    if ( !item.remoteId().isEmpty() ) {
        TNS__Name_value field;
        field.setName( QLatin1String( "id" ) );
        field.setValue( item.remoteId() );

        itemList << field;
    }

    const KABC::Addressee addressee = item.payload<KABC::Addressee>();
    AccessorHash::const_iterator it    = mAccessors->constBegin();
    AccessorHash::const_iterator endIt = mAccessors->constEnd();
    for ( ; it != endIt; ++it ) {
        // check if this is a read-only field
        if ( (*it)->getter == 0 ) {
            continue;
        }
        TNS__Name_value field;
        field.setName( it.key() );
        field.setValue( (*it)->getter( addressee ) );

        itemList << field;
    }

    TNS__Name_value_list valueList;
    valueList.setItems( itemList );
    soap()->asyncSet_entry( sessionId(), moduleName(), valueList );

    return true;
}

Akonadi::Item LeadsHandler::itemFromEntry( const TNS__Entry_value &entry, const Akonadi::Collection &parentCollection )
{
    Akonadi::Item item;

    const QList<TNS__Name_value> valueList = entry.name_value_list().items();
    if ( valueList.isEmpty() ) {
        kWarning() << "Contacts entry for id=" << entry.id() << "has no values";
        return item;
    }

    item.setRemoteId( entry.id() );
    item.setParentCollection( parentCollection );
    item.setMimeType( KABC::Addressee::mimeType() );

    KABC::Addressee addressee;
    addressee.setUid( entry.id() );
    KABC::Address workAddress,  homeAddress;
    workAddress.setType( KABC::Address::Work | KABC::Address::Pref );
    homeAddress.setType( KABC::Address::Home );

    Q_FOREACH( const TNS__Name_value &namedValue, valueList ) {
        const AccessorHash::const_iterator accessIt = mAccessors->constFind( namedValue.name() );
        if ( accessIt == mAccessors->constEnd() ) {
            // no accessor for field
            continue;
        }

        if ( isAddressValue( namedValue.name() ) ) {
            KABC::Address& address =
                isPrimaryAddressValue( namedValue.name() ) ? workAddress : homeAddress;
            (*accessIt)->setter.aSetter( namedValue.value(), address );
        } else {
            (*accessIt)->setter.vSetter( namedValue.value(), addressee );
        }
    }
    addressee.insertAddress( workAddress );
    addressee.insertAddress( homeAddress );
    item.setPayload<KABC::Addressee>( addressee );
    item.setRemoteRevision( getDateModified( addressee ) );

    return item;
}

bool LeadsHandler::needBackendChange( const Akonadi::Item &item, const QSet<QByteArray> &modifiedParts ) const
{
    if ( ModuleHandler::needBackendChange( item, modifiedParts ) ) {
        return true;
    }

    return modifiedParts.contains( partIdFromPayloadPart( Akonadi::ContactPart::Lookup ) ) ||
           modifiedParts.contains( partIdFromPayloadPart( Akonadi::ContactPart::Standard ) );
}

void LeadsHandler::compare( Akonadi::AbstractDifferencesReporter *reporter,
                               const Akonadi::Item &leftItem, const Akonadi::Item &rightItem )
{
    Q_ASSERT( leftItem.hasPayload<KABC::Addressee>() );
    Q_ASSERT( rightItem.hasPayload<KABC::Addressee>() );

    const KABC::Addressee leftContact = leftItem.payload<KABC::Addressee>();
    const KABC::Addressee rightContact = rightItem.payload<KABC::Addressee>();

    const QString modifiedBy = mSession->userName();
    const QString modifiedOn = formatDate( getDateModified( rightContact ) );

    reporter->setLeftPropertyValueTitle( i18nc( "@title:column", "Local Contact" ) );
    reporter->setRightPropertyValueTitle(
        i18nc( "@title:column", "Serverside Contact: modified by %1 on %2",
               modifiedBy, modifiedOn ) );

    bool seenPrimaryAddress = false;
    bool seenOtherAddress = false;
    AccessorHash::const_iterator it    = mAccessors->constBegin();
    AccessorHash::const_iterator endIt = mAccessors->constEnd();
    for ( ; it != endIt; ++it ) {
        // check if this is a read-only field
        if ( (*it)->getter == 0 ) {
            continue;
        }

        QString leftValue = (*it)->getter( leftContact );
        QString rightValue = (*it)->getter( rightContact );

        QString diffName = (*it)->diffName;
        if ( diffName.isEmpty() ) {
            // check for special fields
            if ( isAddressValue( it.key() ) ) {
                if ( isPrimaryAddressValue( it.key() ) ) {
                    if ( !seenPrimaryAddress ) {
                        diffName = i18nc( "item:intable", "Primary Address" );
                        seenPrimaryAddress = true;
                        const KABC::Address leftAddress =
                            leftContact.address( KABC::Address::Work | KABC::Address::Pref );
                        const KABC::Address rightAddress =
                            rightContact.address( KABC::Address::Work | KABC::Address::Pref );

                        leftValue = leftAddress.formattedAddress();
                        rightValue = rightAddress.formattedAddress();
                    } else {
                        // already printed, skip
                        continue;
                    }
                } else {
                    if ( !seenOtherAddress ) {
                        seenOtherAddress = true;
                        diffName = i18nc( "item:intable", "Other Address" );
                        const KABC::Address leftAddress =
                            leftContact.address( KABC::Address::Home );
                        const KABC::Address rightAddress =
                            rightContact.address( KABC::Address::Home );

                        leftValue = leftAddress.formattedAddress();
                        rightValue = rightAddress.formattedAddress();
                    } else {
                        // already printed, skip
                        continue;
                    }
                }
            } else if ( it.key() == "do_not_call" ) {
                diffName = i18nc( "@item:intable", "Do Not Call" );
                leftValue = getDoNotCall( leftContact ) == QLatin1String( "1" )
                                ? QLatin1String( "Yes" ) : QLatin1String( "No" );
                rightValue = getDoNotCall( rightContact ) == QLatin1String( "1" )
                                ? QLatin1String( "Yes" ) : QLatin1String( "No" );
            } else {
                // internal field, skip
                continue;
            }
        }

        if ( leftValue.isEmpty() && rightValue.isEmpty() ) {
            continue;
        }

        if ( leftValue.isEmpty() ) {
            reporter->addProperty( Akonadi::AbstractDifferencesReporter::AdditionalRightMode,
                                   diffName, leftValue, rightValue );
        } else if ( rightValue.isEmpty() ) {
            reporter->addProperty( Akonadi::AbstractDifferencesReporter::AdditionalLeftMode,
                                   diffName, leftValue, rightValue );
        } else if ( leftValue != rightValue ) {
            reporter->addProperty( Akonadi::AbstractDifferencesReporter::ConflictMode,
                                   diffName, leftValue, rightValue );
        }
    }
}