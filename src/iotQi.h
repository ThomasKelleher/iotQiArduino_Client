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

#ifndef _IOTQI_H
#define _IOTQI_H

#include <iotQiNetwork.h>
#include <AzureIoTProtocol_MQTT.h>

/* -----------------------------------------------------------------------------------
*  @ iotQi Message Properties @
* --------------------------------------------------------------------------------- */
#define IotqiMsgProp_iotQiVersion "iotqi-version"
#define IotqiMsgProp_MessageId "message-id"
#define IotqiMsgProp_MessageType "message-type"
#define IotqiMsgProp_EventClass "event-class"                       // command class, alert class, or telemetry series metric class
#define IotqiMsgProp_EventName "event-name"                         // user-defined name or title for the series
#define IotqiMsgProp_EventValue "event-value"
#define IotqiMsgProp_CorrelationId "correlation-id"
#define IotqiMsgProp_CloudInstanceId "cloud-instance"
#define IotqiMsgProp_StatusCode "status-code"

/* -----------------------------------------------------------------------------------
* > iotQi Message Types <
* --------------------------------------------------------------------------------- */
#define IotqiMsgType_Alert "alert"
#define IotqiMsgType_Command "command"
#define IotqiMsgType_CommandResponse "command-response"
#define IotqiMsgType_Telemetry "telemetry"

/* -----------------------------------------------------------------------------------
* + iotQi Event Classes +
* --------------------------------------------------------------------------------- */
#define IotqiEventClass_Iotqi "iotqi"
#define IotqiEventClass_User "user"

/* -----------------------------------------------------------------------------------
* [] iotQi Event Values []
* --------------------------------------------------------------------------------- */
#define IotqiDeviceStart_AlertName "device-start"

// ---------------------------------------------------------------------------------------------------------------------------------------
// Type Definitions of Templates to be used by the user model
	typedef STRING_HANDLE TelemetryTemplate;
	typedef void AlertTemplate;
	typedef TelemetryTemplate (*TelemetryDelegate)(STRING_HANDLE*);
	typedef AlertTemplate(*AlertDelegate)(STRING_HANDLE*);
// ---------------------------------------------------------------------------------------------------------------------------------------

#ifdef __cplusplus

/// <summary>
/// Enumeration of states the iotQi service can be in.
/// <list type="bullet">
/// <item>Uninitialized
/// <description>iotQi is not initialized.</description></item>
/// <item>PlatformInit
/// <description>The platform is initialized.</description></item>
/// <item>SerializerInit
/// <description>The serializer is initialized.</description></item>
/// <item>IotHubClientHandleInit
/// <description>The IotHubClientHandle is initialized.</description></item>
/// <item>MessageCallbackInit
/// <description>The MessageCallback is initialized.</description></item>
/// </list>
///	</summary>
enum iotQiState
{
	Uninitialized,
	PlatformInit,
	SerializerInit,
	IotHubClientHandleInit,
	MessageCallbackInit
};

class iotQiClient
{
public:

	iotQiClient();
	~iotQiClient();

	static IOTHUBMESSAGE_DISPOSITION_RESULT commandMessageCallback(IOTHUB_MESSAGE_HANDLE msgHandle, void* userContextCallback);

	bool Init(const char* iotqiHubConnection, iotQiNetwork &networkClient);
	bool Begin();

	void SetCallbacks(USERMODEL_GETCOMMANDS getUserCommands, USERMODEL_COMMANDMSGCALLBACK userCommandMsgCallback);

	void SendTelemetry( char* eventName, TelemetryDelegate t_temp, unsigned long &prevTelemetry, int interval=NULL );
	void SendAlert(const char* alertName, const char* alertSubject);
	void SendAlert(const char* alertName, const char* alertSubject, AlertDelegate a_temp);
	

	void DoWork();

private:
	static iotQiNetwork* networkTransport;
	iotQiState iotqiInitState;
	unsigned long lastTelemetry;
	static unsigned int msgTrackingId;
	static IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;

	void ALERT_sendDeviceStart();

	void sendMessage(
		const unsigned char* buffer=NULL, size_t size=NULL, const char* msgType=nullptr, const char* eventClass=nullptr, const char* eventName=nullptr, const char* eventValue=nullptr, int statusCode=NULL, const char* correlationId=nullptr, const char* cloudInstanceId=nullptr);
	void respondToCommand(
		const char* cmdClass=nullptr, const char* cmdName=nullptr, const char* correlationId=nullptr, const char* cloudInstanceId=nullptr);
	void sendTelemetryInternal(
		const char* buffer=NULL, size_t size=NULL, const char* telemetryClass=nullptr, const char* eventName=nullptr, const char* eventValue=nullptr);
	void sendAlertInternal(
		const char* buffer=NULL, size_t size=NULL, const char* alertClass=nullptr, const char* alertName=nullptr, const char* alertSubject=nullptr);
	static void sendConfirmationCallback(
		IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback=nullptr);
	void deinit();
};

extern iotQiClient iotqiClient;
#endif

#endif _IOTQI_H

