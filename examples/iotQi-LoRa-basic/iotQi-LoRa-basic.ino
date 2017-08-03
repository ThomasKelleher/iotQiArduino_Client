// Copyright (c) LooUQ Incorporated. All rights reserved.
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Use Arduino IDE 1.6.8 or later.

// library includes
#include <iotQi.h>
#include <iotQiLoRaNetwork.h>
#include <RH_RF95.h>
// end: library includes


// iotQi project (ino) includes
#include "settings.h"
#include "userModel.h"
// end: project includes

iotQiLoRaNetwork iotqiNetwork;
RH_RF95 radioDriver(SLAVE_SELECT_PIN, INTERRUPT_PIN);

/* setup() ------------------------------------------------------------------------------------------------------------ */


void setup() {
	// Setup the serial communication
	delay( 5000 );
	Serial.begin( 115200 );
	pinMode(13, HIGH);

	// Network Connection Requirements
	iotqiNetwork.SetDriver( radioDriver );
	radioDriver.init();
	radioDriver.setFrequency( FREQUENCY );
	radioDriver.setTxPower( MAX_SIGNAL_STRENGTH );

	// Initialize iotQi client and user model
	iotqiClient.Init( IOTQI_CONNECTION, iotqiNetwork );
	iotqiClient.Begin();
	iotqiClient.SetCallbacks( UserModel_GetCommands, UserModel_CommandMsgCallback );
	initUserModel();
}


/* loop() ------------------------------------------------------------------------------------------------------------- */


void loop()
{
	/* send pending messages & check for commands */
	iotqiClient.DoWork();

	// if (currMillis - prevUserDoWork >= (TELEMETRY_INTERVAL * 1000))        // TELEMETRY_INTERVAL: interval at which to send telemetry sample to iotQi cloud (defined in settings.h)
	// {
	// 	prevUserDoWork = currMillis;
	// 	doUserWork = true;
	// }
	// if (doUserWork)
	// {
	// 	UserModel_DoWork();
	// }
	iotqiClient.SendTelemetry( "wind-telemetry", WindSpeed, TELEMETRY_INTERVAL );
}