/*
 *  $Id: genParser.h,v 1.0 1996/05/07 07:20:50 andre Exp $
 */

#ifndef _GENPARSER_H_
#define _GENPARSER_H_

#include <iostream>
#include <stdlib.h>
#include <genString.h>
#include <genLang.h>

class cl_GenParser {
private :
    unsigned long curr;    // current char index in text
    unsigned long currLine;
    cl_GenBlock *main;
    cl_GenString text;
    cl_GenString token1, token2, token3,  token4, token5;
    void VarName(cl_GenString& leftPart, cl_GenString& rightPart);
    unsigned short NextToken(cl_GenString& aToken, const char* toSkip,
		const char* delimiters, short skipDelimiter=1);
	unsigned short NextWord(cl_GenString& aWord,
		const char*validChars);
    char Current(); // current character in text
    char Skip(); // skip one character in text
//    char Back(); // go back one character in text
    void SkipTo(const char* toSkip, char until);
    void SkipChars(const char* toSkip);
    unsigned short IdentifyToken(const cl_GenString& token);
    void WriteSourceLine();
    void ShowError(const char* error);
    void ShowError(const char* error, const char* name);
protected :
public :
    boolean verbose;
    cl_GenParser();
    ~cl_GenParser();
    void Load(std::istream &is);
    void Include(std::istream &is);
    cl_GenClass* ParseClass(const char* name);
    cl_GenBlock* ParseBlock(const char* blockName);
    cl_GenStatement* ParseStatement();
    const char* Text() const {return (const char*)text;};
};

inline char cl_GenParser::Current(){return text.Get(curr);};
inline char cl_GenParser::Skip(){
    curr++;
    char at=text.Get(curr);
    if (at == '\n')
	currLine++;
    return at;};
//inline char cl_GenParser::Back(){curr--; return text.Get(curr);};
inline void cl_GenParser::SkipTo(const char* toSkip, char until){
    char c=Current();
    while ((c != 0) && (c != until)){
	if (strchr(toSkip, c)==0){
	    std::cout << "Warning : Unexpected " << c << " character, skipped." << std::endl;
	    WriteSourceLine();
	}
	c=Skip();
    };
    if (c==until)
	Skip();
    else {
	std::cout << until << " expected." << std::endl;
	exit(-1);
    }
};
inline void cl_GenParser::SkipChars(const char* toSkip){
    char c=Current();
    while ((c != 0) && (strchr(toSkip,c))){
	c=Skip();
    };
};
inline unsigned short cl_GenParser::NextToken(cl_GenString& aToken,
    const char* toSkip,const char* delimiters,  short skipDelimiter){
    aToken = "";
    char c=Current();
    while ((c != 0) && (strchr(delimiters, c)==0)) {
	if (strchr(toSkip, c)==0)
	    aToken.Add(c);
	c=Skip();
    };
    if (skipDelimiter)
	Skip();
/*
    std::cout << "token=\"" << aToken << "\"" << ", skip=\""
    << toSkip << "\", delimiters=\"" << delimiters <<
    "\" at=" << c << std::endl;
*/
    return (strlen(aToken)); // more token to read
};

inline unsigned short cl_GenParser::NextWord(cl_GenString& aWord,
   const char*validChars){
	aWord="";
	char c=Current();
	while ((c!=0) && (strchr(validChars,c) !=0)){
		aWord.Add(c);
		c=Skip();
	}
  return 0;
}

#endif // _GENPARSER_H_
