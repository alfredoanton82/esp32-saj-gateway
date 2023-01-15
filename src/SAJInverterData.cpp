// =================================================================================================
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
//               MIT license - see license.md for details
// =================================================================================================
#include <SAJInverterData.h>

SAJInverterData::SAJInverterData() {
}

// Copy constructor
SAJInverterData::SAJInverterData(const SAJInverterData &m) {
  *this = m;
}

SAJInverterData::SAJInverterData(ModbusMessage msg) {
  
  if (msg.data() != NULL) {

    mode    = readIntU16(msg, 3, 0);

    dcV[0]  = readFloatU16(msg, 14, 1.0e-1);
    dcI[0]  = readFloatU16(msg, 16, 1.0e-2);
    dcP[0]  = readFloatU16(msg, 18, 1.0e0);

    dcV[1]  = readFloatU16(msg, 20, 1.0e-1);
    dcI[1]  = readFloatU16(msg, 22, 1.0e-2);
    dcP[1]  = readFloatU16(msg, 24, 1.0e0);

    dcV[2]  = readFloatU16(msg, 26, 1.0e-1);
    dcI[2]  = readFloatU16(msg, 28, 1.0e-2);
    dcP[2]  = readFloatU16(msg, 30, 1.0e0);

    busV    = readFloatS16(msg, 32, 1.0e-1);
    temp    = readFloatS16(msg, 34, 1.0e-1);
    // Omitting 36 => GFCI?
    P       = readFloatU16(msg, 38, 1.0e0);
    Q       = readFloatS16(msg, 40, 1.0e0);
    Pf      = readFloatS16(msg, 42, 1.0e-3);

    acV[0]  = readFloatU16(msg, 44, 1.0e-1);
    acI[0]  = readFloatU16(msg, 46, 1.0e-2);
    acF[0]  = readFloatU16(msg, 48, 1.0e-2);
    // Omitting 50 => DC current
    acP[0]  = readFloatU16(msg, 52, 1.0e0);
    acPf[0] = readFloatS16(msg, 54, 1.0e-3);

    acV[1]  = readFloatU16(msg, 56, 1.0e-1);
    acI[1]  = readFloatU16(msg, 58, 1.0e-2);
    acF[1]  = readFloatU16(msg, 60, 1.0e-2);
    // Omitting 62 => DC current
    acP[1]  = readFloatU16(msg, 64, 1.0e0);
    acPf[1] = readFloatS16(msg, 66, 1.0e-3);

    acV[2]  = readFloatU16(msg, 68, 1.0e-1);
    acI[2]  = readFloatU16(msg, 70, 1.0e-2);
    acF[2]  = readFloatU16(msg, 72, 1.0e-2);
    // Omitting 74 => DC current
    acP[2]  = readFloatU16(msg, 76, 1.0e0);
    acPf[2] = readFloatS16(msg, 78, 1.0e-3);

  }

}

SAJInverterData::~SAJInverterData(){}

int SAJInverterData::readIntU16(ModbusMessage msg, int position, float scale)
{
    
  uint16_t raw;

  msg.get(position, raw);

  return (int) raw ;

}

float SAJInverterData::readFloatU16(ModbusMessage msg, int position, float scale)
{
    
  uint16_t raw;

  msg.get(position, raw);

  return (float) raw * scale;;

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
    out &=  ( dcVLimits[0] < dcV[i]  && dcV[i]  < dcVLimits[1] ) && 
            ( dcILimits[0] < dcI[i]  && dcI[i]  < dcILimits[1] ) &&
            ( PLimits[0]   < dcP[i]  && dcP[i]  < PLimits[1] )   &&
            ( acVLimits[0] < acV[i]  && acV[i]  < acVLimits[1] ) &&
            ( acILimits[0] < acI[i]  && acI[i]  < acILimits[1] ) &&
            ( PLimits[0]   < acP[i]  && acP[i]  < PLimits[1] )   &&
            ( PfLimits[0]  < acPf[i] && acPf[i] < PfLimits[1] )  &&
            ( fLimits[0]   < acF[i]  && acF[i]  < fLimits[1] );
  }

  out &=  ( PLimits[0]  < P  && P  < PLimits[1] ) &&
          ( QLimits[0]  < Q  && Q  < QLimits[1] ) &&
          ( PfLimits[0] < Pf && Pf < PfLimits[1] );

  out &= ( busVLimits[0] < busV && busV < busVLimits[1] );

  out &= ( tempLimits[0] < temp && temp < tempLimits[1] );

  return out;
  
}

// Assignment operator
SAJInverterData& SAJInverterData::operator=(const SAJInverterData& m) {

  // Do anything only if not self-assigning
  if (this != &m) {
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
      acP[i]  = m.acP[i];
      acPf[i] = m.acPf[i];
    }
  }

  return *this;
}

// Adition operator
SAJInverterData& SAJInverterData::operator+(const SAJInverterData& m) {

    mode = m.mode; // Assign last value
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
      acP[i]  += m.acP[i];
      acPf[i] += m.acPf[i];
    }

  return *this;
}

SAJInverterData SAJInverterData::scale(float scale) {

    busV *= scale;
    temp *= scale;
    P    *= scale;
    Q    *= scale;
    Pf   *= scale;
    for (int i = 0; i < 3; i++) {
      dcV[i]  *= scale;
      dcI[i]  *= scale;
      dcP[i]  *= scale;
      acV[i]  *= scale;
      acI[i]  *= scale;
      acP[i]  *= scale;
      acPf[i] *= scale;
    }

  return *this;  
}

String SAJInverterData::json() {

  String outJsonMsg;

  StaticJsonDocument<1024> root;
  root["dcVpv1"]  = dcV[0];
  root["dcVpv2"]  = dcV[1];
  root["dcVpv3"]  = dcV[2];
  root["dcIpv1"]  = dcI[0];
  root["dcIpv2"]  = dcI[1];
  root["dcIpv3"]  = dcI[2];
  root["dcPpv1"]  = dcP[0];
  root["dcPpv2"]  = dcP[1];
  root["dcPpv3"]  = dcP[2];
  root["acVl1"]   = acV[0];
  root["acVl2"]   = acV[1];
  root["acVl3"]   = acV[2];
  root["acIl1"]   = acI[0];
  root["acIl2"]   = acI[1];
  root["acIl3"]   = acI[2];
  root["acPl1"]   = acP[0];
  root["acPl2"]   = acP[1];
  root["acPl3"]   = acP[2];
  root["acFl1"]   = acF[0];
  root["acFl2"]   = acF[1];
  root["acFl3"]   = acF[2];
  root["acPfl1"]  = acPf[0];
  root["acPfl2"]  = acPf[1];
  root["acPfl3"]  = acPf[2];
  root["busV"]    = busV;
  root["temp"]    = temp;
  root["pTotal"]  = P;
  root["qTotal"]  = Q;
  root["pfTotal"] = Pf;

  // Convert to JSON
  serializeJson(root, outJsonMsg);

  return outJsonMsg;

}

// Getters
int SAJInverterData::getMode(){
  return mode;
}

array<float,3> SAJInverterData::getdcV() {
  array<float,3> out;
  for (int i = 0; i < 3; i++) {
    out[i] = dcV[i];
  }
  return out;
}

array<float,3> SAJInverterData::getdcI() {
  array<float,3> out;
  for (int i = 0; i < 3; i++) {
    out[i] = dcI[i];
  }
  return out;
}

array<float,3> SAJInverterData::getdcP() {
  array<float,3> out;
  for (int i = 0; i < 3; i++) {
    out[i] = dcP[i];
  }
  return out;
}

array<float,3> SAJInverterData::getacV() {
  array<float,3> out;
  for (int i = 0; i < 3; i++) {
    out[i] = acV[i];
  }
  return out;
}

array<float,3> SAJInverterData::getacI() {
  array<float,3> out;
  for (int i = 0; i < 3; i++) {
    out[i] = acI[i];
  }
  return out;
}

array<float,3> SAJInverterData::getacF() {
  array<float,3> out;
  for (int i = 0; i < 3; i++) {
    out[i] = acF[i];
  }
  return out;
}

array<float,3> SAJInverterData::getacP() {
  array<float,3> out;
  for (int i = 0; i < 3; i++) {
    out[i] = acP[i];
  }
  return out;
}

array<float,3> SAJInverterData::getacPf() {
  array<float,3> out;
  for (int i = 0; i < 3; i++) {
    out[i] = acPf[i];
  }
  return out;
}

float SAJInverterData::getBusV() {
  return busV;
}

float SAJInverterData::getTemp() {
  return temp;
}

float SAJInverterData::getP() {
  return P;
}

float SAJInverterData::getQ() {
  return Q;
}

float SAJInverterData::getPf() {
  return Pf;
}

