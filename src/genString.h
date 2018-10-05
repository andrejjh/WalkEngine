/*
 *  $Id: genString.h,v 1.0 1996/05/06 07:20:50 andre Exp $
 */

#ifndef _GENSTRING_H_
#define _GENSTRING_H_

#include <iostream>
#define PRINT(x) if(verbose) std::cout <<  x << std::endl;


class cl_GenString {
private:
    unsigned long size,usedSize;
    char* ptr;
    void Realloc(unsigned long aSize);

public:
    cl_GenString(const char* aString="");
    ~cl_GenString();
    cl_GenString& operator=(const char *aString);
    cl_GenString& operator=(const cl_GenString &aString);
    operator const char*() const;
    void Assign(const char *aString);
    void Assign(const char *aString, unsigned long size);
    void Concat(const char *aString);
    void Reserve(unsigned long size);
    void Add(const char);
    void DeleteLast();
    void Truncate(unsigned long at);
    char Get(unsigned long index) const;
  unsigned long Size()const {return size;}; // return maximum string size
  unsigned long StringSize() const {return usedSize;}; // return used size
    std::istream& read(std::istream& is);
    std::ostream& write(std::ostream& os) const;
	static void PutShort(std::ostream & os, unsigned short s);
	static unsigned short GetShort(std::istream& is);
	static void PutLong(std::ostream & os, unsigned long s);
	static unsigned long GetLong(std::istream& is);
};

inline cl_GenString::operator const char* () const {return ptr;};
inline cl_GenString& cl_GenString::operator=(const char* aString){
    Assign(aString);
    return *this;
};
inline cl_GenString& cl_GenString::operator=(const cl_GenString &aString){
    Assign((const char *)aString);
    return *this;
};
inline void cl_GenString::PutShort(std::ostream & os, unsigned short s){
	unsigned char c;
	for (int i =0;i<sizeof(short);i++) {
		c= (unsigned char)(s & 0xFF); os.put(c);
		s= s >> 8;
	};
};
inline unsigned short cl_GenString::GetShort(std::istream& is){
	unsigned short shift=0;
	unsigned short s=0;
	for (int i =0;i<sizeof(short);i++) {
		s += is.get() << shift;
		shift +=8;
	};
    return s;
};
inline void cl_GenString::PutLong(std::ostream & os, unsigned long s){
	unsigned char c;
	for (int i =0;i<sizeof(long);i++) {
		c= (unsigned char)(s & 0xFF); os.put(c);
		s= s >> 8;
	};
};
inline unsigned long cl_GenString::GetLong(std::istream& is){
	unsigned short shift=0;
	unsigned long s=0;
	for (int i =0;i<sizeof(long);i++) {
		s += is.get() << shift;
		shift += 8;
	};
    return s;
};
#endif
