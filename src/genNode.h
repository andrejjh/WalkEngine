/*
 *  $Id: genNode.h,v 1.0 1996/05/02 07:20:50 philip Exp $
 */

#ifndef _GENNODE_H_
#define _GENNODE_H_
#include <string.h>
#include <genArray.h>
#include <genString.h>
#include <genGlossary.h>



class cl_GenNode;

typedef int (*plugInFunction) (cl_GenNode&,void*& context) ;


class cl_GenDataMember{
  public :

  enum dataMemberType {DM_SIMPLE, DM_CONTAINER, DM_REFERENCE};
  private :
  dataMemberType dmt;
  cl_GenString name,type;

  public :
    cl_GenDataMember(dataMemberType aDMT,const char* aType, const char* aName);
  dataMemberType DMType(){return dmt;};
  const char* Name(){return name;};
  const char* Type(){return type;};
};

class cl_GenNodeClass{
   cl_GenArray dataMembers;
   cl_GenGlossary& methods;
public :
   cl_GenString name;
   cl_GenNodeClass(const char* aName, cl_GenGlossary& theMethods);
   void AddMember(cl_GenDataMember::dataMemberType aDMT,const char* aType, const char* aName){
     dataMembers.Add((void*)new cl_GenDataMember(aDMT,aType,aName));};
   void AddNodeDataMembers (cl_GenNode& aNode);
   void* FindMethod(const char* name);
   void  AddMethod(const char* name,void* method);
   const char* FindClassForContainer(const char* container);
};

class cl_GenNode {
private :

  cl_GenGlossary conditions, containers, pointers, strings;
  const cl_GenNode* p;	// parent node
  const cl_GenNode* c;	// class node
  cl_GenString className;
  cl_GenString scopedName;	// node scoped name
  short nodeId;
  static short nextId;
  static cl_GenArray allNodes;
  static char indentString[85];
  static char outputString[81];
  short exitOnFatal;

protected :
  static cl_GenGlossary* scopedNames;
  static cl_GenGlossary classes;
public :

  static cl_GenNodeClass* newClass;

  enum {NONE, BOOLEAN, CONTAINER, STRING, POINTER};
  cl_GenNode (const cl_GenNode* _p=NULL, const char* aClassName=NULL);
  ~cl_GenNode();
  short NodeId()const {return nodeId;};
  const cl_GenNode *P() const;
  static const char* IndentString(){ return indentString; }
  static void Indent();
  static void UnIndent();
  static boolean verbose;
  static cl_GenNode* Find(const char* scopedName);
  short GetCondition(const char* conditionName) const;
  cl_GenIterator* GetIterator(const char* containerName) const;
  const char* GetValue(const char* attributeName) const;
  cl_GenNode* GetPointer(const char* pointerName) const;
  void* GetVoidPointer(const char* pointerName) const;

  short IsA() const{return 0;};
  const char* IsOfType() const{return className;};
  const char* ScopedName() const;
  const char* BuildScopedName();
  const char* ClassForContainer(const char* container);
  void FatalError(const char *) const;
  static void FatalStaticError(const char *name);
  static void FatalGenericError(const char* what, const char *name);
  void AddCondition(const char* conditionName, short condValue);
  void AddContainer(const char* containerName);
  void AddInContainer(const char* containerName, const cl_GenNode *aNode);
  void AddUniqueInContainer(const char* containerName, const cl_GenNode *aNode);
  void RemoveFromContainer(const char* containerName, const cl_GenNode *aNode);
  void ClearContainer(const char* containerName);
  void MoveContainer(const char* toCName, const char* fromCName, cl_GenNode* fromNode);
  cl_GenArray* ExtractContainer(const char* containerName);
  void AddStringValue(const char* stringName, const char* stringValue);
  void AppendStringValue(const char* stringName, const char* stringValue);
  void AddPointer(const char* pointerName, const cl_GenNode* pointerValue);
  void AddVoidPointer(const char*pointerName, void*);
  void RemovePointer(const char* pointerName);
  void MovePointer(const char* toPName,const char* fromPName,cl_GenNode* fromNode);
  cl_GenNode* ExtractPointer(const char* pointerName);
  void* GetMethod(const char* methodName);
  short GetPointerCount(const cl_GenNode* pointer);
  short HasAttribute(const char* attribName) const;
  static cl_GenNodeClass* DefineClass(const char* className, cl_GenGlossary& methods);
  static void EndDefineClass();
  static const cl_GenNode* NewObject(const cl_GenNode*p,const char* className, const char* objectName);
  static cl_GenNodeClass* FindClass(const char* className){return ((cl_GenNodeClass*)classes.FindObject(className));};
  std::ostream& write(std::ostream& os)const;
  std::istream& read(std::istream& is);
  static void Save(std::ostream& os, const cl_GenNode* root);
  static cl_GenNode* Load(std::istream& is);
  void Show(boolean recurse=TRUE)const;
  void PlugIn(void* &context);
  static void AddPlugIn(const char* className, plugInFunction pif);
  const cl_GenArray& Containers() const;
  const cl_GenArray& Pointers() const;
};

inline cl_GenNode* cl_GenNode::Find(const char* scopedName){
    if (scopedNames)
	    return ((cl_GenNode*)scopedNames->FindObject(scopedName));
    else
	return 0;
};
inline const cl_GenNode* cl_GenNode::P() const {return p;};
inline const char* cl_GenNode::ScopedName() const {return scopedName;};

inline void cl_GenNode::AddCondition(const char* conditionName, short conditionValue) {
    conditions.Add(conditionName, new short(conditionValue));
};
inline void cl_GenNode::AddContainer(const char* containerName) {
    // new containers do not owns their elements
    containers.Add(containerName, (void*) new cl_GenArray(0));
};
inline void cl_GenNode::AddInContainer(const char* containerName,  const cl_GenNode *aNode) {
    cl_GenArray *container= ((cl_GenArray *)containers.FindObject(containerName));
    if (!container){ // create it now
      container = new cl_GenArray(0);
      containers.Add(containerName,(void*)container);
    };
    container->Add((void*) aNode);
};
inline void cl_GenNode::AddUniqueInContainer(const char* containerName,  const cl_GenNode *aNode) {
    cl_GenArray *container= ((cl_GenArray *)containers.FindObject(containerName));
    if (!container){ // create it now
      container = new cl_GenArray(0);
      containers.Add(containerName,(void*)container);
    }
    else {// check node in container, Add it
      if (container->Find((void*) aNode) == cl_GenArray::NOT_IN_ARRAY)
	container->Add((void*) aNode);
    };
};
inline void cl_GenNode::RemoveFromContainer(const char* containerName,  const cl_GenNode *aNode) {
    cl_GenArray *container= ((cl_GenArray *)containers.FindObject(containerName));
    if (container)
	container->Remove((void*) aNode);
};

inline void cl_GenNode::ClearContainer(const char* containerName) {
    cl_GenArray *container= ((cl_GenArray *)containers.FindObject(containerName));
    if (container)
	container->Clear();
};
inline void cl_GenNode::AddStringValue(const char* stringName,  const char* stringValue) {
    strings.Add(stringName, new cl_GenString(stringValue));
};
inline void cl_GenNode::AddPointer(const char* pointerName,  const cl_GenNode* pointerValue) {
  if(*pointerName=='~'){ // basic node pointers
    if (strcmp(pointerName,"~class")==0)
      c= pointerValue;
    /*    else if (strcmp(pointerName, "~parent")==0)
      p= pointerValue;
      */
  }
  else  // user defined pointers
    pointers.Add(pointerName, (void *)pointerValue);
};
inline void cl_GenNode::AddVoidPointer(const char* pointerName,  void* pointerValue) {
    pointers.Add(pointerName,pointerValue);
};

inline void cl_GenNode::RemovePointer(const char* pointerName) {
    pointers.Remove(pointerName);
};
inline short cl_GenNode::GetPointerCount(const cl_GenNode* pointer){
    return pointers.FindObjectCount((void *)pointer);
};

inline const cl_GenArray& cl_GenNode::Containers()const {
  return containers.Repository();};

inline const cl_GenArray& cl_GenNode::Pointers()const {
  return pointers.Repository();};




#endif // _GENNODE_H_
