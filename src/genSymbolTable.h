/*
 *  $Id: genSymbolTable.h,v 1.0 1996/12/16 16:00:00 andre Exp $
 */

#ifndef _SYMBOLTABLE_H_
#define _SYMBOLTABLE_H_

#include <iostream>
#include <genArray.h>
#include <genString.h>


class cl_GenSymbolTable {
  // This Symbol Table class can contains up to 65535 strings.
  // It provide a string to index and index to string conversion.

public:
  static const unsigned short NOT_A_SYMBOL = 65535;

private:
  cl_GenArray strings;

public:
  cl_GenSymbolTable();
  ~cl_GenSymbolTable();
  unsigned short AddSymbol(const char* aSymbol);
  void Clear(){
    strings.Clear();};
  unsigned short  FindSymbol(const char* aSymbol) const;
  const char* operator[](const unsigned short) const;
  unsigned short NbSymbols() const {
    return strings.NbItems();};
  std::istream& read(std::istream& is);
  std::ostream& write(std::ostream& os) const;
};

inline const char* cl_GenSymbolTable::operator[](const unsigned short idx) const{
  cl_GenString *aString= (cl_GenString*)strings[idx];
  if (aString)
    return ((const char*)*aString);
  else
    return NULL;
};

class cl_GenGlossaryEntry{
public:
  void* object;
  unsigned short symbolIndex;
  unsigned short count;
  cl_GenGlossaryEntry( void * anObject, unsigned short aSymbol, unsigned short count=0);
};

class cl_GenGlossary{
  // This Glossary class is optimized in such a way that symbols/strings
  // are maintained outside of the glossary, in a symbol table object.
  // This allow several glossary to share strings and to reduce the memory usage.
  // A direct access[by symbol index] is used for performance reasons.

  // The fate of the objects pointed by this glossary depends on the ownsItems data member.
  // Strings are stored in the symbol table and follow its fate.

private:
  cl_GenSymbolTable& symbolTable; // where symbols (or strings) are stored
  cl_GenArray glossaryTable; // where Glossary entries are stored
  boolean ownsItems;

  cl_GenGlossaryEntry* FindEntry(const char* aName) const;
public:
  cl_GenGlossary(cl_GenSymbolTable& st, boolean owns=TRUE);
  ~cl_GenGlossary();
  unsigned long Add(const char* aName, void* anObject);
  void Remove(const char* aName);
  short HasObject(const char* aName) const;
  void* FindObject(const char* aName) const;
  unsigned short FindObjectCount(void* anObject) const;
  std::istream& read(std::istream& is);
  std::ostream& write(std::ostream& os) const;

};
#endif
