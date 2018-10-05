//
//  $Id: generic.cc,v 1.0 1996/04/30 06:41:35 andre Exp $
//

#include <stdlib.h>
#include <stdio.h>
#include <genNode.h>
//#include <genLang.h>

// cl_GenDataMember class
// ---------------------
cl_GenDataMember::cl_GenDataMember(dataMemberType aDMT,const char*aType, const char* aName)
  :dmt(aDMT),type(aType),name(aName){
};

// cl_GenNodeClass class
// ---------------------
cl_GenNodeClass::cl_GenNodeClass(const char* aName, cl_GenGlossary& theMethods)
  :name(aName), methods(theMethods){
};
void cl_GenNodeClass::AddNodeDataMembers(cl_GenNode& aNode){
    cl_GenDataMember* dm=0;
    cl_GenIterator iter(dataMembers);
    while ((dm=((cl_GenDataMember*)iter.Next()))){
      switch (dm->DMType()){
      case(cl_GenDataMember::DM_SIMPLE):
	if (strcmp(dm->Type(),"boolean")==0)
	    aNode.AddCondition(dm->Name(), FALSE);
	else if (strcmp(dm->Type(),"string")==0)
	    aNode.AddStringValue(dm->Name(),"");
	break;
      case(cl_GenDataMember::DM_CONTAINER):
	aNode.AddContainer(dm->Name());
	break;
      case(cl_GenDataMember::DM_REFERENCE):
	aNode.AddPointer(dm->Name(),(cl_GenNode*)NULL);
	break;
      };
    };
};
void* cl_GenNodeClass::FindMethod(const char* mName){
  void* found=0;
  found = methods.FindObject(mName);
  if (!found) {
    std::cout << "Cannot find method " << mName << " in class " << name << std::endl;
  };
  return found;
};
void cl_GenNodeClass::AddMethod(const char* mName, void* method){
  methods.Add(mName,method);
};

const char* cl_GenNodeClass::FindClassForContainer(const char* container){
    cl_GenDataMember* dm=0;
    cl_GenIterator iter(dataMembers);
    while ((dm=((cl_GenDataMember*)iter.Next()))){
	if( (dm->DMType()== cl_GenDataMember::DM_CONTAINER)
	  && (strcmp(dm->Name(), container)==0))
	    return dm->Type();
    }
    std::cout << "Cannot find class for container " << container << " in class " << name << std::endl;
    return 0;
};
// cl_GenNode class
// ----------------
short cl_GenNode::nextId(0);
cl_GenGlossary cl_GenNode::classes(TRUE,TRUE);
cl_GenNodeClass* cl_GenNode::newClass=0;
cl_GenArray cl_GenNode::allNodes(FALSE);
cl_GenGlossary* cl_GenNode::scopedNames=0;

char cl_GenNode::outputString[81]= {""};
boolean cl_GenNode::verbose=FALSE;

cl_GenNode::cl_GenNode(  const cl_GenNode* _p, const char* aClassName)
    : nodeId(nextId),p(_p), c(NULL),className(aClassName), exitOnFatal(-1),
    conditions(TRUE), containers(TRUE), pointers(FALSE), strings(TRUE){
      allNodes.AddAt(this,nodeId);
      nextId++;

      AddStringValue("className",aClassName);
      //      if (newClass)
      //	newClass->AddNodeDataMembers(*this);
      if (p) {
	AddPointer("parent", (cl_GenNode*)p);
      };
};

cl_GenNode::~cl_GenNode(){
  allNodes.Remove(this);
};

short cl_GenNode::HasAttribute(const char* attribName) const{
    short type=NONE;
    //    exitOnFatal = 0;
    if (strncmp(attribName,"has_",4)==0)
      // build-in has... boolean attribute
      type = BOOLEAN;
    else {
      if (strings.FindObject(attribName))
	type= STRING;
      else if (conditions.FindObject(attribName))
	type= BOOLEAN;
      else if (pointers.FindObject(attribName))
	type= POINTER;
      else if (containers.FindObject(attribName))
	type= CONTAINER;
      else if (strcmp(attribName,"index")==0)
	type= STRING;
    };
   //    exitOnFatal = -1;
    return type;
};

char cl_GenNode::indentString[85]= {"\0"};
void cl_GenNode::Indent(){
    if (strlen(indentString) < 80)
	strcat(indentString, "    ");
}
void cl_GenNode::UnIndent(){
    if (strlen(indentString))
	indentString[strlen(indentString)-4]= '\0';;
};
cl_GenIterator* cl_GenNode::GetIterator(const char* containerName) const{
    void *found = containers.FindObject(containerName);
    if (found){
	return (new cl_GenIterator(*((cl_GenArray *)found)));
    };
    if (c){
      PRINT ("in cl_GenNode::GetIterator : looking into class node for " << containerName);
      return c->GetIterator(containerName);
    };
    FatalError(containerName);
    return 0;
};
void cl_GenNode::MoveContainer(const char* toCName, const char* fromCName,cl_GenNode* fromNode) {
  cl_GenArray *container= ((cl_GenArray *)fromNode->containers.ExtractObject(fromCName));
  if (container){ // remove it from node, add it to this
    containers.Add(toCName,(void*)container);
  };
};
cl_GenArray* cl_GenNode::ExtractContainer(const char* containerName) {
  return ((cl_GenArray *)containers.ExtractObject(containerName));
};

short cl_GenNode::GetCondition(const char* conditionName) const{
  if (strncmp(conditionName,"has_",4)==0){
    const char* attributeName= 1 + strchr(conditionName,'_');
    if (attributeName) {
#ifdef scriptName
		if (strncmp(attributeName,"script_",7)==0){
			char* scriptName = 1 + strchr(attributeName,'_');
			if (scriptName){// "has_script_<script>" condition on a block node
				cl_GenBlock* b=(cl_GenBlock*)GetVoidPointer("code");
				if (b)
					return (b->FindBlock(scriptName) != NULL);
			}
		}
		else // "has_<attr>" condition
#endif
			return (HasAttribute(attributeName) != NONE);
	}
  }
  else { // search in conditions glossary
    void *found = conditions.FindObject(conditionName);
    if (found){
      return (*((short *)found));
    };
    if (c) // search in class
      return c->GetCondition(conditionName);
  };
  FatalError(conditionName);
  return 0;
};
const char* cl_GenNode::GetValue(const char* attributeName) const{
    void *found = strings.FindObject(attributeName);
    if (found){
	    return (*((cl_GenString *)found));
    }
    else{ // check for index variable
        if (strcmp(attributeName, "index")==0){
	    // find environment node, ask index of this node
	    cl_GenNode* environment = (cl_GenNode *)scopedNames->FindObject("env");
	    if (environment){
    		sprintf(outputString, "%d",environment->GetPointerCount(this));
		return  outputString;
	    };
	};
    };
    if (c)
      return c->GetValue(attributeName);
    FatalError(attributeName);
    return "";
};
void  cl_GenNode::AppendStringValue(const char* stringName,const char* stringValue){
  cl_GenString* found = (cl_GenString *)strings.FindObject(stringName);
  if (found){
    found->Concat(stringValue);
  }
  else {
    FatalError(stringName);
  };
};
cl_GenNode* cl_GenNode::GetPointer(const char* pointerName) const{

    if(*pointerName=='~'){ // basic node pointers
      if (strcmp(pointerName,"~class")==0)
	return (cl_GenNode*)c;
      /*
      else if (strcmp(pointerName, "~parent")==0)
	return p;
	*/
    }
    else {// user defined pointers
      void *found = pointers.FindObject(pointerName);
      if (found){
	return ((cl_GenNode *)found);
      }
    }
    if (c)
      return c->GetPointer(pointerName);
    PRINT ("in cl_GenNode::GetPointer : cannot find " << pointerName);
    FatalError(pointerName);
    return 0;
};
void* cl_GenNode::GetVoidPointer(const char* pointerName) const{

  return pointers.FindObject(pointerName);

};

void cl_GenNode::MovePointer(const char* toPName, const char* fromPName,cl_GenNode* fromNode) {
  cl_GenNode *pointer= ((cl_GenNode *)fromNode->pointers.ExtractObject(fromPName));
  if (pointer){ // remove it from node, add it to this
    pointers.Add(toPName,(void*)pointer);
  };
};
cl_GenNode* cl_GenNode::ExtractPointer(const char* pointerName) {
  return ((cl_GenNode *)pointers.ExtractObject(pointerName));
};

const char* cl_GenNode::BuildScopedName() {
    if (scopedNames==0)
	scopedNames= new cl_GenGlossary(FALSE,TRUE);
    if (scopedName.StringSize()==0){
	//Build scoped name
	if (p) {
	    scopedName = p->ScopedName();
	    if (scopedName.StringSize())
    		scopedName.Concat("::");
	};
	if (GetValue("name"))
	    scopedName.Concat(GetValue("name"));
	// Add it into GenNode glossary
	ulong idx=scopedNames->Add(scopedName, (void *)this);
	PRINT(" cl_GenNode::BuildScopedName() : adding node " << scopedName  << (long)this << " at "<< idx );
    };
    return scopedName;
};

void cl_GenNode::FatalError(const char* name) const{
    if (exitOnFatal){
	/* This will occurs when a node cannot resolve
	* a condition, iterator, variable or pointer name.
	*/
      std::cout << "WALK runtime error: Cannot resolve node.attribute name:\"" << scopedName << "." << name  <<"\"" << std::endl;
	Show(FALSE);
	//	cl_GenStatement::WriteSourceLine();
	exit(-1);
    };
};

void cl_GenNode::FatalStaticError(const char* name) {
    /* This will occurs when a node cannot be found
     */
  std::cout <<"WALK runtime error: Cannot resolve node name:\"" << name <<  "\"" << std::endl;
    //    cl_GenStatement::WriteSourceLine();
    exit(-1);
};

void cl_GenNode::FatalGenericError(const char* what, const char* name) {
  std::cout << "WALK runtime error: " << what << ":\"" << name <<  "\"" << std::endl;
    //    cl_GenStatement::WriteSourceLine();
    exit(-1);
};
cl_GenNodeClass* cl_GenNode::DefineClass(const char* className, cl_GenGlossary& methods){
    newClass = new cl_GenNodeClass(className,methods);
    return newClass;
};
void cl_GenNode::EndDefineClass(){
  if (newClass){
    classes.Add(newClass->name,newClass);
  }
  else
    FatalStaticError("EndDefineClass without Define");

  newClass=NULL;
};
const cl_GenNode* cl_GenNode::NewObject(const cl_GenNode* p,const char* className, const char* objectName){
  /*
    PRINT("Declaring a WALK object " <<  ":\"" << objectName <<  "\"" << " of class:\"" << className << "\"");
    if (strlen(className)){
      newClass = ((cl_GenNodeClass*)classes.FindObject(className));
      if (newClass==0)
	FatalGenericError("cannot find class",className);
    }
    const cl_GenNode* newNode = new cl_GenNode(p, className, objectName);
    newClass=NULL;
    return newNode;
    */
  return 0;
};

void* cl_GenNode::GetMethod(const char* methodName){
    newClass = ((cl_GenNodeClass*)classes.FindObject(className));
    if (newClass==0)
      FatalGenericError("cannot find class",className);
    return ((void*)newClass->FindMethod(methodName));
};
const char* cl_GenNode::ClassForContainer(const char* container){
    newClass = ((cl_GenNodeClass*)classes.FindObject(className));
    if (newClass)
      return (newClass->FindClassForContainer(container));
    else
      return "";
};


std::ostream& cl_GenNode::write (std::ostream& os) const {
  cl_GenString::PutShort(os,NodeId());
  // write conditions
  cl_GenIterator iterB(conditions.Repository());
  cl_GenString::PutShort(os,(conditions.Repository().NbItems()));
  while (iterB.Next()){
    cl_GenGlossaryEntry* curr = (cl_GenGlossaryEntry*)iterB.Current();
    curr->name.write(os);
    os.put(*(char*)curr->object);
  };
  // write strings
  cl_GenIterator iterS(strings.Repository());
  cl_GenString::PutShort(os,(strings.Repository().NbItems()));
  while (iterS.Next()){
    cl_GenGlossaryEntry* curr = (cl_GenGlossaryEntry*)iterS.Current();
    curr->name.write(os);
    ((cl_GenString*)curr->object)->write(os);
  };
  // write containers
  cl_GenIterator iterC(containers.Repository());
  cl_GenString::PutShort(os,containers.Repository().NbItems());
  while (iterC.Next()){
    cl_GenGlossaryEntry* curr = (cl_GenGlossaryEntry*)iterC.Current();
    curr->name.write(os);
    cl_GenArray* currArray= (cl_GenArray*)curr->object;
    cl_GenIterator iterSub(*currArray);
    cl_GenString::PutShort(os,(currArray->NbItems()));
    while (iterSub.Next())
      cl_GenString::PutShort(os,(((cl_GenNode*)iterSub.Current())->NodeId()));
  };
  // write references
  cl_GenIterator iterR(pointers.Repository());
  cl_GenString::PutShort(os,pointers.Repository().NbItems());
  while (iterR.Next()){
    cl_GenGlossaryEntry* curr = (cl_GenGlossaryEntry*)iterR.Current();
    curr->name.write(os);
    cl_GenString::PutShort(os,(((cl_GenNode*)curr->object)->NodeId()));
  };
  cl_GenString::PutShort(os,NodeId());

  return os;
};

std::istream& cl_GenNode::read  (std::istream& is){
  // read conditions
  cl_GenString s;
  ulong nbItems;
  ulong id = cl_GenString::GetShort(is);
  nbItems = cl_GenString::GetShort(is);
  for (ulong i=0;i< nbItems;i++){
    s.read(is);
    short value = is.get();
    AddCondition(s,value);
  };
  // ....to be defined
  id = cl_GenString::GetShort(is);
  return is;
};
void cl_GenNode::Save(std::ostream& os,const cl_GenNode* root){
  std::cout << "cl_GenNode::Save " << allNodes.NbItems() << " nodes" << std::endl;
  cl_GenIterator iter(allNodes);
  cl_GenString::PutShort(os,root->NodeId());
  cl_GenString::PutShort(os,allNodes.NbItems());
  while (iter.Next()){
    ((cl_GenNode*)iter.Current())->write(os);
  };
  cl_GenString::PutShort(os,root->NodeId());
}
void cl_GenNode::Show(boolean recurse) const{
  Indent();
  std::cout << indentString << GetValue("isA") << " at " << (void*)this << std::endl;

  // write conditions
  cl_GenIterator iterB(conditions.Repository());
  std::cout <<    indentString << conditions.Repository().NbItems() << " conditions:" ;
  while (iterB.Next()){
    cl_GenGlossaryEntry* curr = (cl_GenGlossaryEntry*)iterB.Current();
    std::cout << curr->name << ",";
  };
  std::cout << std::endl;

  // write strings
  cl_GenIterator iterS(strings.Repository());
  std::cout <<  indentString << strings.Repository().NbItems() << " strings:";
  while (iterS.Next()){
    cl_GenGlossaryEntry* curr = (cl_GenGlossaryEntry*)iterS.Current();
    std::cout << curr->name << ",";
  };
  std::cout << std::endl;
  // write references
  cl_GenIterator iterR(pointers.Repository());
  std::cout << indentString << pointers.Repository().NbItems() << " references:";
  while (iterR.Next()){
    cl_GenGlossaryEntry* curr = (cl_GenGlossaryEntry*)iterR.Current();
    std::cout << curr->name << "=" << (void*)curr->object << ",";
  };
  std::cout << std::endl;

  // write containers
  cl_GenIterator iterC(containers.Repository());
  std::cout <<  indentString <<  containers.Repository().NbItems() << " containers:" << std::endl ;
  while (iterC.Next()){
    cl_GenGlossaryEntry* curr = (cl_GenGlossaryEntry*)iterC.Current();
    cl_GenArray* array=(cl_GenArray *)curr->object;
    cl_GenIterator iterN (*array);
    std::cout << indentString << curr->name << " with " << array->NbItems() << " nodes:" << std::endl;
    if (recurse){// go down into sub-nodes containers
      while (iterN.Next()){
	cl_GenNode* currNode = (cl_GenNode*)iterN.Current();
	currNode->Show();
      };
    };
  };
  std::cout << std::endl;

  UnIndent();
};


void cl_GenNode::PlugIn(void*& context){
    void* plugIn = GetMethod("PlugIn");
    if (plugIn==0)
      FatalGenericError("cannot find \"PlugIn\" method",className);
    else {// execute plugIn on this node
      plugInFunction fp = ((plugInFunction)plugIn);
      fp(*this, context);
    };
};
void cl_GenNode::AddPlugIn(const char* className, plugInFunction pif){
  cl_GenGlossary* methods= new cl_GenGlossary;
  newClass = new cl_GenNodeClass (className,*methods);

  if (newClass==0)
    FatalGenericError("cannot create class",className);

  classes.Add(className,newClass);
  newClass->AddMethod("PlugIn",(void*)pif);
};
