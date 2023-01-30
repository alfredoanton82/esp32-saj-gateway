// =================================================================================================
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
//               MIT license - see license.md for details
// =================================================================================================
#pragma once
#include <Arduino.h>
#include <ModbusMessage.h>
#include <ArduinoJson.h>

#include <iostream>
#include <string>
#include <array>

using namespace std;

class SAJInverterData {

	protected:
	 
	  int   n;       // Number of samples

		int   mode;    // Working
		float dcV[3];  // DC Voltage per string [V1, V2, V3]
		float dcI[3];  // DC Current per string [I1, I2, V3]
		float dcP[3];  // DC Power per scring [P1, P2, P3]
		float acV[3];  // Voltage between phases and neutral [U_1, U_2, U_3]
		float acI[3];  // Current per phase [I_1, I_2, I_3]
		float acF[3];  // Frequency per phase [F_1, F_2, F_3]
		float acP[3];  // Active Power per phase [P_1, P_2, P_3]
		float acPf[3]; // Current per phase [Pf_1, Pf_2, Pf_3]
		float busV;    // Bus voltage
		float temp;    // Inverter temperature
		float P;  		 // Active power
		float Q;       // Readctive power
		float Pf;      // Power factor

		// Constants
		const float dcVLimits[2]  = {-0.01, 1000.0};
		const float dcILimits[2]  = {-0.01, 12.0};
		const float acVLimits[2]  = {210.0, 250.0};
		const float acILimits[2]  = {-0.01, 50.0};

		const float busVLimits[2] = {10.0, 1000.0};
		const float tempLimits[2] = {-20.0, 100.0};

		const float PLimits[2]  = {-0.01, 5000.0};
		const float QLimits[2]  = {-5000.0, 5000.0};
		const float PfLimits[2] = {-1.01, 1.01};

		const float fLimits[2]  = {45.0, 55.0}; // Nominal 50 Hz

		int   readIntU16(ModbusMessage msg, int position, int scale);
		float readFloatU16(ModbusMessage msg, int position, float scale);
		float readFloatS16(ModbusMessage msg, int position, float scale);
		float readFloatU32(ModbusMessage msg, int position, float scale);

	public:

    // Default Constructor
    SAJInverterData();

    // Message Constructor
    SAJInverterData(ModbusMessage msg);

    // Destructor
    ~SAJInverterData();

    // Operators
    SAJInverterData &operator=(const SAJInverterData &m);
    SAJInverterData &operator+(const SAJInverterData &m);

    // Copy constructor
    SAJInverterData(const SAJInverterData &m);

		// Check integrity
		bool isValid();

		// Json
		String json();

		// Getters
		int getN();
		int getMode();
		array<float,3> getdcV();
		array<float,3> getdcI();
		array<float,3> getdcP();
		array<float,3> getacV();
		array<float,3> getacI();
		array<float,3> getacF();
		array<float,3> getacP();
		array<float,3> getacPf();
		float getBusV();
		float getTemp();
		float getP();
		float getQ();
		float getPf();

};
