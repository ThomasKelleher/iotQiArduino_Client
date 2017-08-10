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

#ifndef _IOTQITYPES_h
#define _IOTQITYPES_h

#include <AzureIoTHub.h>
#include <AzureIoTUtility.h>

/* -----------------------------------------------------------------------------------
* = Enumerations =
* --------------------------------------------------------------------------------- */

/** @brief MODEL_RESULT_VALUES enumerations
*/
#define MODEL_RESULT_VALUES   \
    MODEL_OK,                 \
    MODEL_ERROR

/** @brief Enumeration of results representing
*  the Model function status
*/
DEFINE_ENUM(IOTQIMODEL_RESULT, MODEL_RESULT_VALUES)

/** @brief IotQiResult enumerations
*	for iotQi function results
*/
enum IotQiResult
{
	iotqiOk,
	iotqiError,
	iotqiInvalidArg
};

/* -----------------------------------------------------------------------------------
* - TypeDefinitions -
* --------------------------------------------------------------------------------- */

/** @brief Definitions of function pointers
*	for User model function handles
*/
typedef IOTQIMODEL_RESULT(*USERMODEL_GETCOMMANDS)(STRING_HANDLE commandsMeta);
typedef EXECUTE_COMMAND_RESULT(*USERMODEL_COMMANDMSGCALLBACK)(const char* cmdBuffer);

#endif // !_IOTQITYPES_h
