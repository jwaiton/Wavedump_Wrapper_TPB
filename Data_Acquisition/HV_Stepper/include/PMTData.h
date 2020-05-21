#ifndef PMTData_H
#define PMTdata_H 1
/** @class PMTData PMTData.h
  * PMT data from Hammatsu 
  * M Needham August 2018
**/

#include "csvparser.h"
#include <string>
#include <sstream>
#include <iostream>


class PMTData{

public:

  /** default constructor **/ 
  PMTData(){}
  
  /** destructor **/
  ~PMTData(){}
  
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

  /** streamer from string **/
  void stream(CSVParser& parser);

  /** Operator overloading for stringoutput */
    friend std::ostream& operator<< (std::ostream& s,
                                     const PMTData& obj)
  {
      return obj.fillStream(s);
  }
 
  /** Fill the ASCII output stream **/
  std::ostream& fillStream(std::ostream& s) const;

  /** Additional print to string. May be useful in python ? **/ 
  std::string print() const; 
  
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


inline std::string PMTData::serial() const{
  return m_serial;
}

inline double PMTData::sk() const{
  return (double)m_sk;
}

inline double PMTData::skb() const{
  return m_skb;
}

inline double PMTData::sp() const{
  return m_sp;
}

inline double PMTData::idb() const{
  return m_idb;
}

inline double PMTData::darkCount() const{
  return m_dark;
}

inline double PMTData::workingVoltage() const{
  return m_wV;
}

inline double PMTData::tts() const{
  return m_tts;
}

inline double PMTData::peakToValley() const{
  return m_peakToValley;
}

inline void PMTData::stream(CSVParser& parser){
  parser >> m_serial >> m_sk >> m_skb  >> m_sp >> m_idb >> m_wV >>m_dark >> m_tts >> m_peakToValley;
}

inline std::ostream& PMTData::fillStream(std::ostream& s) const{

  // private helper for stream to iostream 
  s << "** PMTdata for " << serial()  << " **" << std::endl;
  s << "sk: " << sk() << " muA/Im" << std::endl; 
  s << "skb: " << skb() << std::endl;
  s << "sp: " << sp() << " A/Im" << std::endl;
  s << "Idb: " << idb() << " nA" << std::endl;
  s << "Working Voltage: " << workingVoltage() << " V" << std::endl;
  s << "Dark Count " << darkCount() << " cps" << std::endl;
  s << "TTS " << tts() << " ns" << std::endl;
  s  << "peakToValley " << peakToValley() << std::endl;
  return s;
}

inline std::string PMTData::print() const{
  std::ostringstream o;
  fillStream(o);
  return o.str();
}


#endif
