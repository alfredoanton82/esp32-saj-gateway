// =================================================================================================
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
//               MIT license - see license.md for details
// =================================================================================================
#include <DTSU666Data.h>

DTSU666Data::DTSU666Data() {
  n = 0;
}

// Copy constructor
DTSU666Data::DTSU666Data(const DTSU666Data &m) {
  *this = m;
}

DTSU666Data::DTSU666Data(ModbusMessage msg) {
  
  if (msg.data() != NULL) {

    n = 1;

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
    out &= ( Ulimits[0]  < U[i]/n  && U[i]/n  < Ulimits[1] ) && 
           ( Vlimits[0]  < V[i]/n  && V[i]/n  < Vlimits[1] ) &&
           ( Ilimits[0]  < I[i]/n  && I[i]/n  < Ilimits[1] );
  }
  for (int i = 0; i < 4; i++) {
    out &= ( Plimits[0]  < P[i]/n  && P[i]/n  < Plimits[1] ) && 
           ( Qlimits[0]  < Q[i]/n  && Q[i]/n  < Qlimits[1] ) &&
           ( Slimits[0]  < S[i]/n  && S[i]/n  < Slimits[1] ) &&
           ( PfLimits[0] < Pf[i]/n && Pf[i]/n < PfLimits[1] );
  }
  out &= ( fLimits[0] < f/n && f/n < fLimits[1] );

  return out;
  
}

// Assignment operator
DTSU666Data& DTSU666Data::operator=(const DTSU666Data& m) {
  // Do anything only if not self-assigning
  if (this != &m) {
    n = m.n;
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

  n += m.n;
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

String DTSU666Data::json() {

  String outJsonMsg;

  StaticJsonDocument<1024> root;

  root["samples"] = n;

  // Writing averages
  root["V_L12"] = U[0]/n;
  root["V_L23"] = U[1]/n;
  root["V_L31"] = U[2]/n;
  root["V_L1"]  = V[0]/n;
  root["V_L2"]  = V[1]/n;
  root["V_L3"]  = V[2]/n;
  root["I_L1"]  = I[0]/n;
  root["I_L2"]  = I[1]/n;
  root["I_L3"]  = I[2]/n;
  root["P_T"]   = P[0]/n;
  root["P_L1"]  = P[1]/n;
  root["P_L2"]  = P[2]/n;
  root["P_L3"]  = P[3]/n;
  root["Q_T"]   = Q[0]/n;
  root["Q_L1"]  = Q[1]/n;
  root["Q_L2"]  = Q[2]/n;
  root["Q_L3"]  = Q[3]/n;
  root["S_T"]   = S[0]/n;
  root["S_L1"]  = S[1]/n;
  root["S_L2"]  = S[2]/n;
  root["S_L3"]  = S[3]/n;
  root["Pf_T"]  = Pf[0]/n;
  root["Pf_L1"] = Pf[1]/n;
  root["Pf_L2"] = Pf[2]/n;
  root["Pf_L3"] = Pf[3]/n;
  root["Freq"]  = f/n;

  // Convert to JSON
  serializeJson(root, outJsonMsg);

  return outJsonMsg;

}

// Getters
int DTSU666Data::getN(){
  return n;
}

array<float,3> DTSU666Data::getU() {
  array<float,3> out;
  for (int i = 0; i < 3; i++) {
    out[i] = U[i]/n;
  }
  return out;
}

array<float,3> DTSU666Data::getV() {
  array<float,3> out;
  for (int i = 0; i < 3; i++) {
    out[i] = V[i]/n;
  }
  return out;
}

array<float,3> DTSU666Data::getI() {
  array<float,3> out;
  for (int i = 0; i < 3; i++) {
    out[i] = I[i]/n;
  }
  return out;
}

array<float,4> DTSU666Data::getP() {
  array<float,4> out;
  for (int i = 0; i < 4; i++) {
    out[i] = P[i]/n;
  }
  return out;
}

array<float,4> DTSU666Data::getQ() {
  array<float,4> out;
  for (int i = 0; i < 4; i++) {
    out[i] = Q[i]/n;
  }
  return out;
}

array<float,4> DTSU666Data::getS() {
  array<float,4> out;
  for (int i = 0; i < 4; i++) {
    out[i] = S[i]/n;
  }
  return out;
}

array<float,4> DTSU666Data::getPf() {
  array<float,4> out;
  for (int i = 0; i < 4; i++) {
    out[i] = Pf[i]/n;
  }
  return out;
}

float DTSU666Data::getF() {
  return f/n;
}

