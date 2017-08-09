// Copyright (c) 2016 LooUQ Incorporated. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Globals.h

#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <arduino.h>
#include <stdint.h>
#include <iotQiTypes.h>
#include <iotQiDiagnostics.h>

/// # IotQi Constants 
#define DEVICEID_SIZE 37
#define DEVICECONNECTIONSTRING_SIZE 160
#define IOTQI_SCHEMA_VERSION	"v-1.0"
#define IOTQI_CODE_VERSION "iotQi MQTT v1.0"
#define NETWORKINFO_SIZE 128
#define NETWORKTYPE_SIZE 16
#define EVENTNAME_SIZE 80
#define EVENTVALUE_SIZE 80

// Status Code (use http conventions)
#define HTTP_OK 200
#define HTTP_ERROR 500
#define HTTP_NOTFOUND 404

/// & IotQi information
extern char deviceId[];
extern char deviceConnectionString[];
extern const char* versionInfo;
//extern char* cmdRspBuffer;
//extern int cmdRspBufferSize;
//extern int cmdRspStatusCode;


/// @ Network properties
extern char networkType[];
extern char networkString[];

/// ^ Delegates
extern USERMODEL_GETCOMMANDS fptrUserGetCommands;
extern USERMODEL_COMMANDMSGCALLBACK fptrUserCmdMsgCallback;

#endif  /*_GLOBALS_H */
