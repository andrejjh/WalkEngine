/*
 *  $Id: metaStream.h,v 1.0 1997/01/21 07:20:50 andre Exp $
 */

#ifndef _METASTREAM_H_
#define _METASTREAM_H_

#include <iostream>
#include <string>
#include <fstream>
#include <genArray.h>
#include <genString.h>

class cl_MParserIStream{
  private :
  cl_GenString theInput;
  std::istream& is;
  std::streampos where;
  public :
  static const char *IFile;
  cl_MParserIStream(std::istream& i);
  boolean GetString(cl_GenString& s, char until);
  boolean GetAlnumString(cl_GenString& s);
  boolean GetAlpunctString(cl_GenString& s);
  boolean GetIdentifier(cl_GenString& s, const char* delimiters, boolean skipDelimiters=TRUE);
  short GetChar(const char* toSkip);
  short SkipChar();
  short At();
  boolean AtEnd();
  void SavePosition();
  void RestorePosition();
  short GetShort();
  void EatWhite();
  void ShowWhere(const char* errMsg);
  const char* WhereInInput();
  void Advance(short delta);
  boolean MatchString(cl_GenString& s,boolean caseSensitive);
};


inline void cl_MParserIStream::Advance(short delta){
  is.seekg(delta, std::ios::cur);
};

inline short cl_MParserIStream::At(){
  return is.peek();};
inline boolean cl_MParserIStream::AtEnd(){
  return (is.peek() == EOF);};
inline void cl_MParserIStream::SavePosition(){
  where = is.tellg();
};
inline void cl_MParserIStream::RestorePosition(){
  is.seekg(where);
};
inline short cl_MParserIStream::GetShort(){
  short s;
  is >> s;
  return s;
}
inline void cl_MParserIStream::EatWhite(){
  short c=0;
  while ((c = is.peek())  && (c != EOF) && (strchr(" \n\t", c)!=0) ){
    c = is.get();
  };
};



#endif // _METASTREAM_H_
