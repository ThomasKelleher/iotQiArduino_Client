// iotQiNetwork.h

#include <globals.h>

#ifndef     _IOTQINETWORK_h
#define	_IOTQINETWORK_h

#pragma	region	Enumerate Network Types
//#define	WIFI					0
//#define	LORA				1
//#define	BLUETOOTH	2
//#define	LTE					3
#pragma endregion

#ifdef __cplusplus
class iotQiNetwork
{
	friend class iotQiClient;
public:
	virtual int32_t GetRSSI()=0;

private:
	virtual void updateNetworkInfo()=0;
	virtual void init() = 0;
	virtual IotQiResult begin() = 0;
	virtual void end() = 0;

	virtual IotQiResult SendEventAsync(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_MESSAGE_HANDLE msgHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK sendConfirmationCallback, void* msgTrackingId) = 0;
	virtual IotQiResult IotDoWork(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle) = 0;

};
#endif

#endif

