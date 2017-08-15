// Copyright (c) Microsoft. All rights reserved.
// Copyright (c) LooUQ Incorporated. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#ifndef _IOTQI_H
#define _IOTQI_H

#include <iotQiNetwork.h>

// Message Properties
#define IotqiMsgProp_iotQiVersion "iotqi-version"
#define IotqiMsgProp_MessageId "message-id"
#define IotqiMsgProp_MessageType "message-type"
#define IotqiMsgProp_EventClass "event-class"                       // command class, alert class, or telemetry series metric class
#define IotqiMsgProp_EventName "event-name"                         // user-defined name or title for the series
#define IotqiMsgProp_EventValue "event-value"
#define IotqiMsgProp_CorrelationId "correlation-id"
#define IotqiMsgProp_CloudInstanceId "cloud-instance"
#define IotqiMsgProp_StatusCode "status-code"

// iotQi Message Types
#define IotqiMsgType_Alert "alert"
#define IotqiMsgType_Command "command"
#define IotqiMsgType_CommandResponse "command-response"
#define IotqiMsgType_Telemetry "telemetry"

// iotQi Message Event Classes
#define IotqiEventClass_Iotqi "iotqi"
#define IotqiEventClass_User "user"

// iotQi Event Values
#define IotqiDeviceStart_AlertName "device-start"

// ---------------------------------------------------------------------------------------------------------------------------------------
typedef STRING_HANDLE TelemetryTemplate;
typedef void AlertTemplate;
typedef TelemetryTemplate (*TelemetryDelegate)(STRING_HANDLE*);
typedef AlertTemplate(*AlertDelegate)(STRING_HANDLE*);
// ---------------------------------------------------------------------------------------------------------------------------------------

#ifdef __cplusplus

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
	//void SetResponseBody(const unsigned char* buffer, size_t size);

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

