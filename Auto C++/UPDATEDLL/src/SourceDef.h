//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   SourceDef.h
//  Version     :   1.0
//  Creater     :   
//  Date        :   2002-12-19 16:54:27
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _UPDATE_SOURCE_DEFINE_H_
#define _UPDATE_SOURCE_DEFINE_H_

#define defIDS_FILE_ERROR_FORMAT			"The file %s does not exist or is corrupted!"
#define defIDS_ERROR_INDEX					"The information file index in the selected upgrade folder has been destroyed. Please download the correct file via Internet upgrade, or notify the network administrator to download the correct file via Internet upgrade."
#define defIDS_INDEX_ERROR					"An error occurred while parsing the downloaded upgrade information file. Please select another upgrade server to re-upgrade."
#define defIDS_UPDATE_SELF_SUCCESS			"Upgrade program first updates itself successfully!"
#define defIDS_UPDATE_SELF_FAILED			"Upgrade program priority update itself failed! Please confirm whether the directory where the download file is stored can be written, or whether UpdateSelf.DAT in this directory can be modified, and then upgrade again!"
#define defIDS_UPDATE_FAILED				"Failed to update %s to %s! Please upgrade again!"
#define defIDS_NEED_RESET					"Update %s to %s succeeded and needs to be restarted"
#define defIDS_UPDATE_SUCCESS				"Update %s to %s success"
#define defIDS_COPY_TEMPFILE_FAILED			"Failed to update %s to %s failed to copy temporary files"
#define defIDS_PROCESS_SHARING_FAILED		"Failed to update %s to %s failed to process conflicting files"
#define defIDS_UPDATE_SUCCESS_NEED_RESET	"Update %s to %s Success: File conflict, need to restart"
#define defIDS_UPDATE_SHARING_SUCCESS		"Update %s to %s Success: file conflict, need to be restarted to complete the update"
#define defIDS_UPDATE_FINISH_FAILED			"Failed to update the file, please upgrade again!"
#define defIDS_UPDATE_FINISH_SUCCESS		"Update the file successfully, congratulations!"
#define defIDS_NOTIFY_REUPDATE				"In order to ensure the normal operation of the program, please re-upgrade and then run. \r\nYou can choose to upgrade from LAN or local, select the directory saved by the file download to upgrade, no need to re-download the file."
#define defIDS_NEED_REBOOT					"To update the file you are using, please restart your computer."
#define defIDS_DOWNLOAD_FILE_FAILED			"Failed to download file %s to %s"
#define defIDS_DOWNLOAD_FILE_SUCCESSFUL		"Download file %s to %s succeeded"
#define defIDS_INVALID_USERCHECK			"Sorry, the serial number has not been verified by the server, your serial number may be incomplete or due to network transmission error, please upgrade later!"
#define defIDS_PIRATIC_SERIAL_NUMBER		"Sorry, your serial number may be illegally used by other users and cannot be verified by upgrade"
#define defIDS_INHIBITIVE_SERIAL_NUMBER		"Sorry, your serial number is already disabled."
#define defIDS_CONNECT_SERVER_FAILED		"Sorry, due to the server being busy or updating, please upgrade later!"
#define defIDS_CHECK_SERIAL_NUMBER_ERROR	"I am very sorry, the server could not be connected due to busy network, please upgrade later!"
#define defIDS_HTTP_CHECK_FAILED			"Failed to authenticate user with HTTP!"
#define defIDS_USE_HTTP_CHECK				"Failed with UDP authentication, using HTTP authentication."
#define defIDS_HTTP_CHECK_SUCCESSFUL		"Authenticate user identity with HTTP!"
#define defIDS_UDP_CHECK_SUCCESSFUL			"Verify user identity with UDP successfully!"
#define defIDS_INDEX_ERRORANALYSIS			"An error occurred while parsing the downloaded upgrade information file. Please select another upgrade server to re-upgrade."


#endif  //_UPDATE_SOURCE_DEFINE_H_