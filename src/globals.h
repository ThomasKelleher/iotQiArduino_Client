/*
	Copyright (c) 2016 LooUQ Incorporated.

	The GNU Licence(GNU)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <arduino.h>
#include <stdint.h>
#include <iotQiTypes.h>
#include <iotQiDiagnostics.h>


/* -----------------------------------------------------------------------------------
* # IotQi Constants #
* --------------------------------------------------------------------------------- */
#define DEVICEID_SIZE 37
#define DEVICECONNECTIONSTRING_SIZE 160
#define IOTQI_SCHEMA_VERSION	"v-1.0"
#define IOTQI_CODE_VERSION "iotQi MQTT v1.0"
#define NETWORKINFO_SIZE 128
#define NETWORKTYPE_SIZE 16
#define EVENTNAME_SIZE 80
#define EVENTVALUE_SIZE 80

/* ------------------------------------------------------------------------------------
* <> Status Codes (use http conventions) <>
* ---------------------------------------------------------------------------------- */
#define HTTP_OK 200
#define HTTP_ERROR 500
#define HTTP_NOTFOUND 404

/* ------------------------------------------------------------------------------------
* & IotQi information &
* ---------------------------------------------------------------------------------- */
extern char deviceId[];
extern char deviceConnectionString[];
extern const char* versionInfo;

/* ------------------------------------------------------------------------------------
* @ Network properties @
* ---------------------------------------------------------------------------------- */
extern char networkType[];
extern char networkString[];

/* ------------------------------------------------------------------------------------
* ^ Delegates ^
* ---------------------------------------------------------------------------------- */
extern USERMODEL_GETCOMMANDS fptrUserGetCommands;
extern USERMODEL_COMMANDMSGCALLBACK fptrUserCmdMsgCallback;


#endif  /*_GLOBALS_H */
