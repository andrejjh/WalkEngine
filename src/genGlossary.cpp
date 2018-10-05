//
//  $Id: genGlossary.cc,v 1.0 1996/05/06 06:41:35 andre Exp $
//

#include <string.h>
#include <genGlossary.h>


cl_GenGlossaryEntry::cl_GenGlossaryEntry(const char* aName,  void * anObject)
    : name (aName),  object(anObject), count(0){
};

cl_GenGlossary::cl_GenGlossary(boolean owns,boolean l)
    : ownsItems(owns),large(l){
}
cl_GenGlossary::~cl_GenGlossary(){
    if (ownsItems) { // delete object stored in glossary entries
	cl_GenIterator iter(repository);
	cl_GenGlossaryEntry *curr;
	while (curr=((cl_GenGlossaryEntry *)iter.Next()) ){
	    if (curr->object) delete (curr->object);
	}
    }
}


unsigned long cl_GenGlossary::Add(const char* aName, void* anObject){
    unsigned long result = cl_GenArray::NOT_IN_ARRAY;
    unsigned long nameLength = strlen(aName);
    cl_GenGlossaryEntry *found = FindEntry(aName);
    if (found){ // replace existing entry in glossary
	if ( ownsItems && (found->object) && (anObject != found->object))
	    delete found->object;
	found->object=anObject;
	found->count++;
	result= result -1; // = re-use
    }
    else {  // add a new entry in repository
    	cl_GenGlossaryEntry* newEntry = new cl_GenGlossaryEntry(aName, anObject);
    	result = repository.Add(newEntry);
	if (large) { // add in fastAccessTable
	  cl_GenArray* a= ((cl_GenArray*)fastAccessTable[nameLength]);
	  if (a==NULL) {
            a= new cl_GenArray(FALSE);
	    fastAccessTable.AddAt(a,nameLength);
	  }
	  a->Add(newEntry);
	}
    };
    return result;
};

void cl_GenGlossary::Remove(const char* aName){
    unsigned short nameLen= strlen(aName);
    cl_GenGlossaryEntry *found = FindEntry(aName);
    if (found){
	if ( ownsItems && (found->object))
	    delete found->object;
        // remove from repository
	repository.Remove(found);
        if (large) {// remove from fastAccessTable
          cl_GenArray* a= ((cl_GenArray*)fastAccessTable[nameLen]);
          if (a)
	    a->Remove(found);
	}
   }
};

cl_GenGlossaryEntry *cl_GenGlossary::FindEntry(const char* aName) const{
    unsigned short nameLen= strlen(aName);
    if (large) { // use fastAccessTable
      cl_GenArray* a= ((cl_GenArray*)fastAccessTable[nameLen]);
      if (a) {
        cl_GenIterator iter(*a);
        cl_GenGlossaryEntry *curr;
        iter.Reset();
        while (curr=((cl_GenGlossaryEntry *)iter.Next()) ){
	    if (strncmp(curr->name, aName, nameLen)==0)
	        return curr;
        }
      }
    }
    else { // use repository
      cl_GenIterator iter(repository);
      cl_GenGlossaryEntry *curr;
      iter.Reset();
      while (curr=((cl_GenGlossaryEntry *)iter.Next()) ){
	if ( (nameLen == curr->name.StringSize())
	     && (strncmp(curr->name, aName, nameLen)==0) )
	    return curr;
      }
    }
    return 0;
};



void* cl_GenGlossary::FindObject(const char* aName)const{
    cl_GenGlossaryEntry *found = FindEntry(aName);
    if (found){
	if (!found->object)
		std::cout << " cl_GenGlossary::FindObject name=" << aName << "is NULL" <<std::endl;
	return (found->object);
	}
    else
	return 0;
};
void* cl_GenGlossary::ExtractObject(const char* aName) {
    cl_GenGlossaryEntry *found = FindEntry(aName);
    if (found){ // remove the glossary entry and return the object
      // it is the responsability of the caller to delete it !
      void* theObject = found->object;
      repository.Remove(found);
      return (theObject);
    }
    else
      return 0;
};

unsigned short cl_GenGlossary::FindObjectCount(void* anObject)const{
    cl_GenIterator iter(repository);
    cl_GenGlossaryEntry *curr;

    iter.Reset();
    while (curr=((cl_GenGlossaryEntry *)iter.Next()) ){
	if (curr->object==anObject)
	    return curr->count;
    }
    return 0;
};

const char* cl_GenGlossary::FindObjectName(void* anObject)const{
    cl_GenIterator iter(repository);
    cl_GenGlossaryEntry *curr;

    iter.Reset();
    while (curr=((cl_GenGlossaryEntry *)iter.Next()) ){
	if (curr->object==anObject)
	    return curr->name;
    }
    return 0;
};
