// 
// 
// 

#include <globals.h>

// network globals
char networkType[NETWORKTYPE_SIZE] = "<network type>\0";
char networkString[NETWORKINFO_SIZE] = "<network info>\0";

// =======================================================================================================================================

/* allocated and initialize iotQi globals */
char deviceId[DEVICEID_SIZE] = "\0";
char deviceConnectionString[DEVICECONNECTIONSTRING_SIZE] = "\0";
const char* iotqiCodeVersion = IOTQI_CODE_VERSION;
const char* iotqiSchemaVersion = IOTQI_SCHEMA_VERSION;
//char* cmdRspBuffer = NULL;
//int cmdRspBufferSize = 0;
//int cmdRspStatusCode = 200;