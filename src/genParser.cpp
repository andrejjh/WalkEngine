//
//  $Id: genParser.cc,v 1.0 1996/05/07 06:41:35 andre Exp $
//

#include <string.h>
#include <stdlib.h>
#include <genParser.h>

// cl_GenParser Class
// ----------------
cl_GenParser::cl_GenParser()
    : main(0),curr(0),currLine(1),verbose(FALSE){
};
cl_GenParser::~cl_GenParser(){
};

void cl_GenParser::Load(std::istream &is){
    // load text from std::istream
    int c;
    curr=0;
    text.Reserve(10240); // reserve 10 k for gdl code
    while((c=is.get()) > 0) {
	      text.Add((char)c);
    };
};
void cl_GenParser::Include(std::istream &is){
    // include from std::istream at current position
    cl_GenString remain(((const char*)text)+curr);
    text.Truncate(curr);
    int c;
    while((c=is.get()) > 0) {
	text.Add((char)c);
    };
    text.Concat(remain);
};
void cl_GenParser::ShowError(const char *error){
    std::cout << "WALK parsing error :" << error << std::endl;
    WriteSourceLine();
    exit(-1);
};

void cl_GenParser::ShowError(const char *error, const char* name){
    std::cout << "WALK parsing error :" << error << ":\"" << name << "\"" << std::endl;
    WriteSourceLine();
    exit(-1);
};

void cl_GenParser::WriteSourceLine(){
    char *at = (char *)(const char*)text;
    char *problem = at + curr;
    char *linePtr=at;
    int lineNb=1;

    //compute line number
    while(at < problem) {
	if (*at=='\n') {
	    lineNb++;
	    linePtr=at;
	};
	at++;
    };
    std::cout << "source at line " << currLine << " is :" << std::endl;
    // Output error/warning source line
    at=linePtr+ 1;
    do {std::cout << *at;at++;
    }while (*at !='\n');
    // Show where the problem occurs
    std::cout << std::endl;
    at=linePtr+ 1;
    do {std::cout << " ";at++;
    }while (at !=problem);
    std::cout << "^" << std::endl;
};

void cl_GenParser::VarName(cl_GenString& leftPart, cl_GenString& rightPart) {
    // receive input in leftPart
    // if input format is name1.name2,
    //	    this method returns name1 in leftPart & name2 in rightPart
    // if input format is name1
    //	    this method returns "env" in leftPart & name1 in rightPart
    const char* pointPtr = strchr(leftPart, '.');
    if (pointPtr==0){
	rightPart = leftPart;
	leftPart = "env";
    }
    else{
	*(char*)pointPtr = '\0'; // truncate leftPart up to the '.' sign
	pointPtr++;
	rightPart = pointPtr; // set rigthPart to the string starting after '.'
    };
};

unsigned short cl_GenParser::IdentifyToken(const cl_GenString& token){
  unsigned short result= NOT_A_KEYWORD;
  switch (token.StringSize()){
  case(0) :
    result=GENTEXT;
    break;
  case(1) :
    if (strncmp(token, "*",1)==0) result=GENCOMMENT;
    else if (strncmp(token, "+",1)==0) result=GENINDENT;
    else if (strncmp(token, "-",1)==0) result=GENUNINDENT;
    break;
  case(2) :
    if (strncmp(token, "CR",2)==0) result=GENCR;
    else if (strncmp(token, "IF",2)==0) result=GENIF;
    else if (strncmp(token, "DO",2)==0) result=GENDO;
    break;
  case(3) :
    if (strncmp(token, "VAR",3)==0) result=GENVAR;
    else if (strncmp(token, "FOR",3)==0) result=GENFOR;
    else if (strncmp(token, "SET",3)==0) result=GENSET;
    else if (strncmp(token, "TAB",3)==0) result=GENTAB;
    else if (strncmp(token, "ADD",3)==0) result=GENADD;
    else if (strncmp(token, "NEW",3)==0) result=GENNEW;
    else if (strncmp(token, "ENV",3)==0) result=GENENV;
    break;
  case(4) :
    if (strncmp(token, "ELSE",4)==0) result=GENELSE;
    else if (strncmp(token, "ONDO",4)==0) result=GENONDO;

    break;
  case(5) :
    if (strncmp(token, "ENDIF",5)==0) result= GENENDIF;
    else if (strncmp(token, "CLASS",5)==0) result=GENCLASS;
    else if (strncmp(token, "CLEAN",5)==0) result=GENCLEAN;
    break;
  case(6) :
    if (strncmp(token, "ENDFOR",6)==0) result=GENENDFOR;
    else if (strncmp(token, "SUBSTR",6)==0) result=GENSUBSTR;
    else if (strncmp(token, "SCRIPT",6)==0) result=GENBLOCK;
    else if (strncmp(token, "REMOVE",6)==0) result=GENREMOVE;
    else if (strncmp(token, "INSERT",6)==0) result=GENINSERT;
    else if (strncmp(token, "APPEND",6)==0) result=GENAPPEND;
    else if (strncmp(token, "GETENV",6)==0) result=GENGETENV;

    break;
  case(7) :
    if (strncmp(token, "INCLUDE",7)==0) result=GENINCLUDE;
    else if (strncmp(token, "ASCTIME",7)==0) result=GENASCTIME;
    break;
  case(8) :
    if (strncmp(token, "ENDCLASS",8)==0) result=GENENDCLASS;
    else if (strncmp(token, "OFSTREAM",8)==0) result=GENOFSTREAM;
    break;
  case(9) :
    if (strncmp(token, "SETSTRING",9)==0) result=GENSETSTRING;
    else if (strncmp(token, "ENDSCRIPT",9)==0) result=GENENDBLOCK;
    else if (strncmp(token, "PARSENODE",9)==0) result=GENPARSENODE;
    else if (strncmp(token, "PARSEMETA",9)==0) result=GENPARSEMETA;
    else if (strncmp(token, "INSERTVAR",9)==0) result=GENINSERTVAR;
    break;
  case(12) :
    if (strncmp(token, "APPENDSTRING",12)==0) result=GENAPPENDSTRING;
    else if (strncmp(token, "PARSEMETAVAR",12)==0) result=GENPARSEMETAVAR;
    break;

  default :
    break;
  };
  return result;
};
cl_GenClass* cl_GenParser::ParseClass(const char* name){
    cl_GenClass* newClass = new cl_GenClass(name);
    cl_GenStatement* loopStmt;
    short endClass=0;
    PRINT("Parsing \"" << newClass->Name() << "\" class (at line " << currLine << ")...");

    while (endClass==0){
	loopStmt=ParseStatement();
	switch(loopStmt->IsA()){
	case(GENTHATSALL):
	case(GENENDCLASS):
	    endClass=1;
	    newClass->AddMember(loopStmt);
	    break;
	case(GENEMPTY):
	    delete (loopStmt);
	    break;
	case(GENBLOCK): // add as a methods
	    newClass->AddMethod((cl_GenBlock*)loopStmt);
	    break;
	default:
	    ShowError("statements not allowed in class definition");
	    break;
	};
    };
    PRINT("...class \"" << newClass->Name() << "\" parsed (at line " << currLine << ").");

    return newClass;
};

cl_GenBlock* cl_GenParser::ParseBlock(const char* name){
    cl_GenBlock* block = new cl_GenBlock(name);
    cl_GenStatement* loopStmt;
    short endBlock=0;
    if (!main)
	main = block;

    PRINT("Parsing \"" << block->Name() << "\" block (at line " << currLine << ")...");

    while (endBlock==0){
	loopStmt=ParseStatement();
	switch(loopStmt->IsA()){
	case(GENTHATSALL):
	case(GENENDBLOCK):
	    endBlock=1;
	case(GENEMPTY):
	    delete (loopStmt);
	    break;
	case(GENBLOCK): // add as a subBlock
	    main->AddBlock((cl_GenBlock*)loopStmt);
	    break;
	default: // add as a statement of this block
	    block->Add(loopStmt);
	    break;
	};
    };
    PRINT("...block \"" << block->Name() << "\" parsed (at line " << currLine << ").");

    return block;
};

cl_GenStatement* cl_GenParser::ParseStatement(){
    cl_GenStatement* stmt=0;
    cl_GenStatement* loopStmt;
    int type;
	short start,length,endfor,beforeElse,endif;
	std::ifstream* is;
	const char* equalPtr;
    const char* where=text+curr;
    switch (Current()){
    case('\0') : // end of input
	PRINT("that's all folks");
	stmt = new cl_GenThatsAll();
	break;
    case('$'): // got a keyword
	Skip(); // skip $
	SkipChars(" ");
//	NextToken(token1, "", " $[\n\t\r", 0);
	NextWord(token1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ*+-");
	type = IdentifyToken(token1);
//	WriteSourceLine();
//	PRINT("keyword=" << token1 << "type = " << type  );
	switch(type) {
	case (NOT_A_KEYWORD) :
	    ShowError ("Unexpected keyword", token1);
	    break;
	case(GENBLOCK) :
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "\n\t\r ", "]");
	    stmt = ParseBlock(token2);
	    break;
	case(GENTEXT):
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "\n\t\r ", "]");
	    stmt = new cl_GenText(token2);
	    break;
	case(GENVAR) :
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "\n\t\r ", "]");
	    VarName(token2, token3);
	    stmt = new cl_GenVariable(token2, token3);
	    break;
	case(GENFOR) :
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "\n\t\r ", ",");
	    VarName(token2, token3);
	    NextToken(token4, "\n\t\r ", "]");
	    stmt = new cl_GenFor(token2, token3, token4);
	    endfor = 0;
	    while (endfor==0) {
		loopStmt=ParseStatement();
		switch (loopStmt->IsA()){
		case(GENENDFOR) :
    		    ((cl_GenFor *)stmt)->AddEpilogue((cl_GenEndFor *)loopStmt);
		    endfor=1;
		    break;
		case(GENTHATSALL) :
		case(GENBLOCK):
		case(GENENDBLOCK):
		    delete loopStmt;
		    ShowError("$ENDFOR expected");
		    break;
		case(GENEMPTY):
		    delete loopStmt;
		    break;
		default:
		    ((cl_GenFor *)stmt)->AddLoop(loopStmt);
		    break;
		};
	    };
	    break;
	case(GENENDFOR) :
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "", "]");
	    stmt = new cl_GenEndFor(token2);
	    break;
	case(GENIF) :
	    stmt = new cl_GenIf();
	    SkipTo("\n\t\r", '[');
	    NextToken(token2, "\n\t\r ", "]");
	    equalPtr = strchr(token2, '=');
	    if (equalPtr==0){
		// boolean variable expression
	    	VarName(token2, token3);
		((cl_GenIf *)stmt)->AddCondition(new cl_GenBoolVariable(token2, token3));
	    }
	    else{
		// node.var = value  or node.var= node.var expression
		*(char*)equalPtr = '\0'; // truncate token2 up to the '=' sign
		equalPtr++;
		VarName(token2,token3);
	    	const char* quotePtr = strchr(equalPtr, '"');
		if (quotePtr==0){
		    // node.var=node.var expression
		    token4 = equalPtr; // set token4 to the string starting after '='
		    VarName(token4, token5);
		    ((cl_GenIf *)stmt)->AddCondition(new cl_GenBoolVarEqualVar(token2, token3, token4, token5));
		}
		else{
		    // node.var="value" expression
		    quotePtr++;
		    token4 = quotePtr; // set token4 to the string starting after '"'
		    quotePtr = strchr(token4,  '"'); // search ending '"'
		    if (quotePtr) {
			*(char*)quotePtr = '\0'; // truncate token4 up to the '"' sign
			((cl_GenIf *)stmt)->AddCondition(new cl_GenBoolVarEqualValue(token2, token3, token4));
		    }
		    else{
			ShowError("Unmatched string", token4);
		    }
		}
	    }
	    beforeElse = -1;
	    endif = 0;
	    while (endif==0) {
		loopStmt=ParseStatement();
		switch (loopStmt->IsA()){
		case(GENENDIF) :
		    delete loopStmt;
		    endif=1;
		    break;
		case(GENTHATSALL) :
		case(GENBLOCK):
		case(GENENDBLOCK):
		    delete loopStmt;
		    ShowError("$ENDIF expected");
		    break;
		case(GENEMPTY):
		    delete loopStmt;
		    break;
		case(GENELSE):
		    beforeElse=0;
		    delete loopStmt;
		    break;
		default:
		    if (beforeElse)
			((cl_GenIf *)stmt)->AddThen(loopStmt);
		    else
			((cl_GenIf *)stmt)->AddElse(loopStmt);
		    break;
		};
	    };
	    break;
	case(GENELSE) :
	    stmt = new cl_GenElse();
	    break;
	case(GENENDIF) :
	    stmt = new cl_GenEndIf();
	    break;
	case(GENCR) :
	    stmt = new cl_GenCR();
	    break;
	case(GENINDENT) :
	    stmt = new cl_GenIndent();
	    break;
	case(GENUNINDENT) :
	    stmt = new cl_GenUnIndent();
	    break;
        case(GENTHATSALL):
	  stmt = new cl_GenThatsAll();
	  break;
	case(GENEMPTY):
	    stmt = new cl_GenEmpty();
	    break;
  	case(GENCOMMENT) :
	    // create an empty statement that will be dropped
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "\n\t\r ", "]");
	    stmt = new cl_GenEmpty();
	    break;
	case(GENSET) :
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "\n\t\r ", ",");
	    VarName(token2, token3);
	    NextToken(token4, "\n\t\r ", "]");
	    VarName(token4, token5);
	    stmt = new cl_GenSet(token2, token3, token4, token5);
	    break;
	case(GENSETSTRING) :
	    SkipTo("\n\t\r ", '[');
	    SkipTo("\n\t\r ", '"');
	    // take all string bytes in token 2
	    NextToken(token2, "", "\"");
	    SkipTo("\n\t\r ", ',');
	    NextToken(token3, "\n\t\r ", "]");
	    VarName(token3, token4);
	    stmt = new cl_GenSetString(token2, token3, token4);
	    break;
	case(GENSUBSTR) :
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "\n\t\r ", ",");
	    VarName(token2, token3);
	    NextToken(token4, "\n\t\r ", ",");
	    NextToken(token5, "\n\t\r ", ",");
	    start = atoi(token5);
	    NextToken(token5, "\n\t\r ", "]");
	    length = atoi(token5);
	    VarName(token4, token5);
	    stmt = new cl_GenSubstr(token2, token3, token4, token5, start, length);
	    break;
	case(GENTAB) :
	    stmt = new cl_GenTAB();
	    break;
	case(GENDO) :
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "\n\t\r", "]");
	    stmt = new cl_GenDo(main,token2);
	    break;
	case(GENENDBLOCK) :
	    stmt = new cl_GenEndBlock();
	    break;
	case(GENINCLUDE) :
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "\n\t\r ", "]");
	    is= new std::ifstream(token2);
	    if (is->good()){
		PRINT("Including file :\"" << token2 << "\" (at line " << currLine << ")");
		Include(*is);
	    }
	    else{
		ShowError("Cannot find include file", token2);
	    };
	    // create a dummy statement and continue to parse
	    stmt = new cl_GenEmpty();
	    delete (is);
	    break;
	  case(GENCLASS) :
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "\n\t\r ", "]");
	    stmt = ParseClass(token2);
	    break;
	  case(GENENDCLASS) :
	    stmt = new cl_GenEndClass();
	    break;
	  case(GENNEW) :
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "\n\t\r ", ",");
	    NextToken(token3, "\n\t\r ", "]");
	    stmt = new cl_GenNew(token2, token3);
	    break;
	case (GENADD) :
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "\n\t\r ", ",");
	    VarName(token2, token3);
	    NextToken(token4, "\n\t\r ", "]");
	    stmt = new cl_GenAdd(token2, token3, token4);
	    break;
	case (GENREMOVE) :
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "\n\t\r ", ",");
	    VarName(token2, token3);
	    NextToken(token4, "\n\t\r ", "]");
	    stmt = new cl_GenRemove(token2, token3, token4);
	    break;
	case (GENCLEAN) :
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "\n\t\r ", "]");
	    VarName(token2, token3);
	    stmt = new cl_GenClean(token2, token3);
	    break;
	case (GENONDO) :
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "\n\t\r ", ",");
	    VarName(token2, token3);
	    NextToken(token4, "\n\t\r ", "]");
	    stmt = new cl_GenOnDo(token2, token3, token4);
	    break;
	case(GENPARSENODE) :
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "\n\t\r ", ",");
	    VarName(token2, token3);
	    NextToken(token4, "\n\t\r ", "]");
	    stmt = new cl_GenParseNode(token2, token3,token4);
	    break;
	case(GENINSERT) :
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "\n\t\r ", "]");
	    stmt = new cl_GenInsert(token2);
	    break;
	case(GENASCTIME) :
	    stmt = new cl_GenAsctime();
	    break;
	case(GENPARSEMETA) :
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "\n\t\r ", "]");
	    stmt = new cl_GenParseMeta(token2);
	    break;
	case(GENAPPEND) :
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "\n\t\r ", ",");
	    VarName(token2, token3);
	    NextToken(token4, "\n\t\r ", "]");
	    VarName(token4, token5);
	    stmt = new cl_GenAppend(token2, token3, token4, token5);
	    break;
	case(GENAPPENDSTRING) :
	    SkipTo("\n\t\r ", '[');
	    SkipTo("\n\t\r ", '"');
	    // take all string bytes in token 2
	    NextToken(token2, "", "\"");
	    SkipTo("\n\t\r ", ',');
	    NextToken(token3, "\n\t\r ", "]");
	    VarName(token3, token4);
	    stmt = new cl_GenAppendString(token2, token3, token4);
	    break;
	case(GENENV) :
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "\n\t\r ", "]");
	    stmt = new cl_GenEnv(token2);
	    break;
	case(GENGETENV) :
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "", ",");
	    NextToken(token3, "\n\t\r ", "]");
	    VarName(token3, token4);
	    stmt = new cl_GenGetEnv(token2, token3, token4);
	    break;
	case(GENINSERTVAR) :
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "\n\t\r ", "]");
	    VarName(token2, token3);
	    stmt = new cl_GenInsertVar(token2, token3);
	    break;
	case(GENPARSEMETAVAR) :
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "\n\t\r ", "]");
	    VarName(token2, token3);
	    stmt = new cl_GenParseMetaVar(token2, token3);
	    break;
	case(GENOFSTREAM) :
	    SkipTo("\n\t\r ", '[');
	    NextToken(token2, "\n\t\r ", "]");
	    VarName(token2, token3);
	    stmt = new cl_GenOfstream(token2, token3);
	    break;
	default:
	    ShowError ("Unexpected keyword", token1);
	    break;
	};
	break;
    default:
	// got a literal
	NextToken(token1, "\n\t\r", "$", 0);
	if (token1.StringSize()==0)
		stmt = new cl_GenEmpty();
	else {
//		PRINT("literal=\"" << token1 << "\"" );
		stmt = new cl_GenText(token1);
	};
	break;
    };
    if (stmt)
        stmt->SetSource(where,currLine);
    else
	ShowError("Cannot build any statement");

    return stmt;
};
