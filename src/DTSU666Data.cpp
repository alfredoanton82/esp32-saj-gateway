// =================================================================================================
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
//               MIT license - see license.md for details
// =================================================================================================
#include <DTSU666Data.h>

DTSU666Data::DTSU666Data() {
}

// Copy constructor
DTSU666Data::DTSU666Data(const DTSU666Data &m) {
  *this = m;
}

DTSU666Data::DTSU666Data(ModbusMessage msg) {
  
  if (msg.data() != NULL) {

    U[0]  = readFloat(msg,  0, 3);
    U[1]  = readFloat(msg,  1, 3);
    U[2]  = readFloat(msg,  2, 3);

    V[0]  = readFloat(msg,  3, 3);
    V[1]  = readFloat(msg,  4, 3);
    V[2]  = readFloat(msg,  5, 3);

    I[0]  = readFloat(msg,  6, 3);
    I[1]  = readFloat(msg,  7, 3);
    I[2]  = readFloat(msg,  8, 3);

    P[0]  = readFloat(msg,  9, 3) * 1.0e3;
    P[1]  = readFloat(msg, 10, 3) * 1.0e3;
    P[2]  = readFloat(msg, 11, 3) * 1.0e3;
    P[3]  = readFloat(msg, 12, 3) * 1.0e3;

    Q[0]  = readFloat(msg, 13, 3) * 1.0e3;
    Q[1]  = readFloat(msg, 14, 3) * 1.0e3;
    Q[2]  = readFloat(msg, 15, 3) * 1.0e3;
    Q[4]  = readFloat(msg, 16, 3) * 1.0e3;

    S[0]  = readFloat(msg, 17, 3) * 1.0e3;
    S[1]  = readFloat(msg, 18, 3) * 1.0e3;
    S[2]  = readFloat(msg, 19, 3) * 1.0e3;
    S[3]  = readFloat(msg, 20, 3) * 1.0e3;

    Pf[0] = readFloat(msg, 21, 3); 
    Pf[1] = readFloat(msg, 22, 3);
    Pf[2] = readFloat(msg, 23, 3);
    Pf[3] = readFloat(msg, 24, 3);

    f  = readFloat(msg,34,3);

  }

}

DTSU666Data::~DTSU666Data(){
}

float DTSU666Data::readFloat(ModbusMessage msg, int idx, int offset)
{
    
  float f;

  msg.get(FLOAT_SIZE*idx+offset, f);
  
  return f;

}

bool DTSU666Data::isValid(){

  bool out = true;
  for (int i = 0; i < 3; i++) {
    out &= ( Ulimits[0] < U[i] && U[i] < Ulimits[1] ) && 
           ( Vlimits[0] < V[i] && V[i] < Vlimits[1] ) &&
           ( Ilimits[0] < I[i] && I[i] < Ilimits[1] );
  }
  for (int i = 0; i < 4; i++) {
    out &= ( Plimits[0] < P[i]  && P[i]  < Plimits[1] ) && 
           ( Qlimits[0] < Q[i]  && Q[i]  < Qlimits[1] ) &&
           ( Slimits[0] < S[i]  && S[i]  < Slimits[1] ) &&
           ( PfLimits[0] < Pf[i] && Pf[i] < PfLimits[1] );
  }
  out &= ( fLimits[0] < f && f < fLimits[1] );

  return out;
  
}

// Assignment operator
DTSU666Data& DTSU666Data::operator=(const DTSU666Data& m) {
  // Do anything only if not self-assigning
  if (this != &m) {
    for (int i = 0; i < 3; i++) {
      U[i] = m.U[i];
      V[i] = m.V[i];
      I[i] = m.I[i];
    }
    for (int i = 0; i < 4; i++) {
      P[i]  = m.P[i];
      Q[i]  = m.Q[i];
      S[i]  = m.S[i];
      Pf[i] = m.Pf[i];
    }
    f = m.f;
  }

  return *this;
}

// Adition operator
DTSU666Data& DTSU666Data::operator+(const DTSU666Data& m) {

  for (int i = 0; i < 3; i++) {
    U[i] += m.U[i];
    V[i] += m.V[i];
    I[i] += m.I[i];
  }
  for (int i = 0; i < 4; i++) {
    P[i]  += m.P[i];
    Q[i]  += m.Q[i];
    S[i]  += m.S[i];
    Pf[i] += m.Pf[i];
  }
  f += m.f;

  return *this;
}

DTSU666Data DTSU666Data::scale(float scale) {

  // Scale each value
  for (int i = 0; i < 3; i++) {
    U[i] *= scale;
    V[i] *= scale;
    I[i] *= scale;
  }
  for (int i = 0; i < 4; i++) {
    P[i] *= scale;
    Q[i] *= scale;
    S[i] *= scale;
    Pf[i] *= scale;
  }
  
  f *= scale;

  return *this;  
}

String DTSU666Data::json() {

  String outJsonMsg;

  StaticJsonDocument<1024> root;
  root["V_L12"] = U[0];
  root["V_L23"] = U[1];
  root["V_L31"] = U[2];
  root["V_L1"]  = V[0];
  root["V_L2"]  = V[1];
  root["V_L3"]  = V[2];
  root["I_L1"]  = I[0];
  root["I_L2"]  = I[1];
  root["I_L3"]  = I[2];
  root["P_T"]   = P[0];
  root["P_L1"]  = P[1];
  root["P_L2"]  = P[2];
  root["P_L3"]  = P[3];
  root["Q_T"]   = Q[0];
  root["Q_L1"]  = Q[1];
  root["Q_L2"]  = Q[2];
  root["Q_L3"]  = Q[3];
  root["S_T"]   = S[0];
  root["S_L1"]  = S[1];
  root["S_L2"]  = S[2];
  root["S_L3"]  = S[3];
  root["Pf_T"]  = Pf[0];
  root["Pf_L1"] = Pf[1];
  root["Pf_L2"] = Pf[2];
  root["Pf_L3"] = Pf[3];
  root["Freq"]  = f;

  // Convert to JSON
  serializeJson(root, outJsonMsg);

  return outJsonMsg;

}

// Getters
array<float,3> DTSU666Data::getU() {
  array<float,3> out;
  for (int i = 0; i < 3; i++) {
    out[i] = U[i];
  }
  return out;
}

array<float,3> DTSU666Data::getV() {
  array<float,3> out;
  for (int i = 0; i < 3; i++) {
    out[i] = V[i];
  }
  return out;
}

array<float,3> DTSU666Data::getI() {
  array<float,3> out;
  for (int i = 0; i < 3; i++) {
    out[i] = I[i];
  }
  return out;
}

array<float,4> DTSU666Data::getP() {
  array<float,4> out;
  for (int i = 0; i < 4; i++) {
    out[i] = P[i];
  }
  return out;
}

array<float,4> DTSU666Data::getQ() {
  array<float,4> out;
  for (int i = 0; i < 4; i++) {
    out[i] = Q[i];
  }
  return out;
}

array<float,4> DTSU666Data::getS() {
  array<float,4> out;
  for (int i = 0; i < 4; i++) {
    out[i] = S[i];
  }
  return out;
}

array<float,4> DTSU666Data::getPf() {
  array<float,4> out;
  for (int i = 0; i < 4; i++) {
    out[i] = Pf[i];
  }
  return out;
}

float DTSU666Data::getf() {
  return f;
}

