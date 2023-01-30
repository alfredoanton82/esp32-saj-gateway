// =================================================================================================
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
//               MIT license - see license.md for details
// =================================================================================================
#include <SAJInverterData.h>

SAJInverterData::SAJInverterData() {
  n = 0;
}

// Copy constructor
SAJInverterData::SAJInverterData(const SAJInverterData &m) {
  *this = m;
}


SAJInverterData::SAJInverterData(ModbusMessage msg) {
  
  if (msg.data() != NULL) {

    n       = 1;

    mode    = readIntU16(msg, 3, 1);

    dcV[0]  = readFloatU16(msg, 17, 1.0e-1);
    dcI[0]  = readFloatU16(msg, 19, 1.0e-2);
    dcP[0]  = readFloatU16(msg, 21, 1.0e0);

    dcV[1]  = readFloatU16(msg, 23, 1.0e-1);
    dcI[1]  = readFloatU16(msg, 25, 1.0e-2);
    dcP[1]  = readFloatU16(msg, 27, 1.0e0);

    dcV[2]  = 0.0; //readFloatU16(msg, 29, 1.0e-1);
    dcI[2]  = 0.0; //readFloatU16(msg, 31, 1.0e-2);
    dcP[2]  = 0.0; //readFloatU16(msg, 33, 1.0e0);

    busV    = readFloatS16(msg, 35, 1.0e-1);
    temp    = readFloatS16(msg, 37, 1.0e-1);
    // Omitting 39 => GFCI?
    P       = readFloatU16(msg, 41, 1.0e0);
    Q       = readFloatS16(msg, 43, 1.0e0);
    Pf      = readFloatS16(msg, 45, 1.0e-3);

    acV[0]  = readFloatU16(msg, 47, 1.0e-1);
    acI[0]  = readFloatU16(msg, 49, 1.0e-2);
    acF[0]  = readFloatU16(msg, 51, 1.0e-2);
    // Omitting 50 => DC current
    acP[0]  = readFloatU16(msg, 55, 1.0e0);
    acPf[0] = readFloatS16(msg, 57, 1.0e-3);

    acV[1]  = readFloatU16(msg, 59, 1.0e-1);
    acI[1]  = readFloatU16(msg, 61, 1.0e-2);
    acF[1]  = readFloatU16(msg, 63, 1.0e-2);
    // Omitting 62 => DC current
    acP[1]  = readFloatU16(msg, 67, 1.0e0);
    acPf[1] = readFloatS16(msg, 69, 1.0e-3);

    acV[2]  = readFloatU16(msg, 71, 1.0e-1);
    acI[2]  = readFloatU16(msg, 73, 1.0e-2);
    acF[2]  = readFloatU16(msg, 75, 1.0e-2);
    // Omitting 74 => DC current
    acP[2]  = readFloatU16(msg, 79, 1.0e0);
    acPf[2] = readFloatS16(msg, 81, 1.0e-3);

  }

}

SAJInverterData::~SAJInverterData(){}

int SAJInverterData::readIntU16(ModbusMessage msg, int position, int scale)
{
    
  uint16_t raw;

  msg.get(position, raw);

  return (int) raw * scale;

}

float SAJInverterData::readFloatU16(ModbusMessage msg, int position, float scale)
{
    
  uint16_t raw;

  msg.get(position, raw);

  return (float) raw * scale;

}

float SAJInverterData::readFloatS16(ModbusMessage msg, int position, float scale)
{
    
  int16_t raw;

  msg.get(position, raw);

  return (float) raw * scale;;

}

float SAJInverterData::readFloatU32(ModbusMessage msg, int position, float scale)
{
    
  uint32_t raw;

  msg.get(position, raw);

  return (float) raw * scale;;

}

bool SAJInverterData::isValid(){

  bool out = true;

  for (int i = 0; i < 3; i++) {
    out &=  ( dcVLimits[0] < dcV[i]/n  && dcV[i]/n  < dcVLimits[1] ) && 
            ( dcILimits[0] < dcI[i]/n  && dcI[i]/n  < dcILimits[1] ) &&
            ( PLimits[0]   < dcP[i]/n  && dcP[i]/n  < PLimits[1]   ) &&
            ( acVLimits[0] < acV[i]/n  && acV[i]/n  < acVLimits[1] ) &&
            ( acILimits[0] < acI[i]/n  && acI[i]/n  < acILimits[1] ) &&
            ( PLimits[0]   < acP[i]/n  && acP[i]/n  < PLimits[1]   ) &&
            ( PfLimits[0]  < acPf[i]/n && acPf[i]/n < PfLimits[1]  ) &&
            ( fLimits[0]   < acF[i]/n  && acF[i]/n  < fLimits[1]   );
  }

  out &=  ( PLimits[0]  < P/n  && P/n  < PLimits[1]  ) &&
          ( QLimits[0]  < Q/n  && Q/n  < QLimits[1]  ) &&
          ( PfLimits[0] < Pf/n && Pf/n < PfLimits[1] );

  out &= ( busVLimits[0] < busV/n && busV/n < busVLimits[1] );

  out &= ( tempLimits[0] < temp/n && temp/n < tempLimits[1] );

  return out;
  
}

// Assignment operator
SAJInverterData& SAJInverterData::operator=(const SAJInverterData& m) {

  // Do anything only if not self-assigning
  if (this != &m) {
    n    = m.n;
    mode = m.mode;
    busV = m.busV;
    temp = m.temp;
    P    = m.P;
    Q    = m.Q;
    Pf   = m.Pf;
    for (int i = 0; i < 3; i++) {
      dcV[i]  = m.dcV[i];
      dcI[i]  = m.dcI[i];
      dcP[i]  = m.dcP[i];
      acV[i]  = m.acV[i];
      acI[i]  = m.acI[i];
      acF[i]  = m.acF[i]; 
      acP[i]  = m.acP[i];
      acPf[i] = m.acPf[i];
    }
  }

  return *this;
}

// Adition operator
SAJInverterData& SAJInverterData::operator+(const SAJInverterData& m) {

    mode = m.mode; // Assign last value

    n    += m.n;
    busV += m.busV;
    temp += m.temp;
    P    += m.P;
    Q    += m.Q;
    Pf   += m.Pf;
    for (int i = 0; i < 3; i++) {
      dcV[i]  += m.dcV[i];
      dcI[i]  += m.dcI[i];
      dcP[i]  += m.dcP[i];
      acV[i]  += m.acV[i];
      acI[i]  += m.acI[i];
      acF[i]  += m.acF[i];
      acP[i]  += m.acP[i];
      acPf[i] += m.acPf[i];
    }

  return *this;
}

String SAJInverterData::json() {

  String outJsonMsg;

  StaticJsonDocument<1024> root;
  
  root["mode"]    = mode;
  root["samples"] = n;

  // Average values
  root["dcVpv1"]  = dcV[0]/n;
  root["dcVpv2"]  = dcV[1]/n;
  root["dcVpv3"]  = dcV[2]/n;
  root["dcIpv1"]  = dcI[0]/n;
  root["dcIpv2"]  = dcI[1]/n;
  root["dcIpv3"]  = dcI[2]/n;
  root["dcPpv1"]  = dcP[0]/n;
  root["dcPpv2"]  = dcP[1]/n;
  root["dcPpv3"]  = dcP[2]/n;
  root["acVl1"]   = acV[0]/n;
  root["acVl2"]   = acV[1]/n;
  root["acVl3"]   = acV[2]/n;
  root["acIl1"]   = acI[0]/n;
  root["acIl2"]   = acI[1]/n;
  root["acIl3"]   = acI[2]/n;
  root["acPl1"]   = acP[0]/n;
  root["acPl2"]   = acP[1]/n;
  root["acPl3"]   = acP[2]/n;
  root["acFl1"]   = acF[0]/n;
  root["acFl2"]   = acF[1]/n;
  root["acFl3"]   = acF[2]/n;
  root["acPfl1"]  = acPf[0]/n;
  root["acPfl2"]  = acPf[1]/n;
  root["acPfl3"]  = acPf[2]/n;
  root["busV"]    = busV/n;
  root["temp"]    = temp/n;
  root["pTotal"]  = P/n;
  root["qTotal"]  = Q/n;
  root["pfTotal"] = Pf/n;

  // Convert to JSON
  serializeJson(root, outJsonMsg);

  return outJsonMsg;

}

// Getters
int SAJInverterData::getN(){
  return n;
}

int SAJInverterData::getMode(){
  return mode;
}

array<float,3> SAJInverterData::getdcV() {
  array<float,3> out;
  for (int i = 0; i < 3; i++) {
    out[i] = dcV[i]/n;
  }
  return out;
}

array<float,3> SAJInverterData::getdcI() {
  array<float,3> out;
  for (int i = 0; i < 3; i++) {
    out[i] = dcI[i]/n;
  }
  return out;
}

array<float,3> SAJInverterData::getdcP() {
  array<float,3> out;
  for (int i = 0; i < 3; i++) {
    out[i] = dcP[i]/n;
  }
  return out;
}

array<float,3> SAJInverterData::getacV() {
  array<float,3> out;
  for (int i = 0; i < 3; i++) {
    out[i] = acV[i]/n;
  }
  return out;
}

array<float,3> SAJInverterData::getacI() {
  array<float,3> out;
  for (int i = 0; i < 3; i++) {
    out[i] = acI[i]/n;
  }
  return out;
}

array<float,3> SAJInverterData::getacF() {
  array<float,3> out;
  for (int i = 0; i < 3; i++) {
    out[i] = acF[i]/n;
  }
  return out;
}

array<float,3> SAJInverterData::getacP() {
  array<float,3> out;
  for (int i = 0; i < 3; i++) {
    out[i] = acP[i]/n;
  }
  return out;
}

array<float,3> SAJInverterData::getacPf() {
  array<float,3> out;
  for (int i = 0; i < 3; i++) {
    out[i] = acPf[i]/n;
  }
  return out;
}

float SAJInverterData::getBusV() {
  return busV/n;
}

float SAJInverterData::getTemp() {
  return temp/n;
}

float SAJInverterData::getP() {
  return P/n;
}

float SAJInverterData::getQ() {
  return Q/n;
}

float SAJInverterData::getPf() {
  return Pf/n;
}
