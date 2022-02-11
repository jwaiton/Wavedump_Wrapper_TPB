#ifndef _PMT_H_
#define _PMT_H_

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <stdlib.h>

class PMT {

public:

  /** default constructor **/ 
  PMT(std::vector<std::string> &pmt_data);
  
  /** destructor **/
  ~PMT(){}
  
  /** PMT serial id **/ 
  std::string serial() const; 

  /** Sk [mUA/Im] a**/
  double sk() const;

  /** Skb **/
  double skb() const;

  /** Sp [A/Im] at 1500 V **/
  double sp() const;

  /** Idb [nA] at 1500 V **/
  double idb() const;

  /** Voltage for 10^7 Gain **/ 
  double workingVoltage() const;

  /** Dark count (cps) at working Voltage **/
  double darkCount() const;

  /** Transient time [ns] spread at working Voltage **/
  double tts() const;

  /** peak to valley at working voltage **/
  double peakToValley() const;

private:

 std::string m_serial;
 int m_sk;
 double m_skb;
 double m_sp;
 double m_idb;
 double m_wV;
 double m_dark;
 double m_tts;
 double m_peakToValley;

};

#endif