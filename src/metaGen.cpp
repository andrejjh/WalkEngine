/*
 *  $Id: metaLang.cc,v 1.0 1997/01/15 07:20:50 andre Exp $
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <genNode.h>
#include <metaLang.h>
#include <metaGen.h>


cl_GenString indent = "";
extern short verbose;
char buffer[80]; // temporary buffer for building ascii from integer value

void EnterRule(cl_MRule& aRule,void*& context, void*& subContext,const char* alias){

  cl_GenNode* contextNode =  ((cl_GenNode*)context);
  const char* contextNodeType=NULL;
  cl_MRule* parentRule=NULL;
  cl_GenNode* thisNode = NULL;

  indent.Add(' ');

  if ( (aRule.IsA()== M_SEQUENCE)
       || (aRule.IsA()==M_MANYOF)
       || (aRule.IsA()==M_ONEOF)  ){
    // create a new node
    thisNode =  new cl_GenNode(contextNode,aRule.Name());
    PRINT( indent << "creating a " << alias << (void*) thisNode);
    thisNode->AddVoidPointer("@parentRule",&aRule);
    thisNode->AddStringValue("isA",aRule.Name());
    if (contextNode){ // search contextNode (parent) rule
      parentRule = ((cl_MRule*)contextNode->GetVoidPointer("@parentRule"));
      contextNodeType = contextNode->GetValue("isA");
    };
    if (parentRule) {
      switch (aRule.IsA()){
      case (M_SEQUENCE):// add this (sequence) node in context node
	switch (parentRule->IsA()){
	case(M_SEQUENCE) :
	  PRINT( indent << "adding "  << alias << " (=" << (void*)thisNode << ") as a reference in " << contextNodeType << " (=" << (void*)contextNode << ")");
	  contextNode->AddPointer(alias,thisNode);
	  break;
	case(M_MANYOF) :
	  PRINT(indent << "adding "  << alias << " (=" << (void*)thisNode << ") as a container in " << contextNodeType << " (=" << (void*)contextNode << ")");
	  contextNode->AddInContainer("@container",thisNode);
	  break;
	case(M_ONEOF) :
	  PRINT(indent << "adding "  << alias << " (=" << (void*)thisNode << ") as a oneOf reference in " << contextNodeType << " (=" << (void*)contextNode << ")");
	  contextNode->AddPointer("@pointer",thisNode);
	  break;
	default :
	  std::cout << indent << ">>aaaarghhh" << parentRule->IsA() << std::endl;
	  exit(-1);
	  break;
	};
	break;
      case (M_MANYOF) : // Create an empty container in this Node
	thisNode->AddContainer("@container");
	break;
      case (M_ONEOF) : // Add a (NULL) pointer to this Node
	thisNode->AddPointer("@pointer", NULL);
	break;
      };

    }
  };
  subContext = (void*)thisNode;
};


void ExitRule(cl_MRule& aRule,void*& context, void*& subContext,const char* alias){
  cl_GenNode* contextNode =  ((cl_GenNode*)context);
  cl_GenNode* thisNode =((cl_GenNode*)subContext);
  cl_GenArray* arrayPtr=NULL;

  indent.DeleteLast();
  switch (aRule.IsA()){
  case(M_SEQUENCE) :
    break;
  case(M_MANYOF) :
    if (contextNode && thisNode){
      PRINT(indent << "Collapsing a " << alias << " manyOf :" );
      PRINT(indent << "thisNode (=" <<(void*)thisNode << ") " << thisNode->GetValue("isA"));
      PRINT(indent << "contextNode (=" <<(void*)contextNode << ") " << contextNode->GetValue("isA"));
      cl_MRule* parentRule = ((cl_MRule*)contextNode->GetVoidPointer("@parentRule"));
      if (parentRule) {
	switch (parentRule->IsA()){
	case(M_SEQUENCE) :
	  PRINT("(S)context.alias = this.@container");
	  contextNode->MoveContainer(alias,"@container",thisNode);
	  delete thisNode; // delete collapsed node
	  break;
	case(M_MANYOF) :
	  PRINT(" add all nodes from this.@container to context.@container");
	  arrayPtr=thisNode->ExtractContainer("@container");
	if (arrayPtr)	  {
	  cl_GenIterator arrayIter(*arrayPtr);
	  cl_GenNode* nodePtr;
	  while (nodePtr= ((cl_GenNode*)arrayIter.Next())){
	    contextNode->AddInContainer("@container",nodePtr);
	  };
	  delete arrayPtr;
	};
	  delete thisNode; // delete collapsed node
	  break;
	case (M_ONEOF) :
	  PRINT("parent is a oneOf do not collapse");
	  contextNode->AddPointer("@pointer",thisNode);
	  PRINT(indent << "No Collapse for OneOf to ManyOf");
	  break;
	};
      };
    };
    break;
  case(M_ONEOF) :
    if (contextNode && thisNode){
      PRINT (indent << "Collapsing a " << alias << " oneOf." );
      PRINT (indent << "thisNode (=" <<(void*)thisNode << ") " << thisNode->GetValue("isA"));
      PRINT (indent << "contextNode (=" <<(void*)contextNode << ") " << contextNode->GetValue("isA"));

      cl_MRule* parentRule = ((cl_MRule*)contextNode->GetVoidPointer("@parentRule"));
      if (parentRule) {
	switch (parentRule->IsA()){
	case(M_SEQUENCE) :
	  contextNode->MovePointer(alias,"@pointer",thisNode);
	case(M_ONEOF) :
	  contextNode->MovePointer("@pointer","@pointer",thisNode);
	  break;
	case (M_MANYOF) : // parent is a manyOf addIn container
	  cl_GenNode* nodePtr=thisNode->ExtractPointer("@pointer");
	  if (nodePtr)
	    contextNode->AddInContainer("@container",nodePtr);
	  break;

	};
      };
      delete thisNode; // delete collapsed node
    };
    break;

  case(M_BOOLEAN) :
  case(M_CHAR) :
  case(M_IDENTIFIER) :
  case(M_INTEGER) :
  case(M_NULL) :
  case(M_STRING) :
  case(M_COMMENT) :
  case(M_LITERALSTRING):
  case(M_PATHNAME) :
  case(M_TOKEN) :
    if (*alias != '#'){// this build-in token has an alias -> store it
      switch (aRule.IsA()){
      case(M_BOOLEAN) :
	PRINT (indent << "adding a " << alias << " boolean.");
	contextNode->AddCondition(alias, ((cl_MBoolean*)&aRule)->Value());
	break;
      case(M_CHAR) :
	//      std::cout << indent << "adding a " << alias << " char." << std::endl;
	//      contextNode->AddString(alias, ((cl_MChar*)&aRule)->Value());
	break;
      case(M_IDENTIFIER) :
	PRINT (indent << "adding a " << alias << " identifier as a string.");
	contextNode->AddStringValue(alias, ((cl_MIdentifier*)&aRule)->Value());
	break;
      case(M_INTEGER) :
	PRINT (indent << "adding a " << alias << " integer as a string." );
	sprintf(buffer,"%d",((cl_MInteger*)&aRule)->Value());
	contextNode->AddStringValue(alias,buffer);
	break;
      case(M_NULL) :
	//      std::cout << indent << "adding a " << alias << " null." << std::endl;
	  break;
      case(M_STRING) :
	PRINT(indent << "adding a " << alias << " string.");
	contextNode->AddStringValue(alias, ((cl_MString*)&aRule)->Value());
	break;
      case(M_COMMENT) :
	PRINT(indent << "adding a " << alias << " comment as a string.");
	contextNode->AddStringValue(alias, ((cl_MComment*)&aRule)->Value());
	break;
      case(M_LITERALSTRING) :
	PRINT(indent << "adding a " << alias << " literal as a string.");
	contextNode->AddStringValue(alias, ((cl_MLiteralString*)&aRule)->Value());
	break;
      case(M_PATHNAME) :
	PRINT(indent << "adding a " << alias << " pathname as a string.");
	contextNode->AddStringValue(alias, ((cl_MPathName*)&aRule)->Value());
	break;
      case(M_TOKEN) :
	PRINT(indent << "adding a " << alias << " token as a string.");
	contextNode->AddStringValue(alias, ((cl_MToken*)&aRule)->Value());
	break;

      default :
	break;
      };
    };
    break;
  case(M_LITERAL) : // Add as a boolean if alias is not the literal value
    if ( strcmp(alias,((cl_MLiteral&)aRule).Value()) ){
      PRINT(indent << "adding a " << alias << " literal as boolean.");
      contextNode->AddCondition(alias,TRUE);
    };
    break;


  default :
    std::cout << indent << "<<aaaarghhh" << aRule.IsA() << std::endl;
    exit(-1);
    break;
  };
};
