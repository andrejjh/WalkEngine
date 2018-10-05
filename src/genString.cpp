//
//  $Id: genString.cc,v 1.0 1996/05/06 06:41:35 andre Exp $
//

#include <string.h>
#include <stdlib.h>
#include <genString.h>


cl_GenString::cl_GenString(const char *aString)
: ptr(0),size(0){
    size = strlen(aString);
    ptr =new char[size+1];
    if (!ptr){
	std::cout << "cl_GenString::cl_GenString cannot allocate!!!" << std::endl;
	exit (-1);
    }
    strcpy(ptr, aString);
    usedSize=size;
};

cl_GenString::~cl_GenString(){
    if (ptr) delete ptr;
};

void cl_GenString::Assign(const char *aString){
  usedSize=strlen(aString);
  if (usedSize > size) {
    if (ptr) delete ptr;
    size = usedSize;
    ptr =new char[size+1];
    if (!ptr){
      std::cout << "cl_GenString::cl_GenString cannot allocate!!!" << std::endl;
      exit (-1);
    }
  }
  strcpy(ptr, aString);
};

void cl_GenString::Assign(const char *aString, unsigned long aSize){
  usedSize=aSize;
  if (usedSize > size) {
    if (ptr) delete ptr;
    size = usedSize;
    ptr =new char[size+1];
    if (!ptr){
      std::cout << "cl_GenString::cl_GenString cannot allocate!!!" << std::endl;
      exit (-1);
    }
  }
  memcpy(ptr, aString, aSize);
  ptr[aSize]='\0';
};

void cl_GenString::Concat(const char *aString){
    usedSize += strlen(aString);
    Realloc(usedSize);
    strcat(ptr, aString);
};

void cl_GenString::Realloc(unsigned long aSize) {
    if ( aSize > size) {
        char * newPtr =new char[aSize+1];
	if (!newPtr){
	    std::cout << "cl_GenString::cl_GenString cannot allocate!!!" << std::endl;
	    exit(-1);
	}
	strcpy(newPtr, ptr);
	if (ptr) delete ptr;
	ptr = newPtr;
	size = aSize;
    }
};
void cl_GenString::Reserve(unsigned long aSize){
    Realloc(aSize);
    Assign("");

};
void cl_GenString::Add(const char c){
    if (usedSize == size){
	if (size)
	    Realloc(2*size);
	else
	    Realloc(1);
    };
    ptr[usedSize]=c;
    usedSize++;
    ptr[usedSize]='\0';
};
char cl_GenString::Get(unsigned long index) const{
    if (index < usedSize)
	return ptr[index];
    return 0;
}

void cl_GenString::DeleteLast() {
  if (usedSize) {
    usedSize--;
    ptr[usedSize]='\0';
  }
}
void cl_GenString::Truncate(unsigned long at) {
//    if (at >= size)
//	std::std::cout  << "cl_GenString::Truncate()::check1" << std::std::endl;
//   if (at >= strlen(ptr))
//	std::std::cout  << "cl_GenString::Truncate()::check2" << std::std::endl;
  usedSize=at;
  ptr[usedSize]='\0';
}

std::istream& cl_GenString::read(std::istream& is){
    usedSize = GetShort(is);
//    std::std::cout << "Reading " << strSize << " character(s) string" << std::std::endl;
    Realloc(usedSize);
    for (unsigned long index =0;index < usedSize;index++)
	ptr[index] = is.get();
    ptr[usedSize]='\0';
    return is;
};
std::ostream& cl_GenString::write(std::ostream& os) const {
//    std::std::cout << "Writing " << strSize << " character(s) string" << std::std::endl;
    PutShort (os,(unsigned short)usedSize);
    for (unsigned long index =0;index < usedSize;index++)
	os.put(ptr[index]);
    return os;
};
