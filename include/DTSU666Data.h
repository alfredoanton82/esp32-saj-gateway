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

class DTSU666Data {

	protected:
		float U[3];  // Voltage between phases [U_12, U_23, U_31] [V]
		float V[3];  // Voltage between phases and neutral [U_1, U_2, U_3] [V]
		float I[3];  // Current per phase [I_1, I_2, I_3] [A]
		float P[4];  // Active power [P_t, P_1, P_2, P_3] [W]
		float Q[4];  // Reactive power [Q_t, Q_1, Q_2, Q_3] [VAR]
		float S[4];  // Apparent power [S_t, S_1, S_2, S_3] [VA]
		float Pf[4]; // Power factor [Pf_t, Pf_1, Pf_2, Pf_3]
		float f;     // Frequency 

		// Constants
		const float Ulimits[2]  = {380.0, 420.0}; // Nominal 400 V
		const float Vlimits[2]  = {210.0, 250.0}; // Nominal 230 V
		const float Ilimits[2]  = {-50.0, 50.0}; 
		const float Plimits[2]  = {-20000.0, 20000.0};
		const float Qlimits[2]  = {-20000.0, 20000.0};
		const float Slimits[2]  = {-20000.0, 20000.0};
		const float PfLimits[2] = {-1.01, 1.01};
		const float fLimits[2]  = {45.0, 55.0}; // Nominal 50 Hz

		// Float size
		const int FLOAT_SIZE = sizeof(f);

		// Read float
		float readFloat(ModbusMessage msg, int idx, int offset);

	public:

    // Default Constructor
    DTSU666Data();

    // Message Constructor
    DTSU666Data(ModbusMessage msg);

    // Destructor
    ~DTSU666Data();

    // Operators
    DTSU666Data &operator=(const DTSU666Data &m);
    DTSU666Data &operator+(const DTSU666Data &m);

    // Copy constructor
    DTSU666Data(const DTSU666Data &m);

		// Check integrity
		bool isValid();

		// Multiply by scalar
		DTSU666Data scale(float scale);

		// Json
		String json();

		// Getters
		array<float,3> getU();
		array<float,3> getV();
		array<float,3> getI();
		array<float,4> getP();
		array<float,4> getQ();
		array<float,4> getS();
		array<float,4> getPf();
		float  				 getf();

};
