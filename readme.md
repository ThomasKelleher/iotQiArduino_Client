# LooUQ iotQi Library for Arduino

The LooUQ iotQi library allows a device to connect to the LooUQ cloud. Supported Arduino devices can communicate with the cloud or other iotQi supported devices (like Win IoT devices).

## Prerequisities
Download required software:
*	[Arduino IDE 1.8.1](https://www.arduino.cc/en/Main/Software)
OR
*	[Visual Studio 2017](https://www.visualstudio.com/downloads/) with [Visual Micro for VS](https://marketplace.visualstudio.com/items?itemName=VisualMicro.ArduinoIDEforVisualStudio)

Install the following libraries via the Arduino IDE Library Manager for full functionality in the example
	{
		'AzureIoTHub',
		'AzureIoTProtocol_HTTP',
		'AzureIoTProtocol_MQTT',
		'AzureIoTUtility',
		'RTCZero',
		'WiFi101'
	}

<a name="platformdevices"></a>
## Platforms and Devices
This iotQi library runs specifically on Arduino devices, although there are other libraries/dlls/ect that allow other devices to connect to iotQi and its cloud services.
* The Arduino Platform supports the following devices:
	* Adafruit Feather M0
	* Arduino/Genuino MKR1000
	* Arduino Zero

<a name="setup"></a>
## Setup
First, you will need to [Setup your IoT device with iotQi Setup][iotQi-setup], which will provide you with a connection string that you'll need later to connect to the cloud.
Once the required libraries have been downloaded, download [iotQi for Arduino Devices](./). Check to see if your device requires any patches before proceeding.

## Device Managment
The device you create in the iotQi Setup, will be given a connection string and an alias. You can view and manage your device(s) via the [iotQi Console][iotQi-console].

<a name="examples"></a>
## Examples
Along with the iotQi library, there are a few [examples](./examples/) included to get you started:
* [WiFi-basic](./examples/iotQi-WiFi-basic/)
* [LoRa-basic](./examples/iotQi-LoRa-basic/)


__________________________________________
<a name="patching"></a>
## Patches
### Feather M0
A known issue in the Azure library must be patched prior to any compiling for the Feather M0. The Azure library utilizes the WINC1500 library by Adafruit. Adafruit however has deemed this library deprecated and has switched to support WiFi101 by Arduino.
* Winc1500 patch:
	* Open your Arduino directory ( usually "Documents/Arduino/" ) and then open the "libraries" folder. If the Azure library is installed, you should be able to navigate to "AzureIoTUtility/src/adapters/", then open sslClient_arduino.cpp. Under the "#ifdef ARDUINO_ARCH_ESP8266" block, there is a "#elif ARDUINO_SAMD_FEATHER_M0" block switch. Comment this block out (from the #elif to the #else, but not the #else itself). Save the *.cpp and close the file.

---
For more information about iotQi please visit:
[LooUQ Help Center: Devices](https://support.loouq.com/hc/en-us/categories/202102247-Working-with-iotQi-Devices)

[iotQi-setup]: https://setup.loouq.com/
[iotQi-console]: https://console.loouq.com/
[iotQi-site]: https://www.loouq.com/