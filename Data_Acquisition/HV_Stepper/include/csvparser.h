#ifndef _CSVPARSE_H
#define _CSVPARSE_H

/** Parser for comma separated files */

//BOOST
//#include <boost/lexical_cast.hpp>

#include "stdlib.h"
#include <iostream>
#include <string>
#include <sstream>

namespace CSVConst{

  const std::string separator = ",";
};

class CSVParser {

public:

  CSVParser(const std::string& sIn);

  ~CSVParser(){}

  typedef std::string::size_type size_type;
  

  template <class TYPE>  
  CSVParser & operator >> (TYPE& out);


 private:

  std::string m_sData;
  std::string::size_type m_nPos;
  void SkipSpaces();
  std::string next();

};

inline CSVParser::CSVParser(const std::string& sIn):
m_sData(sIn),
m_nPos(0)
{
 // constructer 
}

template <class TYPE>
inline CSVParser& CSVParser::operator >> (TYPE& out)  {

  // stream 
  std::string str = next(); 
  std::istringstream (str) >> out;
 
  /*try{
    out = boost::lexical_cast<TYPE>(str);
  }
  catch(boost::bad_lexical_cast& e){
      std::cerr << " ERROR " << e.what()   << std::endl; 
      abort();
      }*/
  
  return *this;
}

template <>
inline CSVParser& CSVParser::operator >> (std::string& out)  {

  // specializition for string - take up to first white space
  std::string tmp = next(); 
  size_type pos = 0;
  while (pos < tmp.size() && tmp[pos] != ' ') ++pos;
  out = tmp.substr(0,pos); 
  return *this;    
}

inline void CSVParser::SkipSpaces(){
  // skip leading whitespace
  while (m_nPos < m_sData.size() && m_sData[m_nPos] == ' ')
    ++m_nPos;
}

inline std::string CSVParser::next(){
  // next token 
  SkipSpaces();
  size_type pos = m_sData.find(CSVConst::separator,m_nPos); 
  std::string tStr = m_sData.substr(m_nPos,pos-m_nPos); 
  m_nPos = pos+1;

  return tStr;  
}

#endif

