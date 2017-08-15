// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


#ifdef MBED_BUILD_TIMESTAMP
#include "certs.h"
#endif // MBED_BUILD_TIMESTAMP

#include <Arduino.h>
#include <globals.h>
#include <iotQiModel.h>
#include <iotQi.h>
#include <iotQiUtility.h>
#include <azure_c_shared_utility\platform.h>

// =======================================================================================================================================
iotQiClient iotqiClient;

// set global pointers
USERMODEL_GETCOMMANDS fptrUserGetCommands;
USERMODEL_COMMANDMSGCALLBACK fptrUserCmdMsgCallback;
struct CommandResponse
{
	char* buffer;
	size_t buffer_size;
	int success_code;
}static CmdResponse;

// static members
iotQiNetwork* iotQiClient::networkTransport = nullptr;
unsigned int iotQiClient::msgTrackingId = 0;
IOTHUB_CLIENT_LL_HANDLE iotQiClient::iotHubClientHandle = 0;

// ---------------------------------------------------------------------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------------------------------------------------------------------

iotQiClient::iotQiClient()
	:
	iotqiInitState(Uninitialized),
	lastTelemetry(0)
{
	CmdResponse.buffer = NULL;
	CmdResponse.buffer_size = 0;
	CmdResponse.success_code = 0;
}

iotQiClient::~iotQiClient()
{
}

bool iotQiClient::Init(const char* iotqiHubConnection, iotQiNetwork &networkClient)
{
	Debug::WriteLine("> Starting iotQi...");

	networkTransport = &networkClient;
	SetGlobalDeviceIdFromConnectionString(iotqiHubConnection);
	Debug::Write("init(): DeviceId: "); Debug::WriteLine(deviceId);

	networkTransport->init();
	networkTransport->begin();
	networkTransport->updateNetworkInfo();
	
	if (platform_init() != 0)
	{
		Debug::WriteLine("*** Failed to initialize platform ***");
	}
	else
	{
		iotqiInitState = PlatformInit;
		Debug::WriteLine("> iotQi Platform started");
		if (serializer_init(NULL) != SERIALIZER_OK)
		{
			Debug::WriteLine("*** Failed to initialize serializer library ***");
		}
		else
		{
			Debug::WriteLine("> iotQi Serializer started");
			iotqiInitState = SerializerInit;
			// TODO: Cutoff LoRa Radio

			iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(iotqiHubConnection, MQTT_Protocol);
			if (iotHubClientHandle == NULL)
			{
				Debug::WriteLine("*** Failed to create hub client ***");
			}
			else
			{
				iotqiInitState = IotHubClientHandleInit;

				#ifdef MBED_BUILD_TIMESTAMP
				// For mbed add the certificate information
				if (IoTHubClient_LL_SetOption(iotHubClientHandle, "TrustedCerts", certificates) != IOTHUB_CLIENT_OK)
				{
					//Debug::WriteLine("*** Failed to set option \"TrustedCerts\" ***");
				}
				#endif // MBED_BUILD_TIMESTAMP

				if ( IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, commandMessageCallback, NULL) != IOTHUB_CLIENT_OK )
				{
					Debug::WriteLine("*** Unable to register command receive callback ****");
				}
				else
				{
					iotqiInitState = MessageCallbackInit;
				}
			}
		}
	}

	if (iotqiInitState != MessageCallbackInit)
	{
		//Debug::WriteLine("*** Error initializing iotQi. Please verify both the time and connection string (in settings.h) are valid ***");
		this->deinit();
		return false;
	}

	//Debug::WriteLine("iotQi initialized.");
	return true;
}

bool iotQiClient::Begin()
{
	if (initIotqiModel() != MODEL_OK)
	{
		Debug::WriteLine("iotQi Model Failed to initialize");
		return false;
	}
	else
	{
		ALERT_sendDeviceStart();
		Debug::WriteLine("Sent Alert !");
		return true;
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------------
// Setters
// ---------------------------------------------------------------------------------------------------------------------------------------

void iotQiClient::SetCallbacks(USERMODEL_GETCOMMANDS getUserCommands, USERMODEL_COMMANDMSGCALLBACK userCommandMsgCallback)
{
	fptrUserGetCommands = getUserCommands;
	fptrUserCmdMsgCallback = userCommandMsgCallback;
}

extern "C" void SetResponseBody(unsigned char * buffer, size_t size)
{
	if (CmdResponse.buffer) delete[] CmdResponse.buffer;
	CmdResponse.buffer = new char[size + 1];
	CmdResponse.buffer_size = size;
	strncpy(CmdResponse.buffer, (char*)buffer, size);
	CmdResponse.buffer[size] = '\0';
	free(buffer);
}

// ---------------------------------------------------------------------------------------------------------------------------------------
// Communication
// ---------------------------------------------------------------------------------------------------------------------------------------

void iotQiClient::SendAlert(const char * alertName, const char * alertSubject)
{
	char* cpyName = new char[strlen(alertName)+1];
	char* cpySubject = new char[strlen(alertSubject)+1];

	memcpy(cpyName, alertName, strlen(alertName)+1);
	memcpy(cpySubject, alertSubject, strlen(alertSubject)+1);

	cleanStr(cpyName, "=", '~');
	cleanStr(cpySubject, "=", '~');
	char blankBuffer[] = { "{}" };
	sendAlertInternal(blankBuffer, 3, IotqiEventClass_User, cpyName, cpySubject);
	delete[] cpyName; delete[] cpySubject;
}

void iotQiClient::SendAlert(const char* alertName, const char* alertSubject, AlertDelegate a_temp)
{
	STRING_HANDLE serial_data;
	a_temp(&serial_data);

	char* cpyName = new char[strlen(alertName) + 1];
	char* cpySubject = new char[strlen(alertSubject) + 1];
	memcpy(cpyName, alertName, strlen(alertName) + 1);
	memcpy(cpySubject, alertSubject, strlen(alertSubject) + 1);
	cleanStr(cpyName, "=", '~');
	cleanStr(cpySubject, "=", '~');

	sendAlertInternal(STRING_c_str(serial_data), STRING_length(serial_data), IotqiEventClass_User, cpyName, cpySubject);
	delete[] cpyName; delete[] cpySubject;
	STRING_delete(serial_data);
}

void iotQiClient::SendTelemetry(char* eventName, TelemetryDelegate t_temp, unsigned long &prevTelemetry, int interval)
{
	if ( !interval || (millis() - prevTelemetry >= (interval * 1000)))
	{
		prevTelemetry = millis();
		STRING_HANDLE serial_data;
		STRING_HANDLE eventValue = t_temp(&serial_data);
		char* cpyName = new char[strlen(eventName)+1];
		memcpy(cpyName, eventName, strlen(eventName)+1);

		cleanStr(cpyName, "=", '~');

		sendTelemetryInternal(STRING_c_str(serial_data), STRING_length(serial_data), IotqiEventClass_User, cpyName, STRING_c_str(eventValue));
		delete[] cpyName;
		STRING_delete(serial_data);
		STRING_delete(eventValue);
	}
}

void iotQiClient::DoWork()
{
	/* send & check for commands */
	networkTransport->IotDoWork(iotHubClientHandle);
	ThreadAPI_Sleep(200);
}

// ---------------------------------------------------------------------------------------------------------------------------------------
// IoTQi Templated Messages
// ---------------------------------------------------------------------------------------------------------------------------------------

void iotQiClient::ALERT_sendDeviceStart()
{
	char* deviceStartAlert = nullptr; size_t alertLen = 0;
	TEMPLATE_deviceStarted(&deviceStartAlert, &alertLen);
	sendAlertInternal(deviceStartAlert, alertLen, IotqiEventClass_Iotqi, IotqiDeviceStart_AlertName, deviceId);
	free(deviceStartAlert);
}

// =======================================================================================================================================
// Internal functions for communicating with IoT Hub
// =======================================================================================================================================

void iotQiClient::sendMessage(const unsigned char* buffer, size_t size, const char* msgType, const char* eventClass, const char* eventName, const char* eventValue, int statusCode, const char* correlationId, const char* cloudInstanceId)
{
	IOTHUB_MESSAGE_HANDLE msgHandle = IoTHubMessage_CreateFromByteArray(buffer, size);
	if (msgHandle == NULL)
	{
		Debug::WriteLine("*** Unable to create a new IoTHubMessage for send");
	}
	else
	{
		char messageId[50];
		sprintf(messageId, "%s-%010ld", deviceId, millis());

		Debug::WriteIf(messageId, "@\t'message-id': ");	Debug::WriteLineIf(messageId, messageId);
		Debug::WriteIf(msgType, "@\t'message-type': ");	Debug::WriteLineIf( msgType, msgType );
		Debug::WriteIf(eventClass, "@\t'event-class': ");	Debug::WriteLineIf( eventClass, eventClass);
		Debug::WriteIf(eventName, "@\t'event-name': "); Debug::WriteLineIf( eventName, eventName);
		Debug::WriteIf(eventValue, "@\t'event-value': "); Debug::WriteLineIf( eventValue, eventValue);
		Debug::WriteIf(statusCode, "@\t'status-code': "); Debug::PrintLineIf( statusCode, statusCode);
		Debug::WriteIf(correlationId, "@\t'correlationId': "); Debug::WriteLineIf( correlationId, correlationId);
		Debug::WriteIf(cloudInstanceId, "@\t'cloudInstanceId': "); Debug::WriteLineIf( cloudInstanceId, cloudInstanceId);

		MAP_HANDLE propMap = IoTHubMessage_Properties(msgHandle);
		Map_AddOrUpdate(propMap, IotqiMsgProp_MessageId, messageId);
		Map_AddOrUpdate(propMap, IotqiMsgProp_iotQiVersion, IOTQI_SCHEMA_VERSION);
		Map_AddOrUpdate(propMap, IotqiMsgProp_MessageType, msgType);
		Map_AddOrUpdate(propMap, IotqiMsgProp_EventClass, eventClass);
		Map_AddOrUpdate(propMap, IotqiMsgProp_EventName, eventName);

		if (strcmp(msgType, IotqiMsgType_CommandResponse) == 0)
		{
			char statusCode_str[4] = "";
			sprintf_s(statusCode_str, sizeof(statusCode_str), "%d", statusCode);
			Map_AddOrUpdate(propMap, IotqiMsgProp_StatusCode, statusCode_str);
			Map_AddOrUpdate(propMap, IotqiMsgProp_CorrelationId, correlationId);
			Map_AddOrUpdate(propMap, IotqiMsgProp_CloudInstanceId, cloudInstanceId);
		}
		else
		{
			Map_AddOrUpdate(propMap, IotqiMsgProp_EventValue, eventValue);
		}

		if (networkTransport->SendEventAsync(iotHubClientHandle, msgHandle, sendConfirmationCallback, (void*)(uintptr_t)msgTrackingId) != iotqiOk)
		{
			Debug::WriteLine("***Failed to hand over the message to Client");
		}
		IoTHubMessage_Destroy(msgHandle);
		Debug::Write("\n");
	}
	msgTrackingId++;
}

void iotQiClient::respondToCommand(const char* cmdClass, const char* cmdName, const char* correlationId, const char* cloudInstanceId)
{
	char* prop = nullptr;
	jsonMakePropertyObject(CmdResponse.buffer, &CmdResponse.buffer_size, &prop, cmdName);
	sendMessage((unsigned char*)prop, CmdResponse.buffer_size, IotqiMsgType_CommandResponse, cmdClass, cmdName, "", CmdResponse.success_code, correlationId, cloudInstanceId);
	if (prop) free(prop);
	delete[] CmdResponse.buffer; CmdResponse.buffer_size = CmdResponse.success_code = 0;
	CmdResponse.buffer = NULL;
}

void iotQiClient::sendTelemetryInternal(const char* buffer, size_t size, const char* telemetryClass, const char* eventName, const char* eventValue)
{
	Debug::Write("> Telemetry: ");
	if (buffer != nullptr)
	{
		Debug::WriteLine((char*) buffer, size);
		char* prop = nullptr;
		jsonMakePropertyObject(buffer, &size, &prop, "telemetry");
		sendMessage((unsigned char*)prop, size, IotqiMsgType_Telemetry, telemetryClass, eventName, eventValue);
		if (prop) free(prop);
		return;
	}
	else
	{
		Debug::WriteLine("*** Invalid Telemetry ***");
		return;
	}
}

void iotQiClient::sendAlertInternal(const char* buffer, size_t size, const char* alertClass, const char* alertName, const char* alertSubject)
{
	Debug::Write("\n! Alert: ");
	if (buffer != nullptr)
	{
		Debug::WriteLine((char*)buffer, size);
		char* prop = nullptr;
		jsonMakePropertyObject(buffer, &size, &prop, alertName);
		sendMessage((unsigned char*)prop, size, IotqiMsgType_Alert, alertClass, alertName, alertSubject);
		if (prop) free(prop);
		return;
	}
	else
	{
		Debug::WriteLine("*** Invalid Alert ***");
		return;
	}
}

void iotQiClient::sendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
	unsigned int messageTrackingId = (unsigned int)(uintptr_t)userContextCallback;
#ifdef IOTQI_DEBUG
	(void)printf("iotQiSend:  Result for messageId=%u: status=%s \r\n", messageTrackingId, ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
#endif
}

IOTHUBMESSAGE_DISPOSITION_RESULT iotQiClient::commandMessageCallback(IOTHUB_MESSAGE_HANDLE msgHandle, void* userContextCallback)
{
	Debug::Write("> Command Message Callback: ");
	IOTHUBMESSAGE_DISPOSITION_RESULT result;
	/// {} Buffer to hold message body
	size_t bufSize = NULL;
	const char* cmdBuffer = NULL;

	/// ? Determine the way the body was stored in the IoTHubMessage
	switch (IoTHubMessage_GetContentType(msgHandle))
	{
	case IOTHUBMESSAGE_STRING:
		cmdBuffer = IoTHubMessage_GetString(msgHandle);
		break;
	case IOTHUBMESSAGE_BYTEARRAY:
		IoTHubMessage_GetByteArray(msgHandle, (const unsigned char**)&cmdBuffer, &bufSize);
		/// * Copy to new buffer and Null terminate it
		if (cmdBuffer != NULL)
		{
			char* newBuffer = new char[bufSize + 1];
			strncpy(newBuffer, cmdBuffer, bufSize);
			newBuffer[bufSize] = '\0';
			cmdBuffer = newBuffer;
		}
		break;
	default:
		break;
	}
	if (cmdBuffer == NULL)
	{
		/// ! If the command could not be fetched, abandon the message
		Debug::WriteLine("*** Unable to get IoTHubMessage body ***\n");	
		return IOTHUBMESSAGE_ABANDONED;
	}
	else
	{
		Debug::WriteLine(cmdBuffer);
		Debug::Write("-\tMessage Size = "); Debug::Print(strlen(cmdBuffer)); Debug::Write("\n");
		Debug::WriteLine("-\t> Get Properties from Map:");

		/// -> Get the properties from the map
		MAP_HANDLE mapProperties = IoTHubMessage_Properties(msgHandle);
		const char* correlationId = Map_GetValueFromKey(mapProperties, IotqiMsgProp_MessageId);
		const char* cloudInstanceId = Map_GetValueFromKey(mapProperties, IotqiMsgProp_CloudInstanceId);
		const char* cmdClass = Map_GetValueFromKey(mapProperties, IotqiMsgProp_EventClass);
		const char* cmdName = Map_GetValueFromKey(mapProperties, IotqiMsgProp_EventName);

		Debug::Write("-\t@\tCommand MsgId: "); Debug::WriteLine(correlationId);
		Debug::Write("-\t@\tCommand CloudId: "); Debug::WriteLine(cloudInstanceId);
		Debug::Write("-\t@\tCommand CmdClass: "); Debug::WriteLine(cmdClass);
		Debug::Write("-\t@\tCommand CmdName: "); Debug::WriteLine(cmdName);

		/// ? Determine the class of the recieved message
		EXECUTE_COMMAND_RESULT executeCommandResult;
		if ( strcmp(cmdClass, IotqiEventClass_Iotqi ) == 0)
		{
			/// >_ Execute iotQi Command
			Debug::Write("\n> Processing iotQi Command: "); Debug::Write(cmdName); Debug::WriteLine(cmdBuffer);
			executeCommandResult = IotqiModel_InvokeCommand(cmdBuffer);
			Debug::Write("\n");
		}
		else if ( strcmp(cmdClass, IotqiEventClass_User ) == 0)
		{
			/// >_ Execute user Command
			Debug::Write("\n> Processing User Command: "); Debug::Write(cmdName); Debug::WriteLine(cmdBuffer);
			executeCommandResult = fptrUserCmdMsgCallback(cmdBuffer);
			Debug::Write("\n");
		}
		else
		{
			/// ? Unable to determine the Command
			Debug::WriteLine("!\t *** Could not determine event class ***");
			return IOTHUBMESSAGE_ABANDONED;
		}
		result = (executeCommandResult == EXECUTE_COMMAND_ERROR) ? IOTHUBMESSAGE_ABANDONED : (executeCommandResult == EXECUTE_COMMAND_SUCCESS) ? IOTHUBMESSAGE_ACCEPTED : IOTHUBMESSAGE_REJECTED;
		if (bufSize) delete[] cmdBuffer;

		/// if user code has not created a result object, send back a default response object
		CmdResponse.success_code = executeCommandResult;

		/// & iotQi expects a response for all commands, send results as string statusCode (http status values)
		iotqiClient.respondToCommand(cmdClass, cmdName, correlationId, cloudInstanceId);
	}
	return result;
}

void iotQiClient::deinit()
{
	switch (iotqiInitState)
	{
	case MessageCallbackInit:
		//..
	case IotHubClientHandleInit:
		IoTHubClient_LL_Destroy(iotHubClientHandle);
	case SerializerInit:
		serializer_deinit();
	case PlatformInit:
		platform_deinit();
	default:
		iotqiInitState = Uninitialized;
		break;
	}
}