/*
 *  $Id: genLang.h,v 1.0 1996/05/02 07:20:50 philip Exp $
 *
 *  Copyright (C) 1995 by Sony Telecom Europe (STE)
 *  All rights reserved.
 *
 *  This software or any part of it, may not be used in any form
 *  without permission.
 */

#ifndef _GENLANGUAGE_H_
#define _GENLANGUAGE_H_

#include <fstream>
#include <genArray.h>
#include <genGlossary.h>
#include <genString.h>
#include <genNode.h>


enum GenStatementType {
	NOT_A_KEYWORD,
	GENBLOCK,
	GENTEXT,
	GENVAR,
	GENFOR,
	GENENDFOR,
	GENIF,
	GENELSE,
	GENENDIF,
	GENCR,
	GENINDENT,
	GENUNINDENT,
	GENTHATSALL,
	GENEMPTY,
	GENCOMMENT,
	GENSET,
	GENSETSTRING,
	GENSUBSTR,
	GENBOOLEXPR,
	GENBOOLVAR,
	GENVAREQVAL,
	GENVAREQVAR,
	GENTAB,
	GENDO,
	GENENDBLOCK,
	GENINCLUDE,
	GENCLASS,
	GENENDCLASS,
	GENNEW,
	GENADD,
	GENREMOVE,
	GENCLEAN,
	GENONDO,
	GENPARSENODE,
	GENINSERT,
        GENASCTIME,
	GENPARSEMETA,
	GENAPPEND,
	GENAPPENDSTRING,
	GENENV,
	GENGETENV,
	GENINSERTVAR,
	GENPARSEMETAVAR,
	GENOFSTREAM
};
const unsigned short MAXINDENT = 80;
const unsigned short INDENTSTEP = 4;

class cl_GenStatement;
class cl_GenText;
class cl_GenVariable;
class cl_GenFor;
class cl_GenEndFor;
class cl_GenIf;
class cl_GenElse;
class cl_GenEndIf;
class cl_GenCR;
class cl_GenIndent;
class cl_GenUnIndent;
class cl_GenThatsAll;
class cl_GenEmpty;
class cl_GenSet;
class cl_GenBlock;
class cl_GenTAB;
class cl_GenDo;
class cl_GenEndBlock;
class cl_GenClass;
class cl_GenEndClass;
class cl_GenNew;
class cl_GenAdd;
class cl_GenRemove;
class cl_GenClean;
class cl_GenParseNode;
class cl_GenAsctime;
class cl_GenAppend;
class cl_GenAppendString;
class cl_GenEnv;
class cl_GenGetEnv;
class cl_GenInsertVar;
class cl_GenParseMetaVar;
class cl_GenOfstream;

class cl_GenStatement {
private :
  static char streamHeader[];
protected :
  static cl_GenString defDir;
  static std::ofstream defOs;
  static cl_GenNode environment;
  static cl_GenBlock *currBlock;
  const char* source;
  long sourceLine;
  static const char* atSource;
  static long atLine;
  static long nbStatements;
  static boolean verbose,escape;
public :
    cl_GenStatement();
  static void SetVerbose(boolean v){verbose=v;};
  static void SetOfstream(const char* f);
  static void SetEscape(boolean e){escape=e;};
  static long NbStatements(){return nbStatements;};
  static cl_GenString indentString;
    virtual void Run()=0;
    virtual short IsA() const =0;
    virtual std::ostream& write(std::ostream& os)const;
    virtual std::istream& read(std::istream& is);
    static cl_GenStatement* StreamFrom( std::istream& is);
    void StreamTo(std::ostream& os) const;
    static void AddPointer( const char *name, const cl_GenNode *pointer);
    const char* GetSource()const {return source;};
    long GetLine()const {return sourceLine;};
    void SetSource(const char* sourcePtr,  long lineNb) {
	source=sourcePtr;sourceLine=lineNb;};
    static const char* WriteSourceLine();
    static void SetMainBlock(cl_GenBlock* block);
};

class cl_GenBlock : public cl_GenStatement {
private:
    cl_GenString name;
    cl_GenArray factories;
    cl_GenArray subBlocks;
    cl_GenArray statements;
public:
    cl_GenBlock(const char* name);
    cl_GenBlock();
    ~cl_GenBlock();
    void Add(cl_GenStatement *);
    void AddBlock(cl_GenBlock *subBlock);
    cl_GenBlock* FindBlock(const char *name);
    virtual void Run();
    virtual short IsA() const {return GENBLOCK;};
    virtual std::ostream& write(std::ostream& os)const ;
    virtual std::istream& read(std::istream& is);
    const char* Name(){return name;};
  const cl_GenArray& SubBlocks() const {return subBlocks;};
};

class cl_GenEndBlock : public cl_GenStatement {
private:
public:
    cl_GenEndBlock();
    void Run();
    virtual short IsA() const {return GENENDBLOCK;};
};

class cl_GenEndFor : public cl_GenStatement {
private:
    cl_GenString text;
public:
    cl_GenEndFor(const char* aText);
    cl_GenEndFor();
    void Run();
    virtual short IsA() const {return GENENDFOR;};
    virtual std::ostream& write(std::ostream& os)const ;
    virtual std::istream& read(std::istream& is);
};

class cl_GenElse : public cl_GenStatement {
private:
public:
    cl_GenElse();
    void Run();
    virtual short IsA() const {return GENELSE;};
};

class cl_GenEndIf : public cl_GenStatement {
private:
public:
    cl_GenEndIf();
    void Run();
    virtual short IsA() const {return GENENDIF;};
};

class cl_GenText : public cl_GenStatement {
private:
    cl_GenString text;
public:
    cl_GenText(const char* aText);
    cl_GenText();
    void Run();
    virtual short IsA() const {return GENTEXT;};
    virtual std::ostream& write(std::ostream& os)const;
    virtual std::istream& read(std::istream& is);
};

class cl_GenVariable : public cl_GenStatement {
private:
    cl_GenString varName;
    cl_GenString attrName;
public:
    cl_GenVariable(const char* aVar, const char* anAttr);
    cl_GenVariable();
    void Run();
    virtual short IsA() const {return GENVAR;};
    virtual std::ostream& write(std::ostream& os)const;
    virtual std::istream& read(std::istream& is);
};

class cl_GenFor : public cl_GenStatement {
private:
    cl_GenString varName, containerName;
    cl_GenString nodeName;
    cl_GenBlock loop;
    cl_GenEndFor* epilogue;
public:
    cl_GenFor(const char* aVar, const char* aContainer,  const char* aName);
    cl_GenFor();
    ~cl_GenFor();
    void AddLoop(cl_GenStatement* stmt);
    void AddEpilogue(cl_GenEndFor* stmt);
    void Run();
    virtual short IsA() const {return GENFOR;};
    virtual std::ostream& write(std::ostream& os)const;
    virtual std::istream& read(std::istream& is);
};

class cl_GenBoolExpr : public cl_GenStatement {
public:
    virtual short Value()=0;
    void Run(){};
    virtual short IsA() const {return GENBOOLEXPR;};
    virtual std::ostream& write(std::ostream& os)const=0;
    virtual std::istream& read(std::istream& is)=0;
};

class cl_GenBoolVariable : public cl_GenBoolExpr{
private:
    cl_GenString varName,  conditionName;
public:
    cl_GenBoolVariable(const char* aVar, const char* aCondition);
    cl_GenBoolVariable();
    virtual short Value();
    virtual short IsA() const {return GENBOOLVAR;};
    virtual std::ostream& write(std::ostream& os)const;
    virtual std::istream& read(std::istream& is);
};

class cl_GenBoolVarEqualValue : public cl_GenBoolExpr{
private:
    cl_GenString varName, attrName,  value;
public:
    cl_GenBoolVarEqualValue(const char* aVar, const char* anAttr, const char* aValue);
    cl_GenBoolVarEqualValue();
    virtual short Value();
    virtual short IsA() const {return GENVAREQVAL;};
    virtual std::ostream& write(std::ostream& os)const;
    virtual std::istream& read(std::istream& is);
};

class cl_GenBoolVarEqualVar : public cl_GenBoolExpr{
private:
    cl_GenString leftVarName, leftAttrName,  rightVarName, rightAttrName;
public:
    cl_GenBoolVarEqualVar(const char* lVar, const char* lAttr, const char* rVar, const char* rAttr);
    cl_GenBoolVarEqualVar();
    virtual short Value();
    virtual short IsA() const {return GENVAREQVAR;};
    virtual std::ostream& write(std::ostream& os)const;
    virtual std::istream& read(std::istream& is);
};

class cl_GenIf : public cl_GenStatement {
private:
    cl_GenBoolExpr* condition;
    cl_GenBlock thenBlock;
    cl_GenBlock elseBlock;

public:
    cl_GenIf();
    ~cl_GenIf();
    void AddCondition(cl_GenBoolExpr *condition);
    void AddThen(cl_GenStatement* stmt);
    void AddElse(cl_GenStatement* stmt);
    void Run();
    virtual short IsA() const {return GENIF;};
    virtual std::ostream& write(std::ostream& os)const;
    virtual std::istream& read(std::istream& is);
};

class cl_GenCR : public cl_GenStatement {
private:
public:
    cl_GenCR();
    void Run();
    virtual short IsA() const {return GENCR;};
};

class cl_GenIndent : public cl_GenStatement {
private:

public:
    cl_GenIndent();
    void Run();
    virtual short IsA() const {return GENINDENT;};
};

class cl_GenUnIndent : public cl_GenStatement {
private:

public:
    cl_GenUnIndent();
    void Run();
    virtual short IsA() const {return GENUNINDENT;};
};

class cl_GenThatsAll : public cl_GenStatement {
private:

public:
    cl_GenThatsAll();
    void Run();
    virtual short IsA() const {return GENTHATSALL;};
};

class cl_GenEmpty : public cl_GenStatement {
private:

public:
    cl_GenEmpty();
    void Run();
    virtual short IsA() const {return GENEMPTY;};
};

class cl_GenSet : public cl_GenStatement {
protected:
    cl_GenString fromNode, fromAttr;
    cl_GenString toNode, toAttr;

public:
    cl_GenSet(const char* fNode, const char* fAttr,
		const char* tNode,  const char* tAttr);
    cl_GenSet();
    ~cl_GenSet();
    void Run();
    virtual short IsA() const {return GENSET;};
    virtual std::ostream& write(std::ostream& os)const;
    virtual std::istream& read(std::istream& is);
};
class cl_GenAppend : public cl_GenSet {
public:
    cl_GenAppend() : cl_GenSet(){};
    cl_GenAppend(const char* fNode, const char* fAttr,
		const char* tNode,  const char* tAttr);
    virtual short IsA() const {return GENAPPEND;};
    void Run();
};

class cl_GenSetString : public cl_GenStatement {
protected:
    cl_GenString fromValue;
    cl_GenString toNode, toAttr;

public:
    cl_GenSetString(const char* fValue,
		const char* tNode,  const char* tAttr);
    cl_GenSetString();
    ~cl_GenSetString();
    void Run();
    virtual short IsA() const {return GENSETSTRING;};
    virtual std::ostream& write(std::ostream& os)const;
    virtual std::istream& read(std::istream& is);
};

class cl_GenAppendString : public cl_GenSetString {
public:
    cl_GenAppendString() : cl_GenSetString(){};
    cl_GenAppendString(const char* fValue,
		const char* tNode,  const char* tAttr);
    virtual short IsA() const {return GENAPPENDSTRING;};
    void Run();
};

class cl_GenSubstr : public cl_GenSet {
private:
    short start, length; // for substr() on string values

public:
    cl_GenSubstr(const char* fNode, const char* fAttr,
		const char* tNode,  const char* tAttr,
		short aStart, short aLength);
    cl_GenSubstr();
    ~cl_GenSubstr();
    void Run();
    virtual short IsA() const {return GENSUBSTR;};
    virtual std::ostream& write(std::ostream& os)const;
    virtual std::istream& read(std::istream& is);
};

class cl_GenTAB : public cl_GenStatement {
private:
public:
    cl_GenTAB();
    void Run();
    virtual short IsA() const {return GENTAB;};
};

class cl_GenDo : public cl_GenStatement {
private:
    cl_GenString label;
    cl_GenBlock* scope;
public:
    cl_GenDo(cl_GenBlock* aScope, const char* name);
    cl_GenDo(cl_GenBlock* aScope);
    void Run();
    virtual short IsA() const {return GENDO;};
    virtual std::ostream& write(std::ostream& os)const;
    virtual std::istream& read(std::istream& is);
};

class cl_GenClass : public cl_GenStatement {
private:
    cl_GenString className;
    cl_GenBlock declareBlock;
    cl_GenGlossary methods;
public:
    cl_GenClass(const char* aName);
    cl_GenClass();
    void AddMember(cl_GenStatement* stmt);
    void AddMethod(cl_GenBlock* method);
    cl_GenBlock* FindMethod(const char *name);
    void Run();
    virtual short IsA() const {return GENCLASS;};
    virtual std::ostream& write(std::ostream& os)const ;
    virtual std::istream& read(std::istream& is);
    const char* Name(){return className;};
};
class cl_GenEndClass : public cl_GenStatement {
public:
    cl_GenEndClass();
    void Run();
    virtual short IsA() const {return GENENDCLASS;};
};

class cl_GenNew : public cl_GenStatement {
private:
    cl_GenString className,objectName;
public:
    cl_GenNew(const char* aClass, const char* aName);
    cl_GenNew();
    void Run();
    virtual short IsA() const {return GENNEW;};
    virtual std::ostream& write(std::ostream& os)const ;
    virtual std::istream& read(std::istream& is);
};

class cl_GenAdd : public cl_GenStatement {
protected:
    cl_GenString toNode, toContainer;
    cl_GenString nodeToAdd;

public:
    cl_GenAdd(const char* tNode, const char* tContainer,
		const char* node);
    cl_GenAdd();
    ~cl_GenAdd();
    void Run();
    virtual short IsA() const {return GENADD;};
    virtual std::ostream& write(std::ostream& os)const;
    virtual std::istream& read(std::istream& is);
};

class cl_GenRemove : public cl_GenStatement {
protected:
    cl_GenString toNode, toContainer;
    cl_GenString nodeToAdd;

public:
    cl_GenRemove(const char* tNode, const char* tContainer,
		const char* node);
    cl_GenRemove();
    ~cl_GenRemove();
    void Run();
    virtual short IsA() const {return GENREMOVE;};
    virtual std::ostream& write(std::ostream& os)const;
    virtual std::istream& read(std::istream& is);
};

class cl_GenClean : public cl_GenStatement {
protected:
    cl_GenString toNode, toContainer;

public:
    cl_GenClean(const char* tNode, const char* tContainer);
    cl_GenClean();
    ~cl_GenClean();
    void Run();
    virtual short IsA() const {return GENCLEAN;};
    virtual std::ostream& write(std::ostream& os)const;
    virtual std::istream& read(std::istream& is);
};
class cl_GenOnDo : public cl_GenStatement {
protected:
    cl_GenString node, attr, method;

public:
    cl_GenOnDo(const char* aNode, const char* anAttr,const char* aMethod);
    cl_GenOnDo();
    ~cl_GenOnDo();
    void Run();
    virtual short IsA() const {return GENONDO;};
    virtual std::ostream& write(std::ostream& os)const;
    virtual std::istream& read(std::istream& is);
};

class cl_GenParseNode : public cl_GenStatement {
private:
    cl_GenString fromNode,fromAttr,toNode;
public:
    cl_GenParseNode(const char* aFrom,const char* aFromAttr, const char* aTo);
    cl_GenParseNode();
    ~cl_GenParseNode();
    void Run();
    virtual short IsA() const {return GENPARSENODE;};
    virtual std::ostream& write(std::ostream& os)const ;
    virtual std::istream& read(std::istream& is);
};
class cl_GenInsert : public cl_GenStatement {
private:
    cl_GenString file;
public:
    cl_GenInsert(const char* aFile);
    cl_GenInsert();
    ~cl_GenInsert();
    void Run();
    virtual short IsA() const {return GENINSERT;};
    virtual std::ostream& write(std::ostream& os)const ;
    virtual std::istream& read(std::istream& is);
};
class cl_GenAsctime : public cl_GenStatement {
private:
public:
    cl_GenAsctime();
    void Run();
    virtual short IsA() const {return GENASCTIME;};
};
class cl_GenParseMeta : public cl_GenStatement {
private:
    cl_GenString file;
public:
    cl_GenParseMeta(const char* aFile);
    cl_GenParseMeta();
    ~cl_GenParseMeta();
    void Run();
    virtual short IsA() const {return GENPARSEMETA;};
    virtual std::ostream& write(std::ostream& os)const ;
    virtual std::istream& read(std::istream& is);
};

class cl_GenEnv : public cl_GenStatement {
private:
    cl_GenString variable;
public:
    cl_GenEnv(const char* aVariable);
    cl_GenEnv();
    void Run();
    virtual short IsA() const {return GENENV;};
    virtual std::ostream& write(std::ostream& os)const;
    virtual std::istream& read(std::istream& is);
};

class cl_GenGetEnv : public cl_GenStatement {
protected:
    cl_GenString fromVariable;
    cl_GenString toNode, toAttr;

public:
    cl_GenGetEnv(const char* fVaraible,
		const char* tNode,  const char* tAttr);
    cl_GenGetEnv();
    ~cl_GenGetEnv();
    void Run();
    virtual short IsA() const {return GENGETENV;};
    virtual std::ostream& write(std::ostream& os)const;
    virtual std::istream& read(std::istream& is);
};
class cl_GenInsertVar : public cl_GenStatement {
private:
    cl_GenString fromNode,fromAttr;
public:
    cl_GenInsertVar(const char* aFrom,const char* aFromAttr);
    cl_GenInsertVar();
    ~cl_GenInsertVar();
    void Run();
    virtual short IsA() const {return GENINSERTVAR;};
    virtual std::ostream& write(std::ostream& os)const ;
    virtual std::istream& read(std::istream& is);
};
class cl_GenParseMetaVar : public cl_GenStatement {
private:
    cl_GenString fromNode,fromAttr;
public:
    cl_GenParseMetaVar(const char* aFrom,const char* aFromAttr);
    cl_GenParseMetaVar();
    ~cl_GenParseMetaVar();
    void Run();
    virtual short IsA() const {return GENPARSEMETAVAR;};
    virtual std::ostream& write(std::ostream& os)const ;
    virtual std::istream& read(std::istream& is);
};
class cl_GenOfstream : public cl_GenStatement {
private:
    cl_GenString fromNode,fromAttr;
public:
    cl_GenOfstream(const char* aFrom,const char* aFromAttr);
    cl_GenOfstream();
    ~cl_GenOfstream();
    void Run();
    virtual short IsA() const {return GENOFSTREAM;};
    virtual std::ostream& write(std::ostream& os)const ;
    virtual std::istream& read(std::istream& is);
};
#endif // _GENLANG_H_
