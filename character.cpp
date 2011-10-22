/***************************************************************************
    NWNX Character - Character functions plugin
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
    101108      KFW			    Compiled for NWNX4 version 1.09!
    101108      KFW			    Deprecated the GetBICFilename function.
    101108      KFW			    Added the SetWing function.
    101108      KFW			    Added the SetTail function.
    101208      KFW			    Added the SetHairTint function.
    101208      KFW			    Added the SetHeadTint function.
    101208      KFW				Added the SetBodyTint function.
    080509      FoamBats4All	Added ini file to set BicFunction's path. Rebundled.
	092510		FoamBats4All	Added ini file setting for servervault path.
	---------------------------------------------------------------------------------------

*/

// Defines.
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500

// Includes.
#include "character.h"

/***************************************************************************
    NWNX and DLL specific functions
***************************************************************************/

Character * plugin;

DLLEXPORT Plugin * GetPluginPointerV2( ){
	return( plugin );
}


BOOL APIENTRY DllMain( HMODULE hModule, DWORD dwReason, LPVOID lpReserved ){

	switch( dwReason ){

		case DLL_PROCESS_ATTACH:{
			plugin = new Character( );
			TCHAR szPath[ MAX_PATH ];
			GetModuleFileName( hModule, szPath, MAX_PATH );
			plugin->SetPluginFullPath( szPath );
			break;
								}
		case DLL_PROCESS_DETACH:{
			delete plugin;
			break;
								}
		default:
			break;

	}

	return( TRUE );

}


/***************************************************************************
    Implementation of Character Plugin
***************************************************************************/

Character :: Character( ){

	header		=	"NWNX Character Plugin V.";
	header	   +=	CHARACTER_VERSION;
	header	   +=	"\n(c) 2006 by Ingmar Stieger (Papillon)\nWith edits by kungfoowiz, Nemrod, and FoamBats4All\nvisit us at http://www.nwnx.org\n";

	description	=	"This plugin modifies character (BIC) files.";

	subClass	=	"CHARACTER";

	version		=	CHARACTER_VERSION;

	return;

}


Character :: ~Character( ){

	wxLogMessage( wxT( "* Plugin unloaded." ) );

}


bool Character :: Init( PCHAR ps_nwnxhome ){

	assert( GetPluginFileName() );

	// Log file.
	wxString logfile( ps_nwnxhome ); 
	logfile.append( wxT( "\\" ) );
	logfile.append( GetPluginFileName( ) );
	logfile.append( wxT( ".txt" ) );
	logger = new wxLogNWNX( logfile, wxString( header.c_str( ) ) );

	// Ini file.
	wxString inifile( ps_nwnxhome ); 
	inifile.append( wxT( "\\" ) );
	inifile.append( GetPluginFileName() );
	inifile.append( wxT( ".ini" ) );
	wxLogTrace( TRACE_VERBOSE, wxT( "* reading inifile %s" ), inifile );
	config = new wxFileConfig( wxEmptyString, wxEmptyString, inifile, wxEmptyString, wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_NO_ESCAPE_CHARACTERS );

	// Read setting: BicFunctions 
	if ( !config->Read( wxT( "bicfunctions" ), &sBicFunctions ) ) {
		wxLogMessage( wxT( "* BicFunctions path not set in INI file. " ) );
		sBicFunctions = wxT( "BicFunctions.exe" );
		wxLogMessage( wxT( "* Using default path: '%s'"), sBicFunctions );
	} else {
		wxLogMessage( wxT( "* BicFunctions path set: '%s'"), sBicFunctions );
	}

	// Read setting: Servervault
	if ( !config->Read( wxT( "servervault" ), &sServervault ) ) {
		wxLogMessage( wxT( "* Servervault path not set in INI file." ) );
		char buffer[MAX_PATH];
		if ( SUCCEEDED( SHGetSpecialFolderPath( NULL, buffer, CSIDL_MYDOCUMENTS, FALSE ) ) ) {
			sServervault = wxString( buffer ) + wxString( "\\Neverwinter Nights 2\\servervault\\" );
			wxLogMessage( wxT( "* Guessing servervault path: '%s'"), sServervault );
		} else {
			wxLogMessage( wxT( "* Servervault could not be parsed. Please set it in the ini file." ) );
			return ( FALSE );
		}
	} else {
		wxLogMessage( wxT( "* Servervault path set: '%s'"), sServervault );
	}

	wxLogMessage( wxT( "* Plugin initialized." ) );
	return( TRUE );

}


VOID Character :: GetFunctionClass( TCHAR * fClass ){

	_tcsncpy_s( fClass, 128, wxT( "CHARACTER" ), 9 );

	return;

}


VOID Character :: SetString( PCHAR psFunction, PCHAR psParam1, INT nParam2, PCHAR psValue ){

	// Log the NWNX4 string for debugging purposes.
	wxLogMessage(
		wxT( "* Plugin SetString( psFunction = %s, psParam1 = %s, nParam2 = %d, psValue = %s )" ),
		psFunction,
		psParam1,
		nParam2,
		psValue );

	// Variables.
	wxString function( psFunction, wxConvUTF8 );
	wxString parameter1( psParam1, wxConvUTF8 );
	wxString parameter3( psValue, wxConvUTF8 );
	parameter1.Replace("\\","/");

	
	// Check for a function string.
	if(			function == wxT( "" ) ){
		wxLogMessage( wxT( "* Function not specified." ) );
		return;
	}

	// Execute the desired function.

	// BootPlayerAndUpdateCharacter( .. ).
	if(			function == wxT( "UPDATE" ) )
		UpdateCharacter( ( PCHAR ) parameter1.c_str( ), ( PCHAR ) parameter3.c_str( ) );
	// DeleteCharacter( .. ).
	else if(	function == wxT( "DEL" ) )
		DelBICFilename( ( PCHAR ) parameter1.c_str( ) );
	// ArchiveCharacter( .. ).
	else if(	function == wxT( "ARCHIVE" ) )
		ArchiveBICFilename( ( PCHAR ) parameter1.c_str( ) );

	return;

}


PCHAR Character :: GetString( PCHAR psFunction, PCHAR psParam1, INT nParam2 ){

	// Log the NWNX4 string for debugging purposes.
	wxLogMessage( wxT( "* Plugin GetString( psFunction = %s, psParam1 = %s, nParam2 = %d ) " ), psFunction, psParam1, nParam2 );

	// Variables.
	wxString wxRequest( psFunction, wxConvUTF8 );
	wxString function( psFunction, wxConvUTF8 );
	wxString parameter1( psParam1, wxConvUTF8 );
	parameter1.Replace("/","\\");


	// Check for a function string.
	if ( function == wxT( "" ) ){
		wxLogMessage( wxT( "* Function not specified." ) );
		return( NULL );
	}

	// Execute the desired function.
    if ( function == wxT( "GETBIC" ) ) {
		// GetBICFilename( ... )
        wxLogMessage( wxT( "* Function unavailable." ) );
	} else if ( function == wxT( "GETSERVERVAULT" ) ) {
		// GetServervault( ... )
		return ( ( PCHAR ) sServervault.c_str() );
    } else {
		// Name, Description, Subclass, Version query.
		// Process generic functions.
		wxString query = ProcessQueryFunction( function.c_str( ) );
		if ( query != wxT( "" ) ) {
			sprintf_s( returnBuffer, MAX_BUFFER, "%s", query );
		} else {
			wxLogMessage( wxT( "* Unknown function '%s' called." ), function );
			return( NULL );
		}
	
	}

	return( returnBuffer );

}


// This method will write text to NWN2's NWNXGetString(..) return value.
VOID Character :: WriteNWN2String( PCHAR pszNWN2String ){

	// Copy the desired string into NWN2's one.
	nwnxcpy( returnBuffer, pszNWN2String );

	return;

}


// This method will perform changes to the character.
VOID Character :: UpdateCharacter( PCHAR pszFilename, PCHAR pszCommands ){

	// Initialize the process structures.
	ZeroMemory( &StartupInfo, sizeof( StartupInfo ) );
	StartupInfo.cb = sizeof( StartupInfo );
	ZeroMemory( &ProcessInfo, sizeof( ProcessInfo ) );

	// Read the path to the BicFunctions file.


	// Form the BIC function's command-line.
	StringCbCopy( szCommandLine, 2000, sBicFunctions );
	StringCbCat( szCommandLine, 2000, " \"" );
	StringCbCat( szCommandLine, 2000, pszFilename );
	StringCbCat( szCommandLine, 2000, "\" " );
	StringCbCat( szCommandLine, 2000, pszCommands );

	// Keep a logfile of precise commands.
	wxLogMessage( "* UpdateCharacter( %s )", szCommandLine );
	
	// Execute the BIC function.
	if( !CreateProcess( NULL, szCommandLine, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &StartupInfo, &ProcessInfo ) ) {
			wxLogMessage( "* CreateProcess( .. ) failed!" );
			return;
	}

	// Wait until it completes.
	WaitForSingleObject( ProcessInfo.hProcess, INFINITE );
	// Cleanup.
	CloseHandle( ProcessInfo.hProcess );
	CloseHandle( ProcessInfo.hThread );

	return;

}


// This method will delete the specified BIC filename.
VOID Character :: DelBICFilename( PCHAR pszBICFilename ){

	// Variables.
	wxString sStatus = wxT( "" );


	// Delete the specified BIC filename.
	if( DeleteFile( pszBICFilename ) )	sStatus = "successful";
	else								sStatus = "failed";

	// Keep a logfile of precise commands.
	wxLogMessage( "* DelBICFilename( %s ) %s.", pszBICFilename, sStatus );

	return;

}


// This method will archive the specified BIC filename.
VOID Character :: ArchiveBICFilename( PCHAR pszBICFilename ){

	// Variables.
	wxString sStatus = wxT( "" );
	wxString pszBICBackupFilename = pszBICFilename;

	// Rename the specified BIC filename's extension.
	pszBICBackupFilename.Replace( ".bic", ".bak" );

	if( CopyFile( pszBICFilename, pszBICBackupFilename, false ) )	sStatus = "successful";
	else															sStatus = "failed";

	// Keep a logfile of precise commands.
	wxLogMessage( "* ArchiveBICFilename( %s ) to backup BIC filename = %s, %s.", pszBICFilename, pszBICBackupFilename, sStatus );

	// Delete the specified BIC filename.
	if( DeleteFile( pszBICFilename ) )	sStatus = "successful";
	else								sStatus = "failed";

	// Keep a logfile of precise commands.
	wxLogMessage( "* Purge old BIC filename %s, %s.", pszBICFilename, sStatus );
	return;

}

// This method will get the last saved BIC filename from the player's vault.
/*VOID Character :: GetBICFilename( PCHAR pszPlayerVault ){

	// Variables.
	wxString FilenameHighest, FilenameCurrent;
	time_t FileModTimeHighest = 0, FileModTimeCurrent = 0;


	wxLogMessage( "Vault = %s", pszPlayerVault );
	// Seek out the latest BIC filename.
	FilenameCurrent = wxFindFirstFile( pszPlayerVault );
	while( !FilenameCurrent.empty( ) ){

		// Filename found with a higher file modification time.
		if( ( FileModTimeCurrent = wxFileModificationTime( FilenameCurrent ) ) > FileModTimeHighest ){
			FilenameHighest = FilenameCurrent;
			FileModTimeHighest = FileModTimeCurrent;
		}

		// Get the next filename.
		FilenameCurrent = wxFindNextFile( );

	}

	// Keep a logfile of precise commands.
	wxLogMessage( "* %s = GetBICFilename( %s )", FilenameHighest.c_str( ), pszPlayerVault );

	// Pass the filename data back to the NWN2 caller.
	WriteNWN2String( ( PCHAR ) FilenameHighest.c_str( ) );

	return;

}*/