#ifndef _IOTQITYPES_h
#define _IOTQITYPES_h

#include <AzureIoTHub.h>
#include <AzureIoTUtility.h>
#include <AzureIoTProtocol_MQTT.h>

#define MODEL_RESULT_VALUES   \
    MODEL_OK,                 \
    MODEL_ERROR

/** @brief Enumeration of results representing
*  the Model function status
*/
DEFINE_ENUM(IOTQIMODEL_RESULT, MODEL_RESULT_VALUES)

// typedef to define required userModel callback functions for command retrieval and command message received

enum IotQiResult
{
	iotqiOk,
	iotqiError,
	iotqiInvalidArg
};

typedef IOTQIMODEL_RESULT(*USERMODEL_GETCOMMANDS)(STRING_HANDLE commandsMeta);
typedef EXECUTE_COMMAND_RESULT(*USERMODEL_COMMANDMSGCALLBACK)(const char* cmdBuffer);

#endif // !_IOTQITYPES_h
