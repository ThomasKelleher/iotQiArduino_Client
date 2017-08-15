// Copyright (c) LooUQ Incorporated. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef ARDUINO
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/platform.h"
#include "serializer.h"
#include "iothub_client_ll.h"
#include "iothubtransporthttp.h"
#endif
#include "sdk/schemaserializer.h"

#include <iotQi.h>
#include "userModel.h"

/* ------------------------------------------------------------------------------------------------------------------------------------------
*  Add any required headers for project specific functionality.  These headers are likely to be required in the UserModel.c file as well.
------------------------------------------------------------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------------------------------------------ */

/* ==========================================================================================================================================
*  Model Type            Description
*  -------------------   ---------------------------------------------
*  double                double precision floating point number
*  int                   32 bit integer
*  float                 single precision floating point number
*  long                  long integer
*  int8_t                8 bit integer
*  int16_t               16 bit integer
*  int32_t               32 bit integer
*  int64_t               64 bit integer
*  bool                  boolean
*  ascii_char_ptr        ASCII string
*  EDM_DATE_TIME_OFFSET  date time offset
*  EDM_GUID              GUID
*  EDM_BINARY            binary
*  DECLARE_STRUCT        complex data type

https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-device-sdk-c-serializer
========================================================================================================================================== */


/* Model Definition ====================================================================================================================== */

BEGIN_NAMESPACE(UserModelNamespace);

DECLARE_MODEL(UserModel,
WITH_DATA(ascii_char_ptr, DeviceId),	// @Property
WITH_DATA(int, WindSpeed),	// @Property
WITH_ACTION(TurnFanOn),	// ()Function
WITH_ACTION(TurnFanOff),	// ()Function
WITH_ACTION(SetAirResistance, int, Position)	// ()Function
);

END_NAMESPACE(UserModelNamespace);


/* Model variables ====================================================================================================================== */
UserModel* userModel;

/* Add any global or persistant model variables here */

/* Model command methods ================================================================================================================= */

EXECUTE_COMMAND_RESULT TurnFanOn(UserModel* device)
{
    (void)device;
    (void)printf("Turning fan on.\r\n");
	digitalWrite(13, HIGH);
    return HTTP_OK;
}

EXECUTE_COMMAND_RESULT TurnFanOff(UserModel* device)
{
    (void)device;
    (void)printf("Turning fan off.\r\n");
	digitalWrite(13, LOW);
    return HTTP_OK;
}

EXECUTE_COMMAND_RESULT SetAirResistance(UserModel* device, int Position)
{
    (void)device;
    (void)printf("Setting Air Resistance Position to %d.\r\n", Position);
	unsigned char* buffer; size_t size;
	if (SERIALIZE(&buffer, &size, device->WindSpeed))
	{
		return HTTP_ERROR;
	}
	else
	{
		SetResponseBody(buffer, size);
	}
    return HTTP_OK;
}

// ------------------------------------------------------------------------------------------------------------------------------------------

TelemetryTemplate WindSpeed(STRING_HANDLE* sample_data)
{
    int avgWindSpeed = 10;
    userModel->DeviceId = "IotqiDevice1";
    userModel->WindSpeed = avgWindSpeed + (rand() % 4 + 2);
    
    unsigned char* msgBuffer;
    size_t msgSize;

    if ( SERIALIZE(&msgBuffer, &msgSize, userModel->DeviceId, userModel->WindSpeed) )
    {
        (void)printf("***Failed to serialize model to message\r\n");
    }
    else
    {
		*sample_data = STRING_construct_n(msgBuffer, msgSize);
		free(msgBuffer);
		//(void)printf("WindSpeed: serialization result=%.*s size=%d \r\n", STRING_length(*sample_data), STRING_c_str(*sample_data), msgSize);
		return STRING_construct_sprintf("Wind Speed:%d", userModel->WindSpeed);
    }
}

AlertTemplate WindAlert(STRING_HANDLE* alert_data)
{
	userModel->WindSpeed = 25;

	unsigned char* msgBuffer;
	size_t msgSize;

	if (SERIALIZE(&msgBuffer, &msgSize, userModel->WindSpeed))
	{
		(void)printf("*** Failed to serialize model to message\r\n");
	}
	else
	{
		*alert_data = STRING_construct_n(msgBuffer, msgSize);
		free(msgBuffer);
	}
}


/* iotQi Required ======================================================================================================
 * The following methods are required to enable your model code to interface with iotQi
 * ================================================================================================================== */

/* initialize and destroy model objects */

IOTQIMODEL_RESULT initUserModel()
{
    /* initialize any parts of your model you need here */
    int avgWindSpeed = 10;

	/* Seed the Random Number Generator with Time */
	srand((unsigned int)time(NULL));


    /* iotQi needs to have your "model" initialized too*/
	userModel = CREATE_MODEL_INSTANCE(UserModelNamespace, UserModel);
	if (userModel == NULL)
	{
		(void)printf("Failed on create user-defined model\r\n");
        return MODEL_ERROR;
	}
    (void)printf("=== User-Defined Model Initialized ===\r\n\n");
    return MODEL_OK;
}

void deinitUserModel()
{
    (void)printf("Deconstructing User-Defined Model\r\n");
	DESTROY_MODEL_INSTANCE(userModel);
}

/* -----------------------------------------------------------------------------------------------------------------------
 * The next two methods link your user model commands back to iotQi
 * -------------------------------------------------------------------------------------------------------------------- */

IOTQIMODEL_RESULT UserModel_GetCommands(STRING_HANDLE commandsMeta)
{
    #if defined(IOTQI_DEBUG)
    (void)printf("> Fetching user-defined commands\r\n");
    #endif

	if (SchemaSerializer_SerializeCommandMetadata(GET_MODEL_HANDLE(UserModelNamespace, UserModel), commandsMeta) != SCHEMA_SERIALIZER_OK)
	{
		return MODEL_ERROR;
	}
	return MODEL_OK;
}


EXECUTE_COMMAND_RESULT UserModel_CommandMsgCallback(const char* cmdBuffer)
{
    #ifdef IOTQI_DEBUG
    (void)printf("> Invoking user command: %s \r\n", cmdBuffer);
    #endif

    EXECUTE_COMMAND_RESULT result = EXECUTE_COMMAND(userModel, cmdBuffer);
}



