//
//  $Id: genStreamable.cc,v 1.0 1996/12/18 06:41:35 andre Exp $
//

#include <stdlib.h>
#include <string.h>
#include <genString.h>
#include <genStreamable.h>

cl_GenArray  cl_GenStreamable::classRegistry(TRUE); // classRegistry owns its elements
cl_GenArray  cl_GenStreamable::inClassTable(TRUE); // inClassTable owns its elements

// cl_GenStreamEntry Class
// -------------------
cl_GenStreamEntry::cl_GenStreamEntry(const char* aName, StreamableCreator aCreator)
  :className(aName),creator(aCreator),createCount(0){
};

// cl_GenStreamable Class
// -------------------
unsigned long cl_GenStreamable::ClassIndex(const char* aClassName, cl_GenArray & anArray){
  cl_GenStreamEntry* entry=NULL;
  cl_GenIterator iter(anArray);
  while (entry=((cl_GenStreamEntry*)iter.Next())){
    if (strcmp(entry->className, aClassName)==0)
      return iter.CurrentIndex();
  };

  return NOT_A_CLASS_INDEX;
};

std::ostream& operator<<(std::ostream& os, cl_GenStreamable& s){
  unsigned long classIndex= cl_GenStreamable::OutClassIndex(s.ClassName());
  if (classIndex == cl_GenStreamable::NOT_A_CLASS_INDEX){
    std::cerr << "cl_GenStreamable::" << " operator<< unregistred class" << std::endl;
  }
  else{
    os.put(classIndex);
    s.write(os);
    os.put(classIndex);
  };
  return os;
};

std::istream& operator>>(std::istream& is, cl_GenStreamable& s){
  unsigned long classIndex= is.get();
  cl_GenStreamEntry* entry= (cl_GenStreamEntry*)cl_GenStreamable::inClassTable[classIndex];
  if (!entry) {// className assertion failed
    std::cerr << "cl_GenStreamable::" << "operator>>  invalid class index" << std::endl;
  }
  else{
    if (strcmp(entry->className,s.ClassName()))
      std::cerr << "cl_GenStreamable::" << "operator>>  unmatched class name" << entry->className << s.ClassName() << std::endl;
    else
      s.read(is);
  };
  if (is.get() != classIndex)
      std::cerr << "cl_GenStreamable::" << "operator>>  unmatched class index" << entry->className << std::endl;
  return is;
};



void cl_GenStreamable::RegisterClass(const char* aClassName, StreamableCreator aCreator){
  if (classRegistry.NbItems() == NOT_A_CLASS_INDEX){
      std::cerr << "cl_GenStreamable::" << "RegisterClass too many classes" << aClassName << std::endl;
      return;
  };

  cl_GenStreamEntry* entry=NULL;
  cl_GenIterator iter(classRegistry);
  while (entry=((cl_GenStreamEntry*)iter.Next())){
    if (strcmp(entry->className, aClassName)==0){// duplicate class name error
      std::cerr << "cl_GenStreamable::" << "RegisterClass duplicate class name :" << aClassName << std::endl;
      return;
    }
  };
  classRegistry.Add(new cl_GenStreamEntry(aClassName,aCreator));
};

StreamableCreator cl_GenStreamable::FindClassCreator(const char* aClassName){
  cl_GenStreamEntry* entry=NULL;
  unsigned long classIndex = OutClassIndex(aClassName);
  if (classIndex != NOT_A_CLASS_INDEX)
    entry=(cl_GenStreamEntry*)cl_GenStreamable::classRegistry[classIndex];
  if (entry)
    return (StreamableCreator)entry->creator;
  return (StreamableCreator)NULL;
};

cl_GenStreamable* cl_GenStreamable::Create(std::istream& is){
  cl_GenStreamable* newObject = NULL;
  // read class index from input stream
  unsigned long classIndex= is.get();
  cl_GenStreamEntry* entry=(cl_GenStreamEntry*)inClassTable[classIndex];

  if (entry==NULL)
    std::cerr << "cl_GenStreamable::" << "Create invalid class index" << std::endl;
  else{
    newObject = ((StreamableCreator)entry->creator)(entry->className);

    if (newObject){ // read object through virtual method
      entry->createCount++; // count number of object created
      newObject->read(is);

      if (is.get() != classIndex){
	delete (newObject);
	newObject=NULL;
	std::cerr << "cl_GenStreamable::" << "Create unmatched class index" << entry->className << std::endl;
      };
    };
  };
  // return object we just read
  return (newObject);
};

boolean cl_GenStreamable::WriteClassTable(std::ostream& os){
  cl_GenStreamEntry* entry=NULL;
  cl_GenIterator iter(classRegistry);
  os.put(classRegistry.NbItems());
  while (entry=((cl_GenStreamEntry*)iter.Next())){
    os << entry->className << std::endl;
  };
  return TRUE;
};

boolean cl_GenStreamable::ReadClassTable(std::istream& is){
  unsigned char nbItems;
  char c;
  cl_GenString className;
  is >> nbItems;
  inClassTable.Clear();
  //  inClassTable.Reserve(nbItems);
  for (unsigned short i=0;i < nbItems;i++){
    className="";
    c=is.get();
    while(!is.eof() && (c != '\n')){
      className.Add(c);
       c=is.get();
    };
    if (c!='\n'){
      std::cerr << "cl_GenStreamable::" << "ReadClassTable unexpected eof" << std::endl;
      return FALSE;
    };
    StreamableCreator classCreator= FindClassCreator(className);
    if (classCreator==NULL){
      std::cerr << "cl_GenStreamable::" << "ReadClassTable unregistred class name " << className << std::endl;
      return FALSE;
    };
    inClassTable.Add(new cl_GenStreamEntry(className,classCreator));
  };
  return TRUE;
};

const char* cl_GenStreamable::CurrentClassName(std::istream& is){
  const char* result=NULL;
  unsigned long classIndex= is.peek(); // peek class index do not extract it !
  cl_GenStreamEntry* entry=(cl_GenStreamEntry*)inClassTable[classIndex];
  if (entry==NULL)
    std::cerr << "cl_GenStreamable::" << "Create invalid class index" << std::endl;
  else
    result = entry->className;
  return result;
};
