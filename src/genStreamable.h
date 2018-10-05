//
//  $Id: genStreamable.h,v 1.0 1996/12/18 06:41:35 andre Exp $
//

#ifndef _GENSTREAMABLE_H_
#define _GENSTREAMABLE_H_


#include <iostream>
#define PRINT(x) if(verbose) std::cout <<  x << std::endl;
#include <genArray.h>

class cl_GenStreamable;
typedef cl_GenStreamable* (*StreamableCreator)(const char* className);

class cl_GenStreamEntry{
public:
  cl_GenString className;
  StreamableCreator creator;
  unsigned short createCount;
  cl_GenStreamEntry(const char* aName, StreamableCreator aCreator);
};

class cl_GenStreamable {
  public :
  static const unsigned char NOT_A_CLASS_INDEX = 0xff;

private:
  static cl_GenArray inClassTable;
  static cl_GenArray classRegistry;
  static unsigned long ClassIndex(const char* aClassName, cl_GenArray& anArray);

public :
  virtual const char* ClassName() const =0;
  virtual std::ostream& write(std::ostream& os)=0;
  virtual std::istream& read(std::istream& is)=0;

  friend std::ostream& operator<<(std::ostream& os, cl_GenStreamable& s);
  friend std::istream& operator>>(std::istream& is, cl_GenStreamable& s);

  static void RegisterClass(const char* aClassName, StreamableCreator aCreator);
  static StreamableCreator FindClassCreator(const char* aClassName);
  static unsigned long InClassIndex(const char* aClassName){
    return ClassIndex(aClassName, inClassTable);};
  static unsigned long OutClassIndex(const char* aClassName){
    return ClassIndex(aClassName, classRegistry);};

  static cl_GenStreamable* Create(std::istream& is);
  static boolean WriteClassTable(std::ostream& os);
  static boolean ReadClassTable(std::istream& is);
  static const char* CurrentClassName(std::istream& is);
};


#endif // _GENSTREAMABLE_H_
