/***************************************************************************
    NWNX Character Include - Character functions plugin
    Copyright (C) 2006 Ingmar Stieger (Papillon, papillon@nwnx.org)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ***************************************************************************/

/*

	---------------------------------------------------------------------------------------
	Date		Name			Reason
	---------------------------------------------------------------------------------------
	021507		KFW				Initial.
	071307		KFW				Refactored to NWNX4 1.08.
	071507		KFW				Widened BICFunctions.exe command-line.
	******		Nemrod			New function: Archive Character.
	---------------------------------------------------------------------------------------

*/

#if !defined(CHARACTER_H_INCLUDED)
#define CHARACTER_H_INCLUDED

#define DLLEXPORT extern "C" __declspec(dllexport)
#define CHARACTER_VERSION "0.0.1.2"

#include "windows.h"
#include "stdio.h"
#include "azroles.h"
#include "ObjBase.h"
#include "shlobj.h"
#include "../plugin.h"
#include "../../misc/log.h"
#include "wx/tokenzr.h"
#include "wx/hashset.h"
#include "wx/filefn.h"
#include "wx/fileconf.h"
#include "strsafe.h"

class Character : public Plugin {

public:

	Character( );
	~Character( );

	bool Init( PCHAR nwnxhome );

	INT GetInt( PCHAR psFunction, PCHAR psParam1, INT nParam2 ){ return( 0 ); }
	VOID SetInt( PCHAR psFunction, PCHAR psParam1, INT nParam2, INT nValue ){ };
	FLOAT GetFloat( PCHAR psFunction, PCHAR psParam1, INT nParam2 ){ return( 0.0 ); }
	VOID SetFloat( PCHAR psFunction, PCHAR psParam1, INT nParam2, FLOAT fValue ){ };
	
	VOID SetString( PCHAR psFunction, PCHAR psParam1, INT nParam2, PCHAR psValue );
	PCHAR GetString( PCHAR psFunction, PCHAR psParam1, INT nParam2 );

	VOID GetFunctionClass( TCHAR * fClass );

	VOID WriteNWN2String( PCHAR pszNWN2String );							// This method will write text to NWN2's NWNXGetString(..) return value.
	VOID UpdateCharacter( PCHAR pszFilename, PCHAR pszCommands );			// This method will perform changes to the character.
	VOID DelBICFilename( PCHAR pszBICFilename );							// This method will delete the specified BIC filename.
	VOID ArchiveBICFilename( PCHAR pszBICFilename );						// This method will archive the specified BIC filename.
	VOID GetBICFilename( PCHAR pszPlayerVault );							// This method will get the last saved BIC filename from the player's vault.

private:

	wxLogNWNX * logger;
	wxFileConfig *config;

	STARTUPINFO StartupInfo;						// Process StartUp Information structure.
	PROCESS_INFORMATION ProcessInfo;				// Process Information structure.
	CHAR szCommandLine[ 2000 ];						// BIC Function's command-line (2000 bytes).

	wxString sBicFunctions;
	wxString sServervault;

};

#endif