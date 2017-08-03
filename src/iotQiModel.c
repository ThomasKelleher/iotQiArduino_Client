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
#include <iotQiModel.h>

#ifdef MBED_BUILD_TIMESTAMP
#include "certs.h"
#endif // MBED_BUILD_TIMESTAMP

/* ------------------------------------------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------------------------------------------ */
#pragma region SerializeModel
BEGIN_NAMESPACE(IotqiNamespace);

  DECLARE_STRUCT(DeviceInfo,
    ascii_char_ptr, deviceId,
#if (NETWORK_TYPE != WIFI)
	ascii_char_ptr, connectionString,
#endif // NETWORK_TYPE != WIFI
    ascii_char_ptr, iotqiCodeVersion,
    ascii_char_ptr, iotqiSchemaVersion
  );  

  DECLARE_STRUCT(NetworkInfo,
    ascii_char_ptr, networkType,
    ascii_char_ptr, networkSpecifics
  );
  
  DECLARE_STRUCT(CommandInfo,
    ascii_char_ptr_no_quotes, iotqiCommands,
    ascii_char_ptr_no_quotes, userCommands
  );
  
  DECLARE_MODEL(IotqiModel,
    WITH_DATA(DeviceInfo, deviceInfo),
    WITH_DATA(NetworkInfo, networkInfo),
    WITH_DATA(CommandInfo, commandInfo),
  
    WITH_ACTION(getdeviceinfo),
    WITH_ACTION(getnetworkinfo),
    WITH_ACTION(getcommandinfo)
  );

END_NAMESPACE(IotqiNamespace);
#pragma endregion

EXECUTE_COMMAND_RESULT getdeviceinfo(IotqiModel* iotqiContext);
EXECUTE_COMMAND_RESULT getnetworkinfo(IotqiModel* iotqiContext);
EXECUTE_COMMAND_RESULT getcommandinfo(IotqiModel* iotqiContext);

IotqiModel* iotqiContext;

// ============================================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------------------------
// Model Setup
// ---------------------------------------------------------------------------------------------------------------------------------------

IOTQIMODEL_RESULT initIotqiModel()
{
	iotqiContext = CREATE_MODEL_INSTANCE(IotqiNamespace, IotqiModel);
	if (iotqiContext == NULL)
	{
#ifdef IOTQI_DEBUG
		(void)printf("***Failed on to create iotQi device model\r\n");
#endif
		return MODEL_ERROR;
	}
#ifdef IOTQI_DEBUG
	(void)printf("iotQi Device Model Initialized\r\n");
#endif
	return MODEL_OK;
}

void deinitIotqiModel()
{
	DESTROY_MODEL_INSTANCE(iotqiContext);
}

// ---------------------------------------------------------------------------------------------------------------------------------------
// Model Callable Functions
// ---------------------------------------------------------------------------------------------------------------------------------------

EXECUTE_COMMAND_RESULT getdeviceinfo(IotqiModel* iotqiContext)
{
	(void)iotqiContext;
	(void)printf("-\tReponding to DeviceId request...\r\n");

	unsigned char* buffer;
	size_t size;

	iotqiContext->deviceInfo.deviceId = (char*)deviceId;
	iotqiContext->deviceInfo.iotqiCodeVersion = IOTQI_CODE_VERSION;
	iotqiContext->deviceInfo.iotqiSchemaVersion = IOTQI_SCHEMA_VERSION;

	if (SERIALIZE(&buffer, &size, iotqiContext->deviceInfo) != CODEFIRST_OK)
	{
		return HTTP_ERROR;
	}
	else
	{
		SetResponseBody(buffer, size);
	}
	return HTTP_OK;
}

EXECUTE_COMMAND_RESULT getnetworkinfo(IotqiModel* iotqiContext)
{
	(void)iotqiContext;

	unsigned char* buffer;
	size_t size;

	iotqiContext->networkInfo.networkType = networkType;
	iotqiContext->networkInfo.networkSpecifics = networkString;

	if ( SERIALIZE(&buffer, &size, iotqiContext->networkInfo) != CODEFIRST_OK )
	{
		return HTTP_ERROR;
	}
	else
	{
		SetResponseBody(buffer, size);
	}
	return HTTP_OK;
}

EXECUTE_COMMAND_RESULT getcommandinfo(IotqiModel* iotqiContext)
{
	(void)iotqiContext;
#ifdef IOTQI_DEBUG
	(void)printf("Reponding to getcommandinfo request...\r\n");
#endif

	/* iotQi makes use of Azure SDK strings functionalitiy */
	STRING_HANDLE iotqiCommandsMeta;
	STRING_HANDLE userCommandsMeta;
	iotqiCommandsMeta = STRING_new();
	userCommandsMeta = STRING_new();

	if (iotqiCommandsMeta == NULL || userCommandsMeta == NULL)
	{
#ifdef _DEBUG_MODEL
		(void)printf("Failed on creating strings for commands metadata\r\n");
#endif
	}
	else
	{
		/* registered callback (see global:fptrGetCommand and .INO iotqiSetCallbacks()
		\/          */
		if (IotqiModel_GetCommands(iotqiCommandsMeta) != iotqiOk || (((*fptrUserGetCommands)(userCommandsMeta)) != iotqiOk))
		{
#ifdef _DEBUG_MODEL
			(void)printf("Failed serializing command metadata\r\n");
#endif
		}
		else
		{
			unsigned char* buffer;
			size_t size;
			iotqiContext->commandInfo.iotqiCommands = (char*)STRING_c_str(iotqiCommandsMeta);
			iotqiContext->commandInfo.userCommands = (char*)STRING_c_str(userCommandsMeta);

			/* Here is the actual serialization of the device message */
			if (SERIALIZE(&buffer, &size, iotqiContext->commandInfo) != CODEFIRST_OK)
			{
#ifdef _DEBUG_MODEL
				(void)printf("Failed serializing command response\r\n");
#endif
				return HTTP_ERROR;
			}
			else
			{
				SetResponseBody(buffer, size);
			}
		}
		STRING_delete(iotqiCommandsMeta);
		STRING_delete(userCommandsMeta);
	}
	return HTTP_OK;
}

// ---------------------------------------------------------------------------------------------------------------------------------------
// Model Functions
// ---------------------------------------------------------------------------------------------------------------------------------------

EXECUTE_COMMAND_RESULT IotqiModel_InvokeCommand(const char* cmdBuffer)
{
    #if defined(IOTQI_DEBUG)
    (void)printf("> Invoking iotQi command: %s \r\n", cmdBuffer);
    #endif

    EXECUTE_COMMAND_RESULT result = EXECUTE_COMMAND(iotqiContext, cmdBuffer);
}

IOTQIMODEL_RESULT IotqiModel_GetCommands(STRING_HANDLE commandsMeta)
{
#if defined(IOTQI_DEBUG)
	(void)printf("> Fetching iotQi commands\r\n");
#endif

	if (SchemaSerializer_SerializeCommandMetadata(GET_MODEL_HANDLE(IotqiNamespace, IotqiModel), commandsMeta) != SCHEMA_SERIALIZER_OK)
	{
		return MODEL_ERROR;
	}
	return MODEL_OK;
}


// ============================================================================================================================================
// Message Templates
// ============================================================================================================================================

IOTQIMODEL_RESULT TEMPLATE_deviceStarted(char** sendBuffer, size_t* size)
{
	iotqiContext->deviceInfo.deviceId = (char*)deviceId;
	iotqiContext->deviceInfo.iotqiCodeVersion = IOTQI_CODE_VERSION;
	iotqiContext->deviceInfo.iotqiSchemaVersion = IOTQI_SCHEMA_VERSION;
	iotqiContext->networkInfo.networkType = networkType;
	iotqiContext->networkInfo.networkSpecifics = networkString;

	if ( SERIALIZE(sendBuffer, size, iotqiContext->deviceInfo, iotqiContext->networkInfo) != CODEFIRST_OK)
	{
		(void)printf("*** Error serializing device start message ***\r\n");
		return MODEL_ERROR;
	}
	else
	{
#ifdef _DEBUG_MODEL
		(void)printf("DeviceStartMsg: %.*s \r\n", *size, *sendBuffer);
#endif
		return MODEL_OK;
	}
}

//IOTQIMODEL_RESULT TEMPLATE_<message_name>()
//{
//
//}