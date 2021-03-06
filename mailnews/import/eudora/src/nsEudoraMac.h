/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org Code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either of the GNU General Public License Version 2 or later (the "GPL"),
 * or the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef nsEudoraMac_h__
#define nsEudoraMac_h__

#include "nscore.h"
#include "nsString.h"
#include "nsIFileSpec.h"
#include "nsISupportsArray.h"
#include "nsEudoraMailbox.h"
#include "nsEudoraAddress.h"

class nsIImportService;
class nsIMsgAccountManager;
class nsIMsgAccount;


class nsEudoraMac : public nsEudoraMailbox, public nsEudoraAddress {
public:
	nsEudoraMac();
	~nsEudoraMac();

		// retrieve the mail folder
	virtual PRBool		FindMailFolder( nsIFileSpec *pFolder);
		// get the list of mailboxes
	virtual nsresult	FindMailboxes( nsIFileSpec *pRoot, nsISupportsArray **ppArray);
		// get a TOC file from a mailbox file
	virtual nsresult	FindTOCFile( nsIFileSpec *pMailFile, nsIFileSpec **pTOCFile, PRBool *pDeleteToc);

	virtual nsresult	GetAttachmentInfo( const char *pFileName, nsIFileSpec *pSpec, nsCString& mimeType, nsCString& aAttachment);

		// Address book stuff
	virtual PRBool		FindAddressFolder( nsIFileSpec *pFolder);
		// get the list of mailboxes
	virtual nsresult	FindAddressBooks( nsIFileSpec *pRoot, nsISupportsArray **ppArray);

		// import settings
	static PRBool	ImportSettings( nsIFileSpec *pIniFile, nsIMsgAccount **localMailAccount);
	static PRBool	FindSettingsFile( nsIFileSpec *pIniFile) { return( FindEudoraLocation( pIniFile, PR_TRUE));}

private:
	static PRBool		FindEudoraLocation( nsIFileSpec *pFolder, PRBool findIni = PR_FALSE, nsIFileSpec *pLookIn = nsnull);


	nsresult	ScanMailDir( nsIFileSpec *pFolder, nsISupportsArray *pArray, nsIImportService *pImport);
	nsresult	IterateMailDir( nsIFileSpec *pFolder, nsISupportsArray *pArray, nsIImportService *pImport);
	nsresult	FoundMailFolder( nsIFileSpec *mailFolder, const char *pName, nsISupportsArray *pArray, nsIImportService *pImport);
	nsresult	FoundMailbox( nsIFileSpec *mailFile, const char *pName, nsISupportsArray *pArray, nsIImportService *pImport);

	PRBool 		IsValidMailFolderName( nsCString& name);
	PRBool 		IsValidMailboxName( nsCString& fName);
	PRBool 		IsValidMailboxFile( nsIFileSpec *pFile);

	PRBool		CreateTocFromResource( nsIFileSpec *pMail, nsIFileSpec *pToc);

	

		// Settings support
	static PRBool	BuildPOPAccount( nsIMsgAccountManager *accMgr, nsCString **pStrs, nsIMsgAccount **ppAccount, nsString& accName);
	static PRBool	BuildIMAPAccount( nsIMsgAccountManager *accMgr, nsCString **pStrs, nsIMsgAccount **ppAccount, nsString& accName);
	static void		SetIdentities( nsIMsgAccountManager *accMgr, nsIMsgAccount *acc, const char *userName, const char *serverName, nsCString **pStrs);
	static void		SetSmtpServer( nsIMsgAccountManager *pMgr, nsIMsgAccount *pAcc, const char *pServer, const char *pUser);
	static PRBool 	GetSettingsFromResource( nsIFileSpec *pSettings, short resId, nsCString **pStrs, PRBool *pIMAP);


private:
	PRUint32		m_depth;
	nsIFileSpec *	m_mailImportLocation;
        PRBool HasResourceFork(FSSpec *fsSpec);
};


#endif /* nsEudoraMac_h__ */

