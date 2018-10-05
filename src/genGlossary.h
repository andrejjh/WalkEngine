/*
 *  $Id: genGlossary.h,v 1.0 1996/05/02 07:20:50 andre Exp $
 */

#ifndef _GLOSSARY_H_
#define _GLOSSARY_H_
#include <genArray.h>
#include <genString.h>

class cl_GenGlossaryEntry{
public:
    cl_GenString name;
    void* object;
    unsigned short count;
    cl_GenGlossaryEntry(const char* aName,  void * anObject);
};

class cl_GenGlossary{
private:
  boolean ownsItems,large;
  cl_GenArray repository;
  cl_GenArray fastAccessTable;
  cl_GenGlossaryEntry* FindEntry(const char* aName) const;
public:
  cl_GenGlossary(boolean owns=TRUE,boolean large=FALSE);
  ~cl_GenGlossary();
  unsigned long Add(const char* aName, void* anObject);
  void Remove(const char* aName);
  short HasObject(const char* aName) const;
  void* FindObject(const char* aName) const;
  void* ExtractObject(const char* aName);
  unsigned short FindObjectCount(void* anObject) const;
  const char* FindObjectName(void* anObject) const;
  const cl_GenArray& Repository() const{return repository;};
};
inline short cl_GenGlossary::HasObject(const char* aName)const{
  return (FindEntry(aName) !=0);
};
#endif
