/*
 *  $Id: metaStream.cc,v 1.0 1997/01/21 07:20:50 andre Exp $
 */

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <genString.h>
#include <metaStream.h>

// cl_MParserIStream Class
// ----------------------
const char * cl_MParserIStream::IFile=NULL;

cl_MParserIStream::cl_MParserIStream(std::istream& i)
  :is(i){

	is.seekg (0, std::ios::end);
    theInput.Reserve(1+is.tellg()); // reserve memory for the input
	is.seekg (0, std::ios::beg);
   //read is in theInput string (for regex analysis only!!!)
    while(is.peek() != EOF) {
      theInput.Add((char)is.get());
    };
    //reset stream position at begining
	is.clear();
	is.seekg(0,std::ios::beg);
};


const char* cl_MParserIStream::WhereInInput(){
  where = is.tellg();
  return (((const char*)theInput)+where);
};


short cl_MParserIStream::GetChar(const char* toSkip){
  int c=0;
  while ((c = is.peek())  && (c != EOF) && (strchr(toSkip, c)!=0)){
    c = is.get();
  };
  return c;
};

short cl_MParserIStream::SkipChar(){
	int c=0;
    is.get();
	return c;
};

boolean cl_MParserIStream::GetString(cl_GenString& result, char until){
  result="";
  int c;

  while ( (c = is.get()) && (c != EOF) && (c != until)){
    result.Add(c);
  };

  return (result.StringSize());
};



boolean cl_MParserIStream::GetIdentifier(cl_GenString& result, const char* delimiters, boolean skipDelimiters){
  result="";
  int c;
  // skip delimiters if any
  while ( skipDelimiters && (c = is.peek()) && (c != EOF) && (strchr(delimiters, c)!=0)){
    is.get();
  };

  // take characters until we encounter one of the delimiters
  //    while ( (c = is.peek()) && (isalnum(c))){
    while ( (c = is.peek()) && (c != EOF) && (strchr(delimiters, c)==0)){
      is.get();
      if  (c== '\\') {
	c = is.get();
	switch (c){
	case('t') : c = '\t';break;
	case('n') : c= '\n';break;
	case ('\'') : c= '\'';break;
	default :
	  std::cout << "cl_MParserIStream::GetIdentifier" << "unexpected escape sequence...skipped";
	};
      };
      result.Add(c);
  };

  return (result.StringSize());
};

boolean cl_MParserIStream::GetAlnumString(cl_GenString& result){
  result="";
  short c;
  c = is.peek();
  if (isalpha(c)){
    do {result.Add(is.get());}
    while ( (c = is.peek()) && (isalnum(c) || (c == '_')));
  };

  return (result.StringSize());
};

boolean cl_MParserIStream::GetAlpunctString(cl_GenString& result){
  result="";
  short c;
  while ( (c = is.peek()) && (isalpha(c) || (ispunct(c)))){
    result.Add(is.get());
  };

  return result.StringSize();
};


void cl_MParserIStream::ShowWhere(const char* errMsg){
  int lineNb,prevLine, lineStart;
  char c;
  if (!is.good()) {
    std::cout << "problem with input stream:" << errMsg << std::endl;
    return;
  };
  where = is.tellg();
  is.seekg(0);
  // count lines
  lineNb =1;
  lineStart = prevLine = 0;
  for (int cur=0;cur < where;cur++){
    if ((char)(is.get()== '\n')){
      lineNb++;
      prevLine = lineStart;
      lineStart = cur+1;
    };
  };
  if (IFile)
    std::cout << "\"" << IFile << "\",";

  std::cout << " at  line " << lineNb << " character " << ((int)where)-lineStart << " :" << errMsg << std::endl;

  if (lineStart){// display prev line
    is.seekg(prevLine);
    do {
      c = is.get();
      std::cout << c;
    } while ((!is.eof()) && (c != '\n'));
  }
  else {// no prevLine
    is.seekg(lineStart);
  }
  // display current line
  do {
    c = is.get();
    std::cout << c;
  } while ((!is.eof()) && (c != '\n'));
  for (int cur = lineStart; cur < where;cur++)
    std::cout << ' ';
  std::cout << "^" << std::endl;
};


boolean cl_MParserIStream::MatchString(cl_GenString& s,boolean caseSensitive){
	if (s.StringSize()==1) { // single character string
		if (caseSensitive)
			return (At() == *((const char*)s));
		else
			return (toupper(At()) == toupper(*((const char*)s)));
	} else {// multiple character string
		char* valuePtr = (char*)(const char*)s;
		const char* endPtr = valuePtr + s.StringSize();
		SavePosition();
		if (caseSensitive) {
			while ((valuePtr < endPtr) && (is.peek() == *valuePtr))
				{is.get(); valuePtr++;}
		}
		else {
			while ((valuePtr < endPtr) && (toupper(SkipChar()) == toupper(*valuePtr)))
				{valuePtr++;}
		}
		RestorePosition();
		return (valuePtr == endPtr);
	};
}
