/*
 *  $Id: metaLang.h,v 1.0 1997/01/15 07:20:50 andre Exp $
 */

#ifndef _METALANGUAGE_H_
#define _METALANGUAGE_H_

#include <string.h>
#include <genArray.h>
#include <genString.h>
#include <genGlossary.h>
#include <metaStream.h>

enum metaRuleType {
  M_NOT_A_RULE,
  M_GRAMMAR,
  M_SEQUENCE,
  M_MANYOF,
  M_ONEOF,
  M_BOOLEAN,
  M_CHAR,
  M_IDENTIFIER,
  M_INTEGER,
  M_NULL,
  M_STRING,
  M_LITERAL,
  M_RULENAME,
  M_COMMENT,
  M_LITERALSTRING,
  M_PATHNAME,
  M_TOKEN,
  M_EOF
};

char* my_regcmp(const char* pattern);
char* my_regex(char* re, char* s);
void my_regfree(char* re);

class cl_MGrammar;

class cl_MRule {
  private :
  short line;
  static boolean skipping;
  static boolean verbose;
  static void* generatedCode;

protected :
  static cl_GenGlossary rulesDecl;
  static cl_GenString source;
  static cl_MParserIStream* metaStream;
  static cl_MParserIStream* sourceStream;
  static cl_MGrammar* grammar;

  public :
  cl_MRule();
  virtual boolean MetaParse()=0;
  virtual boolean Parse(void* context)=0;
  virtual boolean Match()=0;
  virtual short IsA() const =0;
  virtual std::ostream& write(std::ostream& os)const=0;
  virtual std::istream& read(std::istream& is)=0;
  virtual const char* Name()=0;
  virtual boolean Generate(void* context,const char* alias);
  void StreamTo(std::ostream& os) const;
  static cl_MRule* StreamFrom( std::istream& is);

  static cl_MRule* ParseGrammar(std::istream& is);
  static void* ParseSource(std::istream& is);

  static short TerminalType(const char* s);
  static void ShowError(const char* s);
  static void ShowError(const char* s1,const char* s2);
  static void ShowError(const char* s1,const char* s2,const char* s3);
  static void ShowWarning(const char* s);
  static void ShowWarning(const char* s1,const char* s2);
  static void ShowWarning(const char* s1,const char* s2,const char* s3);
 static void Skip();
  static void SetSkipping(boolean s){skipping=s;};
  static void* GeneratedCode() {return generatedCode;};
  virtual const char* Alias(){return Name();};
  static void SetVerbose(boolean v){verbose=v;};
};


class cl_MDefinition : public cl_MRule {
private:
protected:
  cl_GenString name;
public:
  virtual boolean MetaParse()=0;
  virtual boolean Parse(void* context)=0;
  virtual boolean Match()=0;
  virtual short IsA() const=0;

  virtual std::ostream& write(std::ostream& os)const=0 ;
  virtual std::istream& read(std::istream& is)=0;
  virtual const char* Name(){return name;};
  void SetName(const char* aName){name=aName;};
  static cl_MDefinition* MPFactory();
  static short StringToType(const char* s);
};

class cl_MDefElement : public cl_MRule {
private:
  boolean optional;
  cl_GenString alias;
public:
  cl_MDefElement();
  virtual boolean MetaParse()=0;
  virtual boolean Parse(void* context)=0;
  virtual boolean Match()=0;
  virtual short IsA() const=0;
  boolean IsOptional() {return optional;};
  void SetOptional(boolean o) {optional=o;};
  virtual const char* Alias(){return (const char*)alias;};
  void SetAlias(const char* anAlias) {alias = anAlias;};
  virtual std::ostream& write(std::ostream& os)const=0 ;
  virtual std::istream& read(std::istream& is)=0;
  static cl_MDefElement* MPFactory();
  static short CharToType(char c);
};

class cl_MBuildIn : public cl_MDefElement {
private:
public:
  virtual boolean MetaParse(){return TRUE;};
  virtual boolean Parse(void* context)=0;
  virtual boolean Match()=0;
  virtual short IsA() const=0;
  virtual std::ostream& write(std::ostream& os)const ;
  virtual std::istream& read(std::istream& is);
  static cl_MBuildIn* MPFactory();
  static short StringToType(const char* s);
};

class cl_MBoolean : public cl_MBuildIn {
private:
  boolean value;
public:
  cl_MBoolean();
  ~cl_MBoolean();
  virtual boolean Parse(void* context);
  virtual boolean Match();
  virtual short IsA() const {return M_BOOLEAN;};
  virtual const char* Name(){return "#boolean";};
  boolean Value(){return value;};
};

class cl_MChar : public cl_MBuildIn {
private:
  char value;
public:
  cl_MChar();
  ~cl_MChar();
  virtual boolean Parse(void* context);
  virtual boolean Match();
  virtual short IsA() const {return M_CHAR;};
  virtual const char* Name(){return "#char";};
  char Value(){return value;};
};

class cl_MIdentifier : public cl_MBuildIn {
private:
  cl_GenString value;
public:
  cl_MIdentifier();
  ~cl_MIdentifier();
  virtual boolean Parse(void* context);
  virtual boolean Match();
  virtual short IsA() const {return M_IDENTIFIER;};
  virtual const char* Name(){return "#id";};
  const char* Value(){return (const char*)value;};
};

class cl_MInteger : public cl_MBuildIn {
private:
  long value;
public:
  cl_MInteger();
  ~cl_MInteger();
  virtual boolean Parse(void* context);
  virtual boolean Match();
  virtual short IsA() const {return M_INTEGER;};
  virtual const char* Name(){return "#integer";};
  long Value(){return value;};
};

class cl_MNull : public cl_MBuildIn {
private:
public:
  cl_MNull();
  ~cl_MNull();
  virtual boolean Parse(void* context);
  virtual boolean Match();
  virtual short IsA() const {return M_NULL;};
  virtual const char* Name(){return "#null";};
};

class cl_MString : public cl_MBuildIn {
private:
  cl_GenString value;
public:
  cl_MString();
  ~cl_MString();
  virtual boolean Parse(void* context);
  virtual boolean Match();
  virtual short IsA() const {return M_STRING;};
  virtual const char* Name(){return "#string";};
  const char* Value(){return (const char*)value;};

};

class cl_MComment : public cl_MBuildIn {
private:
  cl_GenString value;
public:
  cl_MComment();
  ~cl_MComment();
  virtual boolean Parse(void* context);
  virtual boolean Match();
  virtual short IsA() const {return M_COMMENT;};
  virtual const char* Name(){return "#comment";};
  const char* Value(){return (const char*)value;};

};
class cl_MLiteralString : public cl_MBuildIn {
private:
  cl_GenString value;
public:
  cl_MLiteralString();
  ~cl_MLiteralString();
  virtual boolean Parse(void* context);
  virtual boolean Match();
  virtual short IsA() const {return M_LITERALSTRING;};
  virtual const char* Name(){return "#literal";};
  const char* Value(){return (const char*)value;};

};

class cl_MPathName : public cl_MBuildIn {
private:
  cl_GenString value;
public:
  cl_MPathName();
  ~cl_MPathName();
  virtual boolean Parse(void* context);
  virtual boolean Match();
  virtual short IsA() const {return M_PATHNAME;};
  virtual const char* Name(){return "#pathname";};
  const char* Value(){return (const char*)value;};

};

class cl_MLiteral : public cl_MDefElement {
private:
  cl_GenString value;
public:
  cl_MLiteral();
  ~cl_MLiteral();
  virtual boolean MetaParse();
  virtual boolean Parse(void* context);
  virtual boolean Match();
  virtual short IsA() const {return M_LITERAL;};
  virtual std::ostream& write(std::ostream& os)const ;
  virtual std::istream& read(std::istream& is);
  virtual const char* Name(){return (const char*)value;};
  const char* Value(){return (const char*)value;};

};

class cl_MRuleName : public cl_MDefElement {
private:
  cl_GenString name;
  cl_MRule* rule;
public:
  cl_MRuleName();
  ~cl_MRuleName();
  cl_MRule* ResolveName();
  virtual boolean MetaParse();
  virtual boolean Parse(void* context);
  virtual boolean Match();
  virtual boolean Generate(void* context,const char* alias);
  virtual short IsA() const {return M_RULENAME;};
  virtual std::ostream& write(std::ostream& os)const ;
  virtual std::istream& read(std::istream& is);
  virtual const char* Name(){return (const char*)name;};

};


class cl_MSequence : public cl_MDefinition {
private:
  cl_GenArray parts;
public:
  cl_MSequence();
  ~cl_MSequence();
  long Add(cl_MRule* r){return parts.Add(r);};
  virtual boolean MetaParse();
  virtual boolean Parse(void* context);
  virtual boolean Match();
  virtual short IsA() const {return M_SEQUENCE;};
  virtual std::ostream& write(std::ostream& os)const ;
  virtual std::istream& read(std::istream& is);
};

class cl_MManyOf : public cl_MDefinition {
private:
  cl_MDefElement* repeatDef;
  cl_MDefElement* begin;
  cl_MDefElement* end;
  cl_MDefElement* separator;
public:
  cl_MManyOf();
  ~cl_MManyOf();
  virtual boolean MetaParse();
  virtual boolean Parse(void* context);
  virtual boolean Match();
  virtual short IsA() const {return M_MANYOF;};
  virtual std::ostream& write(std::ostream& os)const ;
  virtual std::istream& read(std::istream& is);
};

class cl_MOneOf : public cl_MDefinition {
private:
  cl_GenArray cases;
  cl_MDefElement* matchingCase;
public:
  cl_MOneOf();
  ~cl_MOneOf();
  long Add(cl_MRule* r){return cases.Add(r);};
  virtual boolean MetaParse();
  virtual boolean Parse(void* context);
  virtual boolean Match();
  virtual short IsA() const {return M_ONEOF;};
  virtual std::ostream& write(std::ostream& os)const ;
  virtual std::istream& read(std::istream& is);
};

class cl_MToken : public cl_MDefinition {
private:
  cl_GenString def; // regular expression definition
  cl_GenString value;
  char *comp; // compiled regular expression
  char *after; // point to next unmatched character
  char *start; // point to first token character
public:
  cl_MToken();
  ~cl_MToken();
  virtual boolean MetaParse();
  virtual boolean Parse(void* context);
  virtual boolean Match();
  virtual short IsA() const {return M_TOKEN;};
  virtual std::ostream& write(std::ostream& os)const ;
  virtual std::istream& read(std::istream& is);
  const char* Value(){return (const char*)value;};
};

class cl_MEOF : public cl_MBuildIn {
private:
public:
  cl_MEOF();
  ~cl_MEOF();
  virtual boolean Parse(void* context);
  virtual boolean Match();
  virtual short IsA() const {return M_EOF;};
  virtual const char* Name(){return "#eof";};
};

class cl_MGrammar : public cl_MDefinition {
private:
  boolean skipping,caseSensitive;
  cl_MRuleName mainRule,skipRule;

  cl_GenArray rules;

public:
  cl_MGrammar();
  ~cl_MGrammar();
  long Add(cl_MRule *r){return rules.Add(r);};
  virtual boolean MetaParse();
  virtual boolean Parse(void* context);
  virtual boolean Match();
  virtual short IsA() const {return M_GRAMMAR;};
  virtual std::ostream& write(std::ostream& os)const ;
  virtual std::istream& read(std::istream& is);
  boolean Verify();
  void SkipInSource();
  boolean IsCaseSensitive(){return caseSensitive;};
  void SetCaseSensitive(boolean cs){caseSensitive=cs;};
};

inline void cl_MRule::Skip(){
  if (!skipping)
    grammar->SkipInSource();
};

#endif // _METALANGUAGE_H_
