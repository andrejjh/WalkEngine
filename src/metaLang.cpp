/*
 *  $Id: metaLang.cc,v 1.0 1997/01/15 07:20:50 andre Exp $
 */

#include <ctype.h>
#include <regex.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <metaLang.h>
#include <metaGen.h>
// extern char *__loc1;
char* my_regcmp(const char* pattern){
	regex_t* r=(regex_t *) malloc(sizeof(regex_t));
	memset(r,0,sizeof(regex_t));
	if ((regcomp(r,pattern,REG_EXTENDED | REG_NEWLINE ))) {
		free(r);r=NULL;
	};
	return (char*)r;
};
char* my_regex(char* re, char* s){
	char* after=NULL;
	regmatch_t m;
	int r=regexec((const regex_t*)re,(const char*)s,1,&m,0);
	if (r==0 && m.rm_so==0)
		after= s + m.rm_eo;
	return after;
};
void my_regfree(char* re){
	regfree((regex_t*)re);
};
// cl_MRule Class
// --------------
void* cl_MRule::generatedCode=NULL;
boolean cl_MRule::skipping(FALSE);
boolean cl_MRule::verbose(TRUE);
cl_GenString cl_MRule::source ("");
cl_MParserIStream* cl_MRule::metaStream=NULL;
cl_MParserIStream* cl_MRule::sourceStream=NULL;
cl_MGrammar* cl_MRule::grammar=NULL;
cl_GenGlossary cl_MRule::rulesDecl(FALSE);

cl_MRule::cl_MRule()
    :  line(0) {
};

void cl_MRule::StreamTo(std::ostream& os) const{
  os.put((unsigned char)IsA()); // write rule type as an header
  write(os);
  os.put((unsigned char)IsA()); // write rule type as a trailer
};

cl_MRule* cl_MRule::StreamFrom(std::istream& is){
    short type=0;
    cl_MRule* aRule;
    type = is.get();

    switch (type){
    case(M_GRAMMAR) : aRule = new cl_MGrammar(); break;
    case(M_SEQUENCE) : aRule = new cl_MSequence(); break;
    case(M_MANYOF) : aRule = new cl_MManyOf(); break;
    case(M_ONEOF) : aRule = new cl_MOneOf(); break;
    case(M_BOOLEAN) : aRule = new cl_MBoolean(); break;
    case(M_CHAR) : aRule = new cl_MChar(); break;
    case(M_IDENTIFIER) : aRule = new cl_MIdentifier(); break;
    case(M_INTEGER) : aRule = new cl_MInteger(); break;
    case(M_NULL) : aRule = new cl_MNull(); break;
    case(M_STRING) : aRule = new cl_MString(); break;
    case(M_LITERAL) : aRule = new cl_MLiteral(); break;
    case(M_RULENAME) : aRule = new cl_MRuleName(); break;
    case(M_COMMENT) : aRule = new cl_MComment(); break;
    case(M_LITERALSTRING) : aRule = new cl_MLiteralString(); break;
    case(M_PATHNAME) : aRule = new cl_MPathName(); break;
    case(M_TOKEN) : aRule = new cl_MToken(); break;
    case(M_EOF) : aRule = new cl_MEOF(); break;


    default :
	PRINT ("cl_MRule::StreamFrom unexpected rule type=" <<
		type << " ,at=" << is.gcount());
	exit(-1);
	break;
    };
    aRule->read(is);
    if (type != is.get()) {
	PRINT ("cl_MRule::StreamFrom unmatched rule type=" <<
		type << " ,at=" << is.gcount());
	exit(-1);
    };
    return aRule;
};

cl_MRule* cl_MRule::ParseGrammar(std::istream& is){
  metaStream = new cl_MParserIStream(is);
  if (grammar)
    delete (cl_MGrammar*)grammar;
  grammar= new cl_MGrammar();
  if (!grammar->MetaParse()){
    delete (cl_MGrammar*)grammar;
    grammar=NULL;
  };
  delete metaStream;
  metaStream=NULL;
  return grammar;
};
void* cl_MRule::ParseSource(std::istream& is){
  if (grammar){
    sourceStream = new cl_MParserIStream(is);
    generatedCode = NULL;
    grammar->Parse(NULL);
    return generatedCode;
  };
  return NULL;
};


void cl_MRule::ShowError(const char* s){

  if (sourceStream)
    sourceStream->ShowWhere(s);
  if (metaStream)
    metaStream->ShowWhere(s);
  exit(-1);
};
void cl_MRule::ShowError(const char* s1, const char* s2){
  cl_GenString s=s1;
  s.Concat(": ");
  s.Concat(s2);
  s.Concat(".");
  ShowError(s);
};
void cl_MRule::ShowError(const char* s1, const char* s2, const char* s3){
  cl_GenString s=s2;
  s.Concat(" \"");
  s.Concat(s3);
  s.Concat("\"");
  ShowError(s1,s);
};
void cl_MRule::ShowWarning(const char* s){
  PRINT( "Warning " << s );
};
void cl_MRule::ShowWarning(const char* s1, const char* s2){
  cl_GenString s=s1;
  s.Concat(": ");
  s.Concat(s2);
  ShowWarning(s);

};
void cl_MRule::ShowWarning(const char* s1, const char* s2, const char* s3){
  cl_GenString s=s1;
  s.Concat(": ");
  s.Concat(s2);
  s.Concat(" , ");
  s.Concat(s3);
  ShowWarning(s);

};

boolean cl_MRule::Generate(void* genContext,const char* alias){
  void *subContext=NULL;
  if (!skipping){
    EnterRule(*this,genContext,subContext,alias);
    if (subContext)
      if (generatedCode==NULL) // save top
	generatedCode = subContext;
  };
  if (!Parse(subContext)){
    ShowError ("cl_MRule::Generate", " parsing failed...");
    // delete generated context if any
    if (subContext) delete subContext;
    subContext =NULL;
    return FALSE;
  };
  if (!skipping){
    ExitRule(*this,genContext,subContext,alias);
  };
  return TRUE;
};


// cl_MDefinition Class
// --------------------
short cl_MDefinition::StringToType(const char *s){
  if (strcmp(s,"sequence")==0) return M_SEQUENCE;
  else if (strcmp(s,"manyOf")==0) return M_MANYOF;
  else if (strcmp(s,"oneOf")==0) return M_ONEOF;
  else if (strcmp(s,"token")==0) return M_TOKEN;
  return M_NOT_A_RULE;
};

cl_MDefinition* cl_MDefinition::MPFactory() {
  cl_MDefinition* definition=NULL;
  cl_GenString name,type;

  metaStream->GetIdentifier(name," \t");
  metaStream->GetIdentifier(type," \t",TRUE);
  switch (StringToType(type)) {
  case (M_SEQUENCE): definition = new cl_MSequence();break;
  case (M_MANYOF): definition = new cl_MManyOf();break;
  case (M_ONEOF): definition = new cl_MOneOf();break;
  case (M_TOKEN): definition = new cl_MToken();break;
  default :
    ShowError ("cl_MDefinition::MPFactory","Unexpected definition type",type);
  };
  if (definition){
    //    std::cout <<  "cl_MDefinition::MPFactory " << name << " as " << type << std::endl;
    definition->SetName(name);
    rulesDecl.Add(name,definition);
  };
  return definition;
};


// cl_MDefElement Class
// --------------------
cl_MDefElement::cl_MDefElement()
  : optional(FALSE),alias(""){};

cl_MDefElement* cl_MDefElement::MPFactory() {
  boolean optional=FALSE;
  cl_GenString alias;
  cl_MDefElement* definition=NULL;
  short c= metaStream->GetChar(" ");
  if (isalpha(c)){// got an alias for definition element
    metaStream->GetIdentifier(alias," =");
    c= metaStream->GetChar(" =");
  };
  if (c=='?'){// got an optional definition element
    optional = TRUE;
    c= metaStream->GetChar(" ?");
  };
  if (c < 0)
    return NULL;
  switch (c) {
  case ('$'):
    definition = new cl_MRuleName();
    //    std::cout << "cl_MDefElement::MPFactory" << " adding a ruleName" << std::endl;
    break;
  case ('#'):
    definition = (cl_MDefElement* )cl_MBuildIn::MPFactory();
    definition->SetAlias(definition->Name());
    //    std::cout << "cl_MDefElement::MPFactory" << " adding a build-in" << std::endl;
    break;
  case ('\''):
    definition = new cl_MLiteral();
    //    std::cout << "cl_MDefElement::MPFactory" << " adding a literal" << std::endl;


    break;
  default :
    alias="";alias.Add(c);
    ShowError("cl_MDefElement::MPFactory","unexpected type",alias);
    break;
  };
  if (definition){
    definition->SetOptional(optional);
    if (alias.StringSize())
      definition->SetAlias(alias);
  };
  return definition;
};

// cl_MBuildIn Class
// -----------------
short cl_MBuildIn::StringToType(const char *s){
  if (strcmp(s,"null")==0) return M_NULL;
  else if (strcmp(s,"id")==0) return M_IDENTIFIER;
  else if (strcmp(s,"string")==0) return M_STRING;
  else if (strcmp(s,"char")==0) return M_CHAR;
  else if (strcmp(s,"integer")==0) return M_INTEGER;
  else if (strcmp(s,"comment")==0) return M_COMMENT;
  else if (strcmp(s,"literal")==0) return M_LITERALSTRING;
  else if (strcmp(s,"boolean")==0) return M_BOOLEAN;
  else if (strcmp(s,"pathname")==0) return M_PATHNAME;
  else if (strcmp(s,"eof")==0) return M_EOF;
  return M_NOT_A_RULE;
};

cl_MBuildIn* cl_MBuildIn::MPFactory(){
  cl_MBuildIn* buildIn=NULL;

  cl_GenString s;
  metaStream->SkipChar();
  metaStream->GetIdentifier(s," \n", TRUE);
  //  std::cout <<  "cl_MBuildIn::MPFactory " << s << std::endl;

  switch (StringToType(s)) {
  case (M_BOOLEAN): buildIn = new cl_MBoolean();break;
  case (M_CHAR): buildIn = new cl_MChar();break;
  case (M_IDENTIFIER) : buildIn = new cl_MIdentifier();break;
  case (M_INTEGER) : buildIn = new cl_MInteger();break;
  case (M_NULL): buildIn = new cl_MNull();break;
  case (M_STRING): buildIn = new cl_MString();break;
  case (M_COMMENT): buildIn = new cl_MComment();break;
  case (M_LITERALSTRING): buildIn = new cl_MLiteralString();break;
  case (M_PATHNAME): buildIn = new cl_MPathName();break;
  case (M_EOF): buildIn = new cl_MEOF();break;


  default :
    ShowError("cl_MBuildIn::MPFactory","Unexpected build-in type",s);
  };
  return buildIn;
};

std::ostream& cl_MBuildIn::write (std::ostream& os) const {
  return os;
};

std::istream& cl_MBuildIn::read  (std::istream& is){
  return is;
};

// cl_MBoolean Class
cl_MBoolean::cl_MBoolean()
  : value(TRUE){
};
cl_MBoolean::~cl_MBoolean(){
};
boolean cl_MBoolean::Match(){
  return FALSE;
};
boolean cl_MBoolean::Parse(void* /*context*/){
  return FALSE;
};
// cl_MChar Class
cl_MChar::cl_MChar()
  : value(' '){
};
cl_MChar::~cl_MChar(){
};
boolean cl_MChar::Match(){
  return TRUE;
};
boolean cl_MChar::Parse(void* /*context*/){
  sourceStream->SkipChar();
  return TRUE;
};
// cl_MIdentifier Class
cl_MIdentifier::cl_MIdentifier()
  : value(""){
};
cl_MIdentifier::~cl_MIdentifier(){
};
boolean cl_MIdentifier::Match(){
  return (isalpha((char)sourceStream->At()));
};
boolean cl_MIdentifier::Parse(void* /*context*/){
  sourceStream->GetAlnumString(value);
  //  std::cout << "cl_MIdentifier::Parse :" << value << std::endl;
  return TRUE;
};
// cl_MInteger Class
cl_MInteger::cl_MInteger()
  : value(0){
};
cl_MInteger::~cl_MInteger(){
};
boolean cl_MInteger::Match(){
  return (((char)sourceStream->At() >= '0') && ((char)sourceStream->At() <= '9'));
};
boolean cl_MInteger::Parse(void* /*context*/){
  value = sourceStream->GetShort();
  //    std::cout << "cl_MInteger::Parse :" << value << std::endl;
  return TRUE;
};
// cl_MNull Class
cl_MNull::cl_MNull(){
};
cl_MNull::~cl_MNull(){
};
boolean cl_MNull::Match(){
  return TRUE;
};
boolean cl_MNull::Parse(void* /*context*/){
  return TRUE;
};
// cl_MString Class
cl_MString::cl_MString()
  : value(""){
};
cl_MString::~cl_MString(){
};
boolean cl_MString::Match(){
  return ((char)sourceStream->At()=='"');
};
boolean cl_MString::Parse(void* /*context*/){
  sourceStream->SkipChar();
  sourceStream->GetIdentifier(value,"\"",FALSE);
  if (sourceStream->At() =='\"') {
    sourceStream->SkipChar();
    //    std::cout << "cl_MString::Parse :" << value << std::endl;
    return TRUE;
  }
  else
    return FALSE;
};
// cl_MComment Class
cl_MComment::cl_MComment()
  : value(""){
};
cl_MComment::~cl_MComment(){
};
boolean cl_MComment::Match(){
    sourceStream->SavePosition();
    char c1= sourceStream->SkipChar();
    char c2= sourceStream->SkipChar();
    sourceStream->RestorePosition();

   return ((c1=='/') && (c2=='*'));
};
boolean cl_MComment::Parse(void* /*context*/){
  short c= 0;
  sourceStream->SkipChar();
  sourceStream->SkipChar();
  value="";
  while ((c != EOF) && (c != '/')){
    while  ((c != EOF) &&(c != '*')){
      c = sourceStream->SkipChar();
      value.Add((char)c);
    };
    c=sourceStream->SkipChar();
    value.Add((char)c);
  };
  // remove trailing */ from value
  value.DeleteLast();
  value.DeleteLast();

  return (c != EOF);
};
// cl_MLiteralString Class
cl_MLiteralString::cl_MLiteralString()
  : value(""){
};
cl_MLiteralString::~cl_MLiteralString(){
};
boolean cl_MLiteralString::Match(){
  return ((char)sourceStream->At()=='\'');
};
boolean cl_MLiteralString::Parse(void* /*context*/){
  sourceStream->SkipChar();
  sourceStream->GetIdentifier(value,"'",FALSE);
  if (sourceStream->At() =='\'') {
    sourceStream->SkipChar();
    //    std::cout << "cl_MString::Parse :" << value << std::endl;
    return TRUE;
  }
  else
    return FALSE;
};
// cl_MPathName Class
cl_MPathName::cl_MPathName()
  : value(""){
};
cl_MPathName::~cl_MPathName(){
};
boolean cl_MPathName::Match(){
  char c= sourceStream->At();
  return (isalpha(c) || ispunct(c));
};
boolean cl_MPathName::Parse(void* /*context*/){
  sourceStream->GetAlpunctString(value);
  return (value.StringSize() > 0);
};
// cl_MLiteral Class
cl_MLiteral::cl_MLiteral()
  : value(""){
};
cl_MLiteral::~cl_MLiteral(){
};
boolean cl_MLiteral::MetaParse(){
  metaStream->SkipChar();
  metaStream->GetIdentifier(value,"\'");
  if (metaStream->At() =='\'')
    metaStream->SkipChar();
  else {
    ShowError("cl_MLiteral::MetaParse"," unmatched ' in literal");
    return FALSE;
  };
  if (strlen(Alias()) ==0) // no alias yet
    SetAlias(value);
  return (value.StringSize() > 0);
};
std::ostream& cl_MLiteral::write (std::ostream& os) const {
  value.write(os);
  return(os);
};
std::istream& cl_MLiteral::read  (std::istream& is){
  value.read(is);
  return(is);
};
boolean cl_MLiteral::Match(){
		return sourceStream->MatchString(value,grammar->IsCaseSensitive());
};

boolean cl_MLiteral::Parse(void* /*context*/){
  //  std::cout << "cl_MLiteral::Parse :" ;
  for (int i=0; i < value.StringSize() ; i++)
    //    std::cout << (char)sourceStream->SkipChar();
    sourceStream->SkipChar();
  //  std::cout << std::endl;
  return TRUE;
};
// cl_MRuleName Class
cl_MRuleName::cl_MRuleName()
  : name(""), rule(NULL){
};
cl_MRuleName::~cl_MRuleName(){
};
boolean cl_MRuleName::MetaParse(){
  boolean result;
  metaStream->SkipChar();
  result=metaStream->GetIdentifier(name," \n");
  if (strlen(Alias())==0) // no alias yet
    SetAlias(name);
  return result;
};
std::ostream& cl_MRuleName::write (std::ostream& os) const {
  name.write(os);
  return(os);
};
std::istream& cl_MRuleName::read  (std::istream& is){
  name.read(is);
  return(is);
};
cl_MRule* cl_MRuleName::ResolveName(){
  if (!rule)
    rule = ((cl_MRule*)rulesDecl.FindObject(name));
  if (!rule)
    ShowError("cl_MRuleName::ResolveName cannot resolve name", name);
  return rule;
};

boolean cl_MRuleName::Match(){
  if (ResolveName())
    return rule->Match();
  else
    return FALSE;
};
boolean cl_MRuleName::Parse(void* context){
  if (ResolveName()){
    rule->Generate(context,Alias());
    return TRUE; // should stop if rule->Parse() failed
  }
  else
    return FALSE;
};

boolean cl_MRuleName::Generate(void* genContext,const char* anAlias){
  // generate target rule within genContext
  if (ResolveName())
    return rule->Generate(genContext,anAlias);
  else
    return NULL;
};

// cl_MSequence Class
// ------------------
cl_MSequence::cl_MSequence()
  : parts(TRUE){
};
cl_MSequence::~cl_MSequence(){
};

std::ostream& cl_MSequence::write (std::ostream& os) const {
  // write sequence parts
  cl_GenIterator iter(parts);
  os << parts.NbItems();
  while (iter.Next()){
    ((cl_MRule*)iter.Current())->StreamTo(os);
  };
  return os;
};

std::istream& cl_MSequence::read  (std::istream& is){
  // read grammar rules
  ulong nbParts;
  cl_MRule* aPart;
  is >> nbParts;
  //    parts.Resize(nbParts);
  for (ulong i=0;i< nbParts;i++){
    aPart = ((cl_MRule *)cl_MRule::StreamFrom(is));
    if (aPart) Add(aPart);
  };
  return is;
};


boolean cl_MSequence::MetaParse(){
  cl_MDefElement* aPart;
  do {
    aPart = cl_MDefElement::MPFactory();
    if (aPart)
      if (aPart->MetaParse())
	Add(aPart);
      else{
	delete aPart;
	aPart=NULL;
      };
  }  while ((aPart) && (!metaStream->AtEnd()) && (metaStream->At() != '\n'));
  return (aPart != NULL);
};
boolean cl_MSequence::Match(){
  cl_MDefElement* currPart;
  cl_GenIterator iter(parts);
  while (currPart = (cl_MDefElement*)iter.Next()){
    if (currPart->Match())
      return TRUE;
    else
      if (!currPart->IsOptional())
				return FALSE;
  };
  return FALSE;
};
boolean cl_MSequence::Parse(void *context){
  cl_MDefElement* currPart;
  cl_GenIterator iter(parts);
  while (currPart = (cl_MDefElement*)iter.Next()){
		Skip();
		ShowWarning("cl_MSequence::Parse at ",currPart->Alias());
    if (currPart->Match()) {
			ShowWarning("cl_MSequence::Parse >Generate ",currPart->Alias());
      currPart->Generate(context,currPart->Alias());
			ShowWarning("cl_MSequence::Parse <Generate ",currPart->Alias());
    } else {
      if (currPart->IsOptional()){
	ShowWarning("cl_MSequence::Parse"," skipping optional part",
		    currPart->Alias());
      } else {
	ShowError("cl_MSequence::Parse","unmatched sequence",
				currPart->Alias());
	return FALSE;
      }
		}
  };
  return TRUE;
};
// cl_MManyOf Class
// ------------------
cl_MManyOf::cl_MManyOf()
  : repeatDef(NULL), begin(NULL), end(NULL), separator(NULL){
};
cl_MManyOf::~cl_MManyOf(){
  if (begin)  delete begin;
  if (end)  delete end;
  if (separator)  delete separator;
  if (repeatDef)  delete repeatDef;

};

std::ostream& cl_MManyOf::write (std::ostream& os) const {
  begin->StreamTo(os);
  end->StreamTo(os);
  separator->StreamTo(os);
  repeatDef->StreamTo(os);
  return os;
};

std::istream& cl_MManyOf::read  (std::istream& is){
  begin = (cl_MDefElement*)cl_MRule::StreamFrom(is);
  end = (cl_MDefElement*)cl_MRule::StreamFrom(is);
  separator = (cl_MDefElement*)cl_MRule::StreamFrom(is);
  repeatDef = (cl_MDefElement*)cl_MRule::StreamFrom(is);
  return is;
};

boolean cl_MManyOf::MetaParse(){

  repeatDef = cl_MDefElement::MPFactory();
  if (repeatDef)
    if (!repeatDef->MetaParse()){
      delete repeatDef;
      repeatDef=NULL;
    };
  begin = cl_MDefElement::MPFactory();
  if (begin)
    begin->MetaParse();
  end = cl_MDefElement::MPFactory();
  if (end)
    end->MetaParse();
  separator = cl_MDefElement::MPFactory();
  if (separator)
    separator->MetaParse();
  return (separator != NULL);
};

boolean cl_MManyOf::Match(){
  if (begin->IsA()!=M_NULL)
    return (begin->Match());
  else
    return repeatDef->Match();
};
boolean cl_MManyOf::Parse(void *context){
	boolean again=TRUE;
  Skip();
  if (!begin->Parse(context)){
    ShowError( "cl_MManyOf::Parse", " begin expected",begin->Name());
    return FALSE;
  };
  while (again) {
    Skip();
    if (repeatDef->Match()) {// parse it!
      repeatDef->Generate(context,repeatDef->Alias());
			Skip();
			if (separator->Match()) {// parse it!
				separator->Parse(context);
			} else {
				again=FALSE;
			}

    } else {
//			ShowWarning("cl_MManyOf::Parse" " rDef expected",repeatDef->Name());
			again=FALSE;
		}
  };
  Skip();
  if (end->Match()) {//parse it!
		end->Parse(context);
	} else {
    ShowError("cl_MManyOf::Parse" " end expected",end->Name());
    return FALSE;
  };
  return TRUE;
};

// cl_MOneOf Class
// ------------------
cl_MOneOf::cl_MOneOf()
  : cases(TRUE), matchingCase(NULL){
};
cl_MOneOf::~cl_MOneOf(){
};


std::ostream& cl_MOneOf::write (std::ostream& os) const {
  // write sequence parts
  cl_GenIterator iter(cases);
  os << cases.NbItems();
  while (iter.Next()){
    ((cl_MRule*)iter.Current())->StreamTo(os);
  };
  return os;
};

std::istream& cl_MOneOf::read  (std::istream& is){
  // read parts
  ulong nbCases;
  cl_MRule* aCase;
  is >> nbCases;
  //    cases.Resize(nbCases);
  for (ulong i=0;i< nbCases;i++){
    aCase = ((cl_MRule *)cl_MRule::StreamFrom(is));
    if (aCase) Add(aCase);
  };
  return is;
};

boolean cl_MOneOf::MetaParse(){
  cl_MDefElement* aPart;
  do {
    aPart = cl_MDefElement::MPFactory();
    if (aPart)
      if (aPart->MetaParse())
	Add(aPart);
      else{
	delete aPart;
	aPart=NULL;
      };
  }  while ((aPart) && (!metaStream->AtEnd()) && (metaStream->At() != '\n'));
  return (aPart != NULL);
};
boolean cl_MOneOf::Match(){
  matchingCase=NULL;
  cl_GenIterator iter(cases);
  while (iter.Next()){
    if (((cl_MDefElement*)iter.Current())->Match()){
      matchingCase = (cl_MDefElement*)iter.Current();
      return TRUE;
    };
  };
  //  std::cout << "cl_MOneOf::Match" << " none of the cases match" << std::endl;
  return FALSE;
};
boolean cl_MOneOf::Parse(void* context){
  boolean result=FALSE;
  matchingCase=NULL;
  cl_GenIterator iter(cases);
  while (iter.Next()){
    if (((cl_MDefElement*)iter.Current())->Match()){
      matchingCase = (cl_MDefElement*)iter.Current();
    };
  };
  if (matchingCase){
//    result= matchingCase->Generate(context,"?alias?");
    result= matchingCase->Generate(context,matchingCase->Alias());
	}
  else
        ShowError("cl_MOneOf::Parse","none of the cases match in",Alias());;
  return result;
};


// cl_MToken Class
// ------------------
cl_MToken::cl_MToken()
 : comp(NULL){
};
cl_MToken::~cl_MToken(){
  if (comp) my_regfree (comp);
};

std::ostream& cl_MToken::write (std::ostream& os) const {
  def.write(os);
  return os;
};

std::istream& cl_MToken::read  (std::istream& is){
  def.read(is);
  comp = my_regcmp ((const char*)def);
  return is;
};

boolean cl_MToken::MetaParse(){
  short c= metaStream->GetChar(" ");
  if (c =='\'')
    metaStream->SkipChar();
  else {
    ShowError("cl_MToken::MetaParse"," reg.exp. should start with '");
    return FALSE;
  };
  metaStream->GetIdentifier(def,"\'",TRUE);
  if (metaStream->AtEnd()) {
    ShowError("cl_MToken::MetaParse"," reg.exp. should stop with '");
    return FALSE;
  };
  comp= my_regcmp((const char*)def);
  if (comp == NULL){
    ShowError("cl_MToken::MetaParse"," cannot compile reg.exp.",def);
    return FALSE;
  };

  return TRUE;
};

boolean cl_MToken::Match(){
  after = NULL;
  start = (char*)sourceStream->WhereInInput();
  if ((start != NULL) && (comp != NULL) ){
    after= my_regex(comp,start);
  };
  return (after != NULL);
};

boolean cl_MToken::Parse(void* /*context*/){
  if (after != NULL){
    // Advance in source stream, store token value
    sourceStream->Advance(after - start);
		value.Assign(start,after-start);
  };
  return (after != NULL);
};
// cl_MEOF Class
cl_MEOF::cl_MEOF(){
};
cl_MEOF::~cl_MEOF(){
};
boolean cl_MEOF::Match(){
  return sourceStream->AtEnd() ;
};
boolean cl_MEOF::Parse(void* /*context*/){
  return TRUE;
};

// cl_MGrammar Class
// --------------
cl_MGrammar::cl_MGrammar()
  : rules(TRUE),caseSensitive(FALSE){
};
cl_MGrammar::~cl_MGrammar(){
};

std::ostream& cl_MGrammar::write (std::ostream& os) const {
  name.write(os);
  mainRule.StreamTo(os);
  skipRule.StreamTo(os);
  // write grammar rules
  cl_GenIterator iter(rules);
  os << rules.NbItems();
  while (iter.Next()){
    ((cl_MRule*)iter.Current())->StreamTo(os);
  };
  return os;
};

std::istream& cl_MGrammar::read  (std::istream& is){
  name.read(is);
  mainRule.StreamFrom(is);
  skipRule.StreamFrom(is);
  // read grammar rules
  ulong nbRules;
  cl_MRule* aRule;
  is >> nbRules;
  //    subRules.Resize(nbRules);
  for (ulong i=0;i< nbRules;i++){
    aRule = ((cl_MRule *)cl_MRule::StreamFrom(is));
    if (aRule) Add(aRule);
  };
  return is;
};

boolean cl_MGrammar::MetaParse(){
  cl_MDefinition* aDef;
  cl_GenString s;
  short c1,c2;
  // parse grammar header
  metaStream->GetIdentifier(s," \t");
  SetName(s);
  metaStream->GetIdentifier(s," \t",TRUE);
  c1 = metaStream->GetChar(" ");
  mainRule.MetaParse();
  c2 = metaStream->GetChar(" ");
  skipRule.MetaParse();
  metaStream->SkipChar();
  if ( (c1 != '$') || (c2 != '$') || (strcmp(s,"grammar") > 0) ) {
    ShowError ("cl_MGrammar::MetaParse", " expecting <name> grammar $main $skip");
    return FALSE;
  };
  caseSensitive=strcmp(s,"GRAMMAR");
  // parse rules defintions
  while (!metaStream->AtEnd()){
    switch (metaStream->At()){
    case ('#'):// this is a comment line, skip it
      metaStream->GetIdentifier(s,"\n");
    case('\n'):// empty line, skip it
      metaStream->SkipChar();
      break;
    default : // rule definition line, parse it
      aDef = cl_MDefinition::MPFactory();
      if ((aDef) &&  aDef->MetaParse()){
	metaStream->SkipChar();
	Add(aDef);
      }
      else // cannot parse what should be a definition line
	return FALSE;
    };

  };
  return TRUE;
};


boolean cl_MGrammar::Match(){
  return (mainRule.Match());
};

boolean cl_MGrammar::Parse(void *context){
  return mainRule.Generate(context,mainRule.Alias());
};


void cl_MGrammar::SkipInSource(){
  if (skipRule.Match()){
    cl_MRule::SetSkipping(TRUE);
    skipRule.Parse(NULL);
  };
  cl_MRule::SetSkipping(FALSE);

};
