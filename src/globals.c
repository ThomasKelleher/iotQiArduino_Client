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

#include <globals.h>

/* ------------------------------------------------------------------------------------
 * Allocate and initialize Network globals
 * ---------------------------------------------------------------------------------- */
char networkType[NETWORKTYPE_SIZE] = "<network type>\0";
char networkString[NETWORKINFO_SIZE] = "<network info>\0";

/* ------------------------------------------------------------------------------------
* Allocate and initialize iotQi globals
* ---------------------------------------------------------------------------------- */
char deviceId[DEVICEID_SIZE] = "\0";
char deviceConnectionString[DEVICECONNECTIONSTRING_SIZE] = "\0";
const char* iotqiCodeVersion = IOTQI_CODE_VERSION;
const char* iotqiSchemaVersion = IOTQI_SCHEMA_VERSION;