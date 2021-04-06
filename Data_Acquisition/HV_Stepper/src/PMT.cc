#include "PMT.h"

PMT::PMT(std::vector<std::string> &pmt_data) {
  //set parameters
  m_serial = pmt_data[0];
  m_sk = atoi(pmt_data[1].c_str());
  m_skb = atof(pmt_data[2].c_str());
  m_sp  = atof(pmt_data[3].c_str());
  m_idb = atof(pmt_data[4].c_str());
  m_wV = atof(pmt_data[5].c_str());
  m_dark = atof(pmt_data[6].c_str());
  m_tts = atof(pmt_data[7].c_str());
  m_peakToValley = atof(pmt_data[8].c_str());
}

std::string PMT::serial() const {
  return m_serial;
}

double PMT::sk() const {
  return m_sk;
}

double PMT::skb() const {
  return m_skb;
}
double PMT::sp() const {
  return m_sp;
}

double PMT::idb() const {
  return m_idb;
}

double PMT::workingVoltage() const {
  return m_wV;
}

double PMT::darkCount() const {
  return m_dark;
}

double PMT::tts() const {
  return m_peakToValley;
}