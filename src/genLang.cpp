//
//  $Id: genLang.cc,v 1.0 1996/05/06 06:41:35 andre Exp $
//
//  Copyright (C) 1995 by Sony Telecom Europe (STE)
//  All rights reserved.
//
//  This software or any part of it, may not be used in any form
//  without permission.
//

#include <stdlib.h>
#include <string.h>
#include <genLang.h>
#include <genNode.h>
#include <metaLang.h>
#include <time.h>


cl_GenNode cl_GenStatement::environment(NULL,"");
cl_GenString cl_GenStatement::indentString;
const char* cl_GenStatement::atSource=0;
long cl_GenStatement::atLine=0;
long cl_GenStatement::nbStatements=0;
cl_GenBlock* cl_GenStatement::currBlock=NULL;
boolean cl_GenStatement::verbose(FALSE);
boolean cl_GenStatement::escape(FALSE);
std::ofstream cl_GenStatement::defOs;
cl_GenString cl_GenStatement::defDir("");

// cl_GenStatement Class
// -------------------

cl_GenStatement::cl_GenStatement()
    : source(NULL),  sourceLine(0) {
};

void cl_GenStatement::SetOfstream(const char* file){
  const char* slash= strrchr(file,'/');
  if (slash) { // save default output directory
    defDir=file;
    defDir.Truncate(slash+1-file);
    std::cout << "Generating..." << "\"" << file << "\"" << ",default directory=" << defDir << std::endl;
  };
  defOs.open(file);
  if (!defOs.good())
    cl_GenNode::FatalGenericError("Cannot open ofstream",file);
};

void cl_GenStatement::AddPointer(const char* name, const cl_GenNode* pointer){
// add env pointer to env !!! should be done only once !!!
  environment.AddStringValue("name", "env");
  environment.BuildScopedName();
  environment.AddStringValue("scopedName", environment.ScopedName());
  environment.AddCondition("TRUE", -1);
  environment.AddCondition("FALSE", 0);
  environment.AddStringValue("NULLSTRING", "");
  environment.AddStringValue("isA", "<environment>");
  environment.AddPointer("env", &environment);
  environment.AddPointer(name, pointer);
};
void cl_GenStatement::SetMainBlock(cl_GenBlock* block){
	cl_GenNode* blockNode =new cl_GenNode(&environment,"<block>");
	blockNode->AddVoidPointer("code",(void*)block);
	environment.AddPointer(block->Name(),blockNode);
	currBlock=block;};
const char*cl_GenStatement::WriteSourceLine(){
/*
    if (atSource) {
	char *at = (char *)atSource;
	std::cout << "Walk at line " << atLine << ":";
	do {std::cout << *at;at++;
	}while (*at !='\n');
	std::cout << std::endl;
    };
*/
    return atSource;
};
std::ostream& cl_GenStatement::write (std::ostream& os) const {
    return os;
};
std::istream& cl_GenStatement::read  (std::istream& is){
    return is;
};

cl_GenStatement* cl_GenStatement::StreamFrom(std::istream& is){
    short type=0;
    cl_GenStatement* aStatement;
    type= is.get();

    switch (type){
    case(GENBLOCK) : aStatement = new cl_GenBlock(); break;
    case(GENTEXT) : aStatement = new cl_GenText(); break;
    case(GENVAR) : aStatement = new cl_GenVariable(); break;
    case(GENFOR) : aStatement = new cl_GenFor(); break;
    case(GENENDFOR) : aStatement = new cl_GenEndFor(); break;
    case(GENIF) : aStatement = new cl_GenIf(); break;
    case(GENCR) : aStatement = new cl_GenCR(); break;
    case(GENINDENT) : aStatement = new cl_GenIndent(); break;
    case(GENUNINDENT) : aStatement = new cl_GenUnIndent(); break;
    case(GENSET) : aStatement = new cl_GenSet(); break;
    case(GENSETSTRING) : aStatement = new cl_GenSetString(); break;
    case(GENSUBSTR) : aStatement = new cl_GenSubstr(); break;
    case(GENBOOLVAR) : aStatement = new cl_GenBoolVariable(); break;
    case(GENVAREQVAL) : aStatement = new cl_GenBoolVarEqualValue(); break;
    case(GENVAREQVAR) : aStatement = new cl_GenBoolVarEqualVar(); break;
    case(GENTAB) : aStatement = new cl_GenTAB(); break;
    case(GENDO) : aStatement = new cl_GenDo(currBlock); break;
    case(GENCLASS) : aStatement = new cl_GenClass();break;
    case(GENENDCLASS) : aStatement = new cl_GenEndClass();break;
    case(GENNEW) : aStatement = new cl_GenNew();break;
    case(GENADD) : aStatement = new cl_GenAdd();break;
    case(GENREMOVE) : aStatement = new cl_GenRemove();break;
    case(GENCLEAN) : aStatement = new cl_GenClean();break;
    case(GENONDO) : aStatement = new cl_GenOnDo(); break;
    case(GENPARSENODE) : aStatement = new cl_GenParseNode() ; break;
    case(GENINSERT) : aStatement = new cl_GenInsert() ; break;
    case(GENASCTIME) : aStatement = new cl_GenAsctime(); break;
    case(GENPARSEMETA) : aStatement = new cl_GenParseMeta(); break;
    case(GENAPPEND) : aStatement = new cl_GenAppend(); break;
    case(GENAPPENDSTRING) : aStatement = new cl_GenAppendString(); break;
    case(GENENV) : aStatement = new cl_GenEnv(); break;
    case(GENGETENV) : aStatement = new cl_GenGetEnv(); break;
    case(GENINSERTVAR) : aStatement = new cl_GenInsertVar(); break;
    case(GENPARSEMETAVAR) : aStatement = new cl_GenParseMetaVar(); break;
    case(GENOFSTREAM) : aStatement = new cl_GenOfstream(); break;

    default :
	PRINT ( "cl_GenStatement::StreamFrom unexpected header: type=" <<
		type << " ,at=" << is.gcount() );
	exit (-1);
    };
    if (aStatement) {
      aStatement->sourceLine = cl_GenString::GetShort(is);
      PRINT(" cl_GenStatement::StreamFrom reading a " << type << ",at line" << aStatement->sourceLine << "in source.");
      aStatement->read(is);
      if (type != is.get()){
	PRINT ( "cl_GenStatement::StreamFrom unexpected trailer.");
	exit (-1);
      };
    };
    return aStatement;

};

void cl_GenStatement::StreamTo(std::ostream& os) const{
  os.put((unsigned char)IsA());
  cl_GenString::PutShort(os,sourceLine);
  write(os);
  os.put((unsigned char)IsA());

};

// cl_GenBlock Class
// -----------------
cl_GenBlock::cl_GenBlock(const char* aName)
    : name(aName){
};
cl_GenBlock::cl_GenBlock(){
};
cl_GenBlock::~cl_GenBlock(){};
void cl_GenBlock::Add(cl_GenStatement* aStatement){
    statements.Add(aStatement);
};

void cl_GenBlock::AddBlock (cl_GenBlock* subBlock){
    PRINT("Adding sub-block " << subBlock->Name() << " to " << Name());
    subBlocks.Add((void*)subBlock);
};
cl_GenBlock* cl_GenBlock::FindBlock(const char* name){
    cl_GenBlock* subBlock=0;
    cl_GenIterator iter(subBlocks);
    while (subBlock=((cl_GenBlock*)iter.Next())){
	if (strcmp(subBlock->Name(), name)==0)
	    return subBlock;
    }
    PRINT( "Cannot find sub-block " << name << " in " << Name());
    return 0;
};

std::ostream& cl_GenBlock::write (std::ostream& os) const {
    name.write(os);
    // save sub-blocks
    cl_GenIterator iterBlocks(subBlocks);
    cl_GenString::PutShort(os, subBlocks.NbItems());
    while (iterBlocks.Next()){
	((cl_GenBlock*)iterBlocks.Current())->StreamTo(os);
    };
    /*
    // save factories
    cl_GenIterator iterFact(factories);
    cl_GenString::PutShort(os,factories.NbItems());
    while (iterFact.Next()){
	((cl_GenFactory*)iterFact.Current())->StreamTo(os);
    };
    */

    // save block statements
    cl_GenIterator iter(statements);
    cl_GenString::PutShort(os, statements.NbItems());
//    std::cout <<  "Writing" << (const char*)name << " block with " << nbItems << " statements" << std::endl;
    while (iter.Next()){
	((cl_GenStatement*)iter.Current())->StreamTo(os);
    };
    return os;
};
std::istream& cl_GenBlock::read  (std::istream& is){
    if (!cl_GenStatement::currBlock)
    	cl_GenStatement::currBlock= this;
    name.read(is);
    PRINT ("cl_GenBlock::read" << name);
    // load sub-blocks
    ulong nbBlocks;
    cl_GenBlock* aBlock;
    nbBlocks = cl_GenString::GetShort(is);
//    subBlocks.Resize(nbBlocks);
    for (ulong i=0;i< nbBlocks;i++){
	aBlock = ((cl_GenBlock *)cl_GenStatement::StreamFrom(is));
	if (aBlock) AddBlock(aBlock);
    };

    /*
    // load factories
    ulong nbFactories;
    cl_GenFactory* aFactory;
    nbFactories = cl_GenString::GetShort(is);
//    factories.Resize(nbBlocks);
    for (i=0;i< nbFactories;i++){
	aFactory = ((cl_GenFactory *)cl_GenStatement::StreamFrom(is));
	if (aFactory) AddFactory(aFactory);
    };
    */
    // load block statements
    ulong nbItems;
    cl_GenStatement* aStatement;
    nbItems = cl_GenString::GetShort(is);
//    std::cout <<  "Reading" << (const char*)name << " block with " << nbItems << " statements" << std::endl;
//    statements.Resize(nbItems);
    for (ulong i=0;i< nbItems;i++){
	aStatement = cl_GenStatement::StreamFrom(is);
	if (aStatement) statements.Add(aStatement);
    };
    return is;
};
void cl_GenBlock::Run(){
    cl_GenIterator iter(statements);
    while (iter.Next()){
        atSource = ((cl_GenStatement *)iter.Current())->GetSource();
        atLine = ((cl_GenStatement *)iter.Current())->GetLine();
        //	WriteSourceLine();
	    nbStatements++;
        ((cl_GenStatement *)iter.Current())->Run();
    };
};

// cl_GenEndBlock Class
// ----------------
cl_GenEndBlock::cl_GenEndBlock(){
};
void cl_GenEndBlock::Run(){

};

// cl_GenEndFor Class
// ----------------
cl_GenEndFor::cl_GenEndFor(const char *aText)
    : text(aText){
};
cl_GenEndFor::cl_GenEndFor(){
};

std::ostream& cl_GenEndFor::write (std::ostream& os) const {
    text.write(os);
    return os;
};
std::istream& cl_GenEndFor::read  (std::istream& is){
    text.read(is);
    return is;
};

void cl_GenEndFor::Run(){
    defOs << text;
};

// cl_GenElse Class
// ----------------
cl_GenElse::cl_GenElse(){
};

void cl_GenElse::Run(){
};

// cl_GenEndIf Class
// ----------------
cl_GenEndIf::cl_GenEndIf(){
};

void cl_GenEndIf::Run(){
};


// cl_GenText Class
// ----------------
cl_GenText::cl_GenText(const char *aText)
    : text(aText){
};
cl_GenText::cl_GenText(){
};

std::ostream& cl_GenText::write (std::ostream& os) const {
    text.write(os);
    return os;
};
std::istream& cl_GenText::read  (std::istream& is){
    text.read(is);
    return is;
};

void cl_GenText::Run(){
    defOs << text;
};

// cl_GenVariable Class
// --------------------
cl_GenVariable::cl_GenVariable(const char* aVar, const char* anAttr)
    : varName(aVar),  attrName(anAttr){
};
cl_GenVariable::cl_GenVariable(){
};
std::ostream& cl_GenVariable::write (std::ostream& os) const {
    varName.write(os);
    attrName.write(os);
    return os;
};
std::istream& cl_GenVariable::read  (std::istream& is){
    varName.read(is);
    attrName.read(is);
    return is;
};

void cl_GenVariable::Run(){
    cl_GenNode* n= environment.GetPointer(varName);
    if (n){
      char* stringValue = ((char*)n->GetValue(attrName));
      if (!escape || (strchr(stringValue,'"') == NULL)){// stream it to os 'as is'
	defOs << stringValue;
      }
      else {//stringValue contains '"' character(s)
	while (*stringValue) {
	  if (*stringValue=='"')// stream it with backslash
	    defOs << "\\\"";
	  else
	    defOs << *stringValue;
	  stringValue++;
	};
      };
    }
    else{
	std::cout << "in $VAR: cannot find node "<< varName << std::endl;
    };
};

// cl_GenFor Class
// ---------------
cl_GenFor::cl_GenFor(const char* aVar, const char* aContainer,  const char* aName)
    : loop("~f"), varName(aVar), containerName(aContainer),  nodeName(aName),  epilogue(0){
};
cl_GenFor::cl_GenFor(){
};

cl_GenFor::~cl_GenFor(){
    if (epilogue) delete (epilogue);
};

void cl_GenFor::AddLoop(cl_GenStatement* aStmt){
    loop.Add(aStmt);
}

void cl_GenFor::AddEpilogue(cl_GenEndFor* aStmt){
    epilogue = aStmt;
}

std::ostream& cl_GenFor::write (std::ostream& os) const {
    varName.write(os);
    containerName.write(os);
    nodeName.write(os);
    loop.write(os);
    if (epilogue)
	epilogue->StreamTo(os);
    return os;
};
std::istream& cl_GenFor::read  (std::istream& is){
    varName.read(is);
    containerName.read(is);
    nodeName.read(is);
    loop.read(is);
    epilogue = ((cl_GenEndFor*)cl_GenStatement::StreamFrom(is));
    return is;
};

void cl_GenFor::Run(){
    cl_GenNode *n = environment.GetPointer(varName);
    if (n){
	cl_GenIterator* iter= n->GetIterator(containerName);
	if (iter){
	    while (iter->Next()) {
		environment.AddPointer(nodeName, ((const cl_GenNode *)iter->Current()));
		loop.Run();
		if (!iter->IsLast())
			epilogue->Run();
	    }
//		environment.AddPointer(nodeName, NULL);
	    environment.RemovePointer(nodeName);
	    delete(iter);
	}
    }
}

// cl_GenBoolVariable Class
// ------------------
cl_GenBoolVariable::cl_GenBoolVariable(const char* aVar, const char* aCondition)
    : varName(aVar),conditionName(aCondition){
};
cl_GenBoolVariable::cl_GenBoolVariable(){
};

std::ostream& cl_GenBoolVariable::write (std::ostream& os) const {
    varName.write(os);
    conditionName.write(os);
    return os;
};
std::istream& cl_GenBoolVariable::read  (std::istream& is){
    varName.read(is);
    conditionName.read(is);
    return is;
};

short cl_GenBoolVariable::Value(){
    cl_GenNode *n = environment.GetPointer(varName);
    if (n && (n->HasAttribute(conditionName)==cl_GenNode::BOOLEAN)) {
	return (n->GetCondition(conditionName));
    };
    if (n==0)
	cl_GenNode::FatalStaticError(varName);
    else
	n->FatalError(conditionName);
    return 0;
};

// cl_GenBoolVarEqualValue Class
// ------------------
cl_GenBoolVarEqualValue::cl_GenBoolVarEqualValue(const char* aVar, const char* anAttr, const char* aValue)
    : varName(aVar),attrName(anAttr), value(aValue){
};
cl_GenBoolVarEqualValue::cl_GenBoolVarEqualValue(){
};

std::ostream& cl_GenBoolVarEqualValue::write (std::ostream& os) const {
    varName.write(os);
    attrName.write(os);
    value.write(os);
    return os;
};
std::istream& cl_GenBoolVarEqualValue::read  (std::istream& is){
    varName.read(is);
    attrName.read(is);
    value.read(is);
    return is;
};
short cl_GenBoolVarEqualValue::Value(){
    cl_GenNode *n = environment.GetPointer(varName);
    if (n) {
	const char* varString = n->GetValue(attrName);
	if (varString)
		return (strcmp(varString, ((const char *)value)) ==0);
    };
    if (n==0)
	cl_GenNode::FatalStaticError(varName);
    else
	n->FatalError(attrName);
    return 0;
};

// cl_GenBoolVarEqualVar Class
// ------------------
cl_GenBoolVarEqualVar::cl_GenBoolVarEqualVar(const char* lVar, const char* lAttr, const char* rVar, const char* rAttr)
    : leftVarName(lVar),leftAttrName(lAttr), rightVarName(rVar), rightAttrName(rAttr){
};
cl_GenBoolVarEqualVar::cl_GenBoolVarEqualVar(){
};

std::ostream& cl_GenBoolVarEqualVar::write (std::ostream& os) const {
    leftVarName.write(os);
    leftAttrName.write(os);
    rightVarName.write(os);
    rightAttrName.write(os);
    return os;
};
std::istream& cl_GenBoolVarEqualVar::read  (std::istream& is){
    leftVarName.read(is);
    leftAttrName.read(is);
    rightVarName.read(is);
    rightAttrName.read(is);
    return is;
};
short cl_GenBoolVarEqualVar::Value(){
    cl_GenNode *l;
    cl_GenNode *r;
    const char* leftString;
    const char* rightString;

    l = environment.GetPointer(leftVarName);
    if (!l)
	cl_GenNode::FatalStaticError(leftVarName);

    leftString = l->GetValue(leftAttrName);
    if (!leftString)
	l->FatalError(leftAttrName);

    r = environment.GetPointer(rightVarName);
    if (!r)
	cl_GenNode::FatalStaticError(rightVarName);

    rightString = r->GetValue(rightAttrName);
    if (!rightString)
	r->FatalError(rightAttrName);

    return (strcmp(leftString, rightString) ==0);
};

// cl_GenIf Class
// --------------
cl_GenIf::cl_GenIf()
    : thenBlock("~t"),elseBlock("~e"),condition(0){
};

cl_GenIf::~cl_GenIf(){
    if (condition) delete (condition);
};

void cl_GenIf::AddCondition(cl_GenBoolExpr* aCondition){
    condition = aCondition;
};

void cl_GenIf::AddThen(cl_GenStatement* stmt){
    thenBlock.Add(stmt);
};
void cl_GenIf::AddElse(cl_GenStatement* stmt){
    elseBlock.Add(stmt);
};

std::ostream& cl_GenIf::write (std::ostream& os) const {
    condition->StreamTo(os);
    thenBlock.write(os);
    elseBlock.write(os);
    return os;
};
std::istream& cl_GenIf::read  (std::istream& is){
    condition = ((cl_GenBoolExpr*)cl_GenStatement::StreamFrom(is));
    thenBlock.read(is);
    elseBlock.read(is);
    return is;
};
void cl_GenIf::Run(){
    if (condition) {
	if (condition->Value())
      	    thenBlock.Run();
	else
    	    elseBlock.Run();
    };
};


// cl_GenCR Class
// ----------------
cl_GenCR::cl_GenCR(){
};

void cl_GenCR::Run(){
    defOs << std::endl << indentString;
};

// cl_GenIndent Class
// ----------------
cl_GenIndent::cl_GenIndent(){
};

void cl_GenIndent::Run(){
    if (indentString.StringSize()==0)
	indentString.Reserve(MAXINDENT);

    if (indentString.StringSize() < MAXINDENT)
	for (int i=0;i < INDENTSTEP; i++)
	    indentString.Add(' ');
};

// cl_GenUnIndent Class
// ----------------
cl_GenUnIndent::cl_GenUnIndent(){
};

void cl_GenUnIndent::Run(){
    if (indentString.StringSize() > 0)
	for (int i=0;i <INDENTSTEP;i++)
	    indentString.DeleteLast();
};

// cl_GenThatsAll Class
// ----------------
cl_GenThatsAll::cl_GenThatsAll(){
};

void cl_GenThatsAll::Run(){
};

// cl_GenEmpty Class
// ----------------
cl_GenEmpty::cl_GenEmpty(){
};

void cl_GenEmpty::Run(){
};

// cl_GenSet Class
// ---------------
cl_GenSet::cl_GenSet(const char* fNode, const char* fAttr,  const char* tNode, const char* tAttr)
    : fromNode(fNode), fromAttr(fAttr),  toNode(tNode), toAttr(tAttr){
};
cl_GenSet::cl_GenSet(){
};
cl_GenSet::~cl_GenSet(){
};

std::ostream& cl_GenSet::write (std::ostream& os) const {
    fromNode.write(os);
    fromAttr.write(os);
    toNode.write(os);
    toAttr.write(os);
    return os;
};
std::istream& cl_GenSet::read  (std::istream& is){
    fromNode.read(is);
    fromAttr.read(is);
    toNode.read(is);
    toAttr.read(is);
    return is;
};

void cl_GenSet::Run(){
    cl_GenNode *from = environment.GetPointer(fromNode);
    cl_GenNode *to = environment.GetPointer(toNode);
    cl_GenNode* n= NULL;

    if (from && to){
	switch (from->HasAttribute(fromAttr)){
	case (cl_GenNode::BOOLEAN):
	    to->AddCondition(toAttr, from->GetCondition(fromAttr));
	    break;
	case (cl_GenNode::STRING):
	    to->AddStringValue(toAttr, from->GetValue(fromAttr));
	    break;
	case (cl_GenNode::POINTER):
		n= from->GetPointer(fromAttr);
		if (n)
		    to->AddPointer(toAttr, n);
	    break;
	default:
	case (cl_GenNode::NONE):
	    from->FatalError(fromAttr);
	    break;
	};
    }
    else{
	if (from==0)
	    cl_GenNode::FatalStaticError(fromNode);
	else
	    cl_GenNode::FatalStaticError(toNode);
    };
};
// cl_GenAppend Class
//-------------
cl_GenAppend::cl_GenAppend(const char* fNode, const char* fAttr,  const char* tNode, const char* tAttr)
    : cl_GenSet(fNode, fAttr, tNode,tAttr){
};
void cl_GenAppend::Run(){
  cl_GenNode *from = environment.GetPointer(fromNode);
  cl_GenNode *to = environment.GetPointer(toNode);

  if (from && to){
    if ( (from->HasAttribute(fromAttr)== cl_GenNode::STRING)
	 && (to->HasAttribute(toAttr)== cl_GenNode::STRING)){
      to->AppendStringValue(toAttr, from->GetValue(fromAttr));
    }
    else {
      if (from->HasAttribute(fromAttr)== cl_GenNode::STRING)
	to->FatalError(toAttr);
      else
	from->FatalError(fromAttr);
    }
  }
  else {
    if (from==0)
      cl_GenNode::FatalStaticError(fromNode);
    else
      cl_GenNode::FatalStaticError(toNode);
  };
};

// cl_GenSetString Class
// ---------------
cl_GenSetString::cl_GenSetString(const char* fValue,  const char* tNode, const char* tAttr)
    : fromValue(fValue), toNode(tNode), toAttr(tAttr){
};
cl_GenSetString::cl_GenSetString(){
};

cl_GenSetString::~cl_GenSetString(){
};


std::ostream& cl_GenSetString::write (std::ostream& os) const {
    fromValue.write(os);
    toNode.write(os);
    toAttr.write(os);
    return os;
};
std::istream& cl_GenSetString::read  (std::istream& is){
    fromValue.read(is);
    toNode.read(is);
    toAttr.read(is);
    return is;
};

void cl_GenSetString::Run(){
    cl_GenNode *to = environment.GetPointer(toNode);
    if (to)
	to->AddStringValue(toAttr, fromValue);
    else
	cl_GenNode::FatalStaticError(toNode);
};
// cl_GenAppendString Class
//-------------
cl_GenAppendString::cl_GenAppendString(const char* fValue,  const char* tNode, const char* tAttr)
    : cl_GenSetString(fValue,tNode,tAttr){
};
void cl_GenAppendString::Run(){
    cl_GenNode *to = environment.GetPointer(toNode);
    if (to){
      if (to->HasAttribute(toAttr)==cl_GenNode::STRING){
	to->AppendStringValue(toAttr, fromValue);
      }
      else{
	to->FatalError(toAttr);
      }
    }
    else
      cl_GenNode::FatalStaticError(toNode);
};

// cl_GenSubstr Class
// ---------------
cl_GenSubstr::cl_GenSubstr(const char* fNode, const char* fAttr,  const char* tNode, const char* tAttr,
    short aStart,  short aLength)
    : cl_GenSet(fNode, fAttr, tNode, tAttr),  start(aStart), length(aLength){
};

cl_GenSubstr::cl_GenSubstr(){
};
cl_GenSubstr::~cl_GenSubstr(){
};

std::ostream& cl_GenSubstr::write (std::ostream& os) const {
    cl_GenSet::write(os);
    os.put((unsigned char)(start<0));
    os.put((unsigned char)start);
    os.put((unsigned char)(length<0));
    os.put((unsigned char)length);
    return os;
};
std::istream& cl_GenSubstr::read  (std::istream& is){
  char sign;
  cl_GenSet::read(is);
  sign = is.get();
  start = is.get();
  if (sign) start = start -256;
  sign = is.get();
  length = is.get();
  if (sign) length = length -256;
  return is;
};
void cl_GenSubstr::Run(){
    cl_GenNode *from = environment.GetPointer(fromNode);
    cl_GenNode *to = environment.GetPointer(toNode);
    short actualStart,actualLength,fromLength;
    char *fromValue = NULL;
    char *newValue = NULL;

    if (from && to){
	switch (from->HasAttribute(fromAttr)){
	case (cl_GenNode::STRING):
	  fromValue = (char*)(from->GetValue(fromAttr));
	  fromLength = strlen(fromValue);
	  if (start < 0) { // relative to endOfString
	    actualStart= fromLength + start;
	  }
	  else {
	    actualStart = start;
	  };
	  if (length < 0) { // relative to endOfString
	    actualLength= fromLength - actualStart + length;
	  }
	  else {
	    actualLength = length;
	  };
	  newValue = new char[actualLength+1];
	  if (fromValue && newValue){
	    memset(newValue, '\0', actualLength+1);
	    if (fromLength > actualStart){ // something to copy
	      if (fromLength > actualStart + actualLength){ // copy actualLength bytes
		strncpy(newValue, fromValue + actualStart, actualLength);
	      }
	      else{ // copy remaining bytes only
		strcpy(newValue, fromValue + actualStart);
	      };

	    };
	    to->AddStringValue(toAttr, newValue);
	    delete newValue;
	  };
	  break;
	default: // not a string attribute
	  from->FatalError(fromAttr);
	  break;
	};
    }
    else{
      if (from==0)
	cl_GenNode::FatalStaticError(fromNode);
      else
	cl_GenNode::FatalStaticError(toNode);
    };
};

// cl_GenTAB Class
// ----------------
cl_GenTAB::cl_GenTAB(){
};

void cl_GenTAB::Run(){
    defOs << '\t';
};

// cl_GenDo Class
// ----------------
cl_GenDo::cl_GenDo(cl_GenBlock* aScope, const char* name)
    : scope(aScope),  label(name){
};
cl_GenDo::cl_GenDo(cl_GenBlock* aScope)
    : scope(aScope){
};

std::ostream& cl_GenDo::write (std::ostream& os) const {
    label.write(os);
    return os;
};
std::istream& cl_GenDo::read  (std::istream& is){
    label.read(is);
    return is;
};

void cl_GenDo::Run(){
    if (scope  && (scope->IsA()== GENBLOCK)){
	cl_GenBlock* procedure=scope->FindBlock(label);
	if (procedure)
	    procedure->Run();
	// ************ remove to allow optional scripts *****
	//	else
	//  cl_GenNode::FatalGenericError("Cannot find script",label);
    };
};
// cl_GenClass Class
// ----------------
cl_GenClass::cl_GenClass(const char *aName)
    : className(aName), declareBlock("~c"){
};
cl_GenClass::cl_GenClass(){
};

void cl_GenClass::AddMember(cl_GenStatement* aStmt){
    declareBlock.Add(aStmt);
}

void cl_GenClass::AddMethod (cl_GenBlock* subBlock){
    PRINT("Adding method " << subBlock->Name() << " to class " << className);
    methods.Add(subBlock->Name(),(void*)subBlock);
};
cl_GenBlock* cl_GenClass::FindMethod(const char* name){
  cl_GenBlock* subBlock=0;
  subBlock = (cl_GenBlock*)methods.FindObject(name);
  if(!subBlock)
    std::cout << "Cannot find method " << name << " in class " << className << std::endl;
  return subBlock;
};

std::ostream& cl_GenClass::write (std::ostream& os) const {
  cl_GenGlossaryEntry* currEntry;
    className.write(os);

    declareBlock.write(os);
    // save methods
    cl_GenIterator iterBlocks(methods.Repository());
    cl_GenString::PutShort(os,methods.Repository().NbItems());
    while (iterBlocks.Next()){
      currEntry = ((cl_GenGlossaryEntry*)iterBlocks.Current());
      ((cl_GenBlock*)currEntry->object)->StreamTo(os);
    };
    return os;
};
std::istream& cl_GenClass::read  (std::istream& is){
    className.read(is);
    declareBlock.read(is);
    // load methods
    ulong nbBlocks;
    cl_GenBlock* aBlock;
    nbBlocks = cl_GenString::GetShort(is);
//    methods.Resize(nbBlocks);
    for (ulong i=0;i< nbBlocks;i++){
	aBlock = ((cl_GenBlock *)cl_GenStatement::StreamFrom(is));
	if (aBlock) AddMethod(aBlock);
    };


    return is;
};

void cl_GenClass::Run(){
  PRINT("Defining a WALK class " <<  ":\"" << className <<  "\"");
  cl_GenNodeClass*newClass =cl_GenNode::DefineClass(className,methods);

  if (newClass && currBlock) {// Add scripts with name = <class>.<script> as method in this class
    cl_GenIterator iterBlocks(currBlock->SubBlocks());
    cl_GenBlock* mBlock=NULL;
    const char* sName=NULL;
    const char* mName=NULL;
    while (mBlock=(cl_GenBlock*)iterBlocks.Next()){
      sName = (const char*)(mBlock->Name());
      mName = strchr(sName,'.');
      if (mName && (className.StringSize() == mName-sName) && (strncmp(className,sName,mName-sName)==0)){
	mName++;
	PRINT("Adding script " << sName << " as method "  << mName << " to class " << className);
	newClass->AddMethod(mName,mBlock);
      };
    };
  };
  declareBlock.Run();
};
// cl_GenEndClass Class
// ----------------
cl_GenEndClass::cl_GenEndClass(){
};

void cl_GenEndClass::Run(){
    cl_GenNode::EndDefineClass();
};

// cl_GenNew Class
// ----------------
cl_GenNew::cl_GenNew(const char* aClass, const char *aName)
    : className(aClass),objectName(aName){
};
cl_GenNew::cl_GenNew(){
};

std::ostream& cl_GenNew::write (std::ostream& os) const {
    className.write(os);
    objectName.write(os);
    return os;
};
std::istream& cl_GenNew::read  (std::istream& is){
    className.read(is);
    objectName.read(is);
    return is;
};

void cl_GenNew::Run(){
    const cl_GenNode* me =NULL;
    if (environment.HasAttribute("me")== cl_GenNode::POINTER)
      me= environment.GetPointer("me");
    const cl_GenNode* newNode= cl_GenNode::NewObject(me, className,objectName);
    AddPointer(objectName, newNode);
};

// cl_GenAdd Class
// ---------------
cl_GenAdd::cl_GenAdd(const char* tNode, const char* tContainer,  const char* node)
    : toNode(tNode), toContainer(tContainer),  nodeToAdd(node){
};
cl_GenAdd::cl_GenAdd(){
};
cl_GenAdd::~cl_GenAdd(){
};

std::ostream& cl_GenAdd::write (std::ostream& os) const {
    toNode.write(os);
    toContainer.write(os);
    nodeToAdd.write(os);
    return os;
};
std::istream& cl_GenAdd::read  (std::istream& is){
    toNode.read(is);
    toContainer.read(is);
    nodeToAdd.read(is);
    return is;
};

void cl_GenAdd::Run(){
  cl_GenNode *to = environment.GetPointer(toNode);
  cl_GenNode *node = environment.GetPointer(nodeToAdd);

  if (node && to){
   to->AddUniqueInContainer(toContainer, node);
  }
  else{
    if (to==0)
      cl_GenNode::FatalStaticError(toNode);
    else
      cl_GenNode::FatalStaticError(nodeToAdd);
  };
};
// cl_GenRemove Class
// ---------------
cl_GenRemove::cl_GenRemove(const char* tNode, const char* tContainer,  const char* node)
    : toNode(tNode), toContainer(tContainer),  nodeToAdd(node){
};
cl_GenRemove::cl_GenRemove(){
};
cl_GenRemove::~cl_GenRemove(){
};

std::ostream& cl_GenRemove::write (std::ostream& os) const {
    toNode.write(os);
    toContainer.write(os);
    nodeToAdd.write(os);
    return os;
};
std::istream& cl_GenRemove::read  (std::istream& is){
    toNode.read(is);
    toContainer.read(is);
    nodeToAdd.read(is);
    return is;
};

void cl_GenRemove::Run(){
    cl_GenNode *to = environment.GetPointer(toNode);
    cl_GenNode *node = environment.GetPointer(nodeToAdd);

    if (node && to){
	switch (to->HasAttribute(toContainer)){
	case (cl_GenNode::CONTAINER):
	    to->RemoveFromContainer(toContainer, node);
	    break;
	default:
	    to->FatalError(toContainer);
	    break;
	};
    }
    else{
	if (to==0)
	    cl_GenNode::FatalStaticError(toNode);
	else
	    cl_GenNode::FatalStaticError(nodeToAdd);
    };
};
// cl_GenClean Class
// ---------------
cl_GenClean::cl_GenClean(const char* tNode, const char* tContainer)
    : toNode(tNode), toContainer(tContainer){
};
cl_GenClean::cl_GenClean(){
};
cl_GenClean::~cl_GenClean(){
};

std::ostream& cl_GenClean::write (std::ostream& os) const {
    toNode.write(os);
    toContainer.write(os);
    return os;
};
std::istream& cl_GenClean::read  (std::istream& is){
    toNode.read(is);
    toContainer.read(is);
    return is;
};

void cl_GenClean::Run(){
    cl_GenNode *to = environment.GetPointer(toNode);

    if (to){
	switch (to->HasAttribute(toContainer)){
	case (cl_GenNode::CONTAINER):
	    to->ClearContainer(toContainer);
	    break;
	case (cl_GenNode::NONE): // add as a new, empty container
	    to->AddContainer(toContainer);
	    break;
	default:
	    to->FatalError(toContainer);
	    break;
	};
    }
    else
      cl_GenNode::FatalStaticError(toNode);
};
// cl_GenOnDo Class
// ---------------
cl_GenOnDo::cl_GenOnDo(const char* aNode, const char* anAttr,const char* aMethod)
    : node(aNode),attr(anAttr),method(aMethod){
};
cl_GenOnDo::cl_GenOnDo(){
};
cl_GenOnDo::~cl_GenOnDo(){
};

std::ostream& cl_GenOnDo::write (std::ostream& os) const {
    node.write(os);
    attr.write(os);
    method.write(os);
    return os;
};
std::istream& cl_GenOnDo::read  (std::istream& is){
    node.read(is);
    attr.read(is);
    method.read(is);
    return is;
};

void cl_GenOnDo::Run(){
    cl_GenNode *to = environment.GetPointer(node);
    cl_GenNode* ref= NULL;
    cl_GenBlock *methodBlock=NULL;
    if (to) {
      ref= to->GetPointer(attr);
      if (ref)
        methodBlock = (cl_GenBlock*)(ref->GetMethod(method));
      else
	to->FatalError(attr);
    }
    else
      cl_GenNode::FatalStaticError(node);

    if (methodBlock){
        cl_GenNode* oldMe = NULL;
	if (environment.HasAttribute("me")== cl_GenNode::POINTER)
	    oldMe = environment.GetPointer("me");
        environment.AddPointer("me",ref);
	methodBlock->Run();
	if (oldMe)
	  environment.AddPointer("me",oldMe);
	else
	  environment.RemovePointer("me");
    }
    else
      ref->FatalError(method);
};

// cl_GenParseNode Class
// ----------------
cl_GenParseNode::cl_GenParseNode(const char* f, const char* fa,const char *t)
    : fromNode(f),fromAttr(fa),toNode(t){
};
cl_GenParseNode::cl_GenParseNode(){
};

std::ostream& cl_GenParseNode::write (std::ostream& os) const {
    fromNode.write(os);
    fromAttr.write(os);
    toNode.write(os);
    return os;
};
std::istream& cl_GenParseNode::read  (std::istream& is){
    fromNode.read(is);
    fromAttr.read(is);
    toNode.read(is);
    return is;
};

void cl_GenParseNode::Run(){
  const cl_GenNode* newNode=NULL;
  cl_GenNode *from = environment.GetPointer(fromNode);

  if (from){
    if (from->HasAttribute(fromAttr)==cl_GenNode::STRING){
      const char* file = from->GetValue(fromAttr);
      std::ifstream is(file);
      if (is.good()){
	cl_MParserIStream::IFile = file;
	cl_MRule::ParseSource(is);
	cl_MParserIStream::IFile = NULL;

	newNode = (const cl_GenNode*)cl_MRule::GeneratedCode();
	if (newNode){
	  environment.AddPointer(toNode, newNode);
	}
	else
	cl_GenNode::FatalGenericError("Error while parsing file",file);
      }
      else
	cl_GenNode::FatalGenericError("Cannot open node",file);
   }
    else
      from->FatalError(fromAttr);
  }
  else
    cl_GenNode::FatalStaticError(fromNode);

};
// cl_GenInsert Class
// ----------------
cl_GenInsert::cl_GenInsert(const char* f)
    : file(f){
};
cl_GenInsert::cl_GenInsert(){
};

std::ostream& cl_GenInsert::write (std::ostream& os) const {
    file.write(os);
    return os;
};
std::istream& cl_GenInsert::read  (std::istream& is){
    file.read(is);
    return is;
};

void cl_GenInsert::Run(){
  std::ifstream h(file);
  if (h.good()){
    char c=h.get();
    while (!h.eof()){
      defOs.put(c);
      c=h.get();
    };
  }
  else
    cl_GenNode::FatalGenericError("Cannot insert file",file);
};

// cl_GenAsctime Class
// ----------------
cl_GenAsctime::cl_GenAsctime(){
};

void cl_GenAsctime::Run(){
  // copy current date&time to os
  time_t t;
  t=time(NULL);
  cl_GenString date(asctime(localtime(&t)));
  date.DeleteLast();
  defOs << date ;
};
// cl_GenParseMeta Class
// ----------------
cl_GenParseMeta::cl_GenParseMeta(const char* f)
    : file(f){
};
cl_GenParseMeta::cl_GenParseMeta(){
};

std::ostream& cl_GenParseMeta::write (std::ostream& os) const {
    file.write(os);
    return os;
};
std::istream& cl_GenParseMeta::read  (std::istream& is){
    file.read(is);
    return is;
};

void cl_GenParseMeta::Run(){
  std::ifstream is(file);
  if (is.good()){
    cl_MParserIStream::IFile = file;
    cl_MRule* g=cl_MRule::ParseGrammar(is);
    cl_MParserIStream::IFile = NULL;
    if (g==NULL)
      cl_GenNode::FatalGenericError("Error while parsing meta",file);
  }
  else
    cl_GenNode::FatalGenericError("Cannot open meta",file);

};


// cl_GenEnv Class
// ----------------
cl_GenEnv::cl_GenEnv(const char* v)
    : variable(v){
};
cl_GenEnv::cl_GenEnv(){
};

std::ostream& cl_GenEnv::write (std::ostream& os) const {
    variable.write(os);
    return os;
};
std::istream& cl_GenEnv::read  (std::istream& is){
    variable.read(is);
    return is;
};

void cl_GenEnv::Run(){
  char* varValue=getenv(variable);
  if (varValue)
    defOs << varValue;
  else // its probably better to stop here
    cl_GenNode::FatalGenericError("Cannot get environment variable",variable);
};

// cl_GenGetEnv Class
// ---------------
cl_GenGetEnv::cl_GenGetEnv(const char* fVariable,  const char* tNode, const char* tAttr)
    : fromVariable(fVariable), toNode(tNode), toAttr(tAttr){
};
cl_GenGetEnv::cl_GenGetEnv(){
};

cl_GenGetEnv::~cl_GenGetEnv(){
};


std::ostream& cl_GenGetEnv::write (std::ostream& os) const {
    fromVariable.write(os);
    toNode.write(os);
    toAttr.write(os);
    return os;
};
std::istream& cl_GenGetEnv::read  (std::istream& is){
    fromVariable.read(is);
    toNode.read(is);
    toAttr.read(is);
    return is;
};

void cl_GenGetEnv::Run(){
  char* varValue=getenv(fromVariable);
  if (varValue) {
    cl_GenNode *to = environment.GetPointer(toNode);
    if (to)
      to->AddStringValue(toAttr, varValue);
    else
      cl_GenNode::FatalStaticError(toNode);
  }
  else // its probably better to stop here
    cl_GenNode::FatalGenericError("Cannot get environment variable",fromVariable);
};

// cl_GenInsertVar Class
// ----------------
cl_GenInsertVar::cl_GenInsertVar(const char* f, const char* fa)
    : fromNode(f),fromAttr(fa){
};
cl_GenInsertVar::cl_GenInsertVar(){
};

std::ostream& cl_GenInsertVar::write (std::ostream& os) const {
    fromNode.write(os);
    fromAttr.write(os);
    return os;
};
std::istream& cl_GenInsertVar::read  (std::istream& is){
    fromNode.read(is);
    fromAttr.read(is);
    return is;
};

void cl_GenInsertVar::Run(){
  cl_GenNode *from = environment.GetPointer(fromNode);
  if (from){
    if (from->HasAttribute(fromAttr)==cl_GenNode::STRING){
      const char* file = from->GetValue(fromAttr);
      std::ifstream h(file);
      if (h.good()){
	char c=h.get();
	while (!h.eof()){
	  defOs.put(c);
	  c=h.get();
	};
      }
      else
	cl_GenNode::FatalGenericError("Cannot insert file",file);
   }
    else
      from->FatalError(fromAttr);
  }
  else
    cl_GenNode::FatalStaticError(fromNode);
};
// cl_GenParseMetaVar Class
// ----------------
cl_GenParseMetaVar::cl_GenParseMetaVar(const char* f, const char* fa)
    : fromNode(f),fromAttr(fa){
};
cl_GenParseMetaVar::cl_GenParseMetaVar(){
};

std::ostream& cl_GenParseMetaVar::write (std::ostream& os) const {
    fromNode.write(os);
    fromAttr.write(os);
    return os;
};
std::istream& cl_GenParseMetaVar::read  (std::istream& is){
    fromNode.read(is);
    fromAttr.read(is);
    return is;
};

void cl_GenParseMetaVar::Run(){
  cl_GenNode *from = environment.GetPointer(fromNode);
  if (from){
    if (from->HasAttribute(fromAttr)==cl_GenNode::STRING){
      const char* file = from->GetValue(fromAttr);
      std::ifstream is(file);
      if (is.good()){
	cl_MParserIStream::IFile = file;
	cl_MRule* g=cl_MRule::ParseGrammar(is);
	cl_MParserIStream::IFile = NULL;
	if (g==NULL)
	  cl_GenNode::FatalGenericError("Error while parsing meta",file);
      }
      else
	cl_GenNode::FatalGenericError("Cannot open meta",file);
   }
    else
      from->FatalError(fromAttr);
  }
  else
    cl_GenNode::FatalStaticError(fromNode);

};
// cl_GenOfstream Class
// ----------------
cl_GenOfstream::cl_GenOfstream(const char* f, const char* fa)
    : fromNode(f),fromAttr(fa){
};
cl_GenOfstream::cl_GenOfstream(){
};

std::ostream& cl_GenOfstream::write (std::ostream& os) const {
    fromNode.write(os);
    fromAttr.write(os);
    return os;
};
std::istream& cl_GenOfstream::read  (std::istream& is){
    fromNode.read(is);
    fromAttr.read(is);
    return is;
};

void cl_GenOfstream::Run(){
  cl_GenNode *from = environment.GetPointer(fromNode);
  if (from){
    if (from->HasAttribute(fromAttr)==cl_GenNode::STRING){
      const char* file = from->GetValue(fromAttr);
      defOs.close();
      cl_GenString fullPath;
      fullPath = defDir;
      fullPath.Concat(file);
      defOs.open(fullPath,std::ofstream::out | std::ofstream::app);
      if (defOs.good())
	std::cout << "Streaming to...\"" << fullPath << "\"" << std::endl;
      else
	cl_GenNode::FatalGenericError("Cannot open ofstream",fullPath);
   }
    else
      from->FatalError(fromAttr);
  }
  else
    cl_GenNode::FatalStaticError(fromNode);

};
