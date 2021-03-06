// Copyright (c) LooUQ Incorporated. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// in Seconds
#define	TELEMETRY_INTERVAL	(10)	// @ Telemetry interval

/* =================================================================================================================================
 *  Setup your LoRa and device connection string here
================================================================================================================================= */

#define SIGNAL_POWER	20 // @ User Defined radio signal strength
 
/* Below is a typical connection string to the iotQi device hub,  you get this information from iotQi Setup on the device pages (the example below is not functional)
 *
 * Device IDs are a GUID made up of hexidecimal digits arranged in 5 tokens
 * The SharedAccessKey is generated by iotQi and will be a random string containing alpha-numeric characters a some special characters, usually this ends in an = character
 *
 * Example connection... yours will look like this
#define IOTQI_CONNECTION "HostName=iothub-prod-loouq.azure-devices.net;DeviceId=ba93e7bb-b2f3-42fa-a272-1c94e2389c25;SharedAccessKey=wnQr8WVBC8R43weh3Nf4sZ8cHJfvrYRfvZW+sioioZc="
*/

#define IOTQI_CONNECTION "ConnectionString" // @ IoT Hub Connection String
/* =================================================================================================================================
================================================================================================================================= */



