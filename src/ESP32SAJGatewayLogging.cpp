
// =================================================================================================
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
//               MIT license - see license.md for details
// =================================================================================================
#include<ESP32SAJGatewayLogging.h>

void loggingSetup() {

  Serial.begin(115200);

  delay(1000);
  
  Serial.println("Connected serial U0...");

}
