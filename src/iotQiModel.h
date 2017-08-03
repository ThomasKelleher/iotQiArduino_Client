// Copyright (c) LooUQ Incorporated. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include <globals.h>

#ifndef _IOTQIMODEL_H
#define _IOTQIMODEL_H

//#define _DEBUG_MODEL

#ifdef __cplusplus
extern "C" {
#endif
	IOTQIMODEL_RESULT initIotqiModel();
	void deinitIotqiModel();

	IOTQIMODEL_RESULT IotqiModel_GetCommands(STRING_HANDLE commandsMeta);
	EXECUTE_COMMAND_RESULT IotqiModel_InvokeCommand(const char* cmdBuffer);

	IOTQIMODEL_RESULT TEMPLATE_deviceStarted( char** sendBuffer, size_t* size);
#ifdef __cplusplus
}
#endif

static void MODEL_setResult(int status, unsigned char* buffer, size_t size);
#endif    // #define _IOTQIMODEL_H

//sendAlert(sendBuffer, size, "iotqi", "device-start", deviceId);