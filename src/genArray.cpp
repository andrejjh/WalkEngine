//
//  $Id: genArray.cc,v 1.0 1996/06/20 13:30:00 andre Exp $
//

#include <memory.h>
#include <genArray.h>

typedef void* voidPtr;


// cl_GenArray class
// -----------------
//const ulong cl_GenArray::FREE_ITEM (0);
const ulong cl_GenArray::NOT_IN_ARRAY (ULONG_MAX);
const ulong cl_GenArray::MAX_ARRAY_SIZE (ULONG_MAX -1);

cl_GenArray::cl_GenArray(boolean owns)
    :  ownsItems(owns),
    items(0), nbItems(0), maxSize(0), idx(0){
 //   Grow();
};

cl_GenArray::~cl_GenArray(){
    Clear();
};

void cl_GenArray::Clear(){
    if (ownsItems){
	for (idx=0;idx<maxSize;idx++)
	    if (items[idx]) delete items[idx];
    };
    if (items) delete items;
    items=0;
    maxSize=0;
    nbItems=0;
};

void cl_GenArray::Grow(){
    ulong newSize;
    if (maxSize==0)
	newSize =1;
    else
	newSize = 2* maxSize;
    if (newSize <= MAX_ARRAY_SIZE){
        ulong allocSize= sizeof (void *) * (newSize);
	void** newItems= new voidPtr[newSize];
	if (newItems){
	    memset(newItems, 0, allocSize);
	    if (items) {
		     memcpy(newItems, items, sizeof (void*) * maxSize);
		     delete items;
	    }
	    items = newItems;
	    maxSize=newSize;
	};
    };
};

ulong cl_GenArray::Add(void* anItem){
    if (nbItems == maxSize)
	Grow();
    idx = Find(NULL);
    if (idx != NOT_IN_ARRAY){
        items[idx]=anItem;
	nbItems++;
    };
    return idx;
};

ulong cl_GenArray::AddAt(void* anItem, ulong at){
    // Grow until at is a valid index
    while (at >= maxSize)
	Grow();
    if (items[at]== NULL){
        items[at]=anItem;
	nbItems++;
    };
    return idx;
};

ulong cl_GenArray::Remove(void* anItem){
    idx=Find(anItem);
    if (idx != NOT_IN_ARRAY){
    	if ((ownsItems) && (items[idx]))
 		delete items[idx];
        items[idx]=NULL;
	nbItems--;
    };
    return idx;
};

ulong cl_GenArray::RemoveAt(ulong at){
    idx=at;
    if (idx < maxSize){
    	if ((ownsItems) && (items[idx]))
 		delete items[idx];
        items[idx]=NULL;
	nbItems--;
    };
    return idx;
};

ulong cl_GenArray::Find(void* anItem) const {
    for (ulong i=0;i<maxSize;i++){
	if (items[i]== anItem)
		return i; // return item index
    };
    return NOT_IN_ARRAY;
};
void cl_GenArray::Swap(const ulong idx){
  void* temp;
  if ((idx > 0) && (idx < maxSize)){
    temp =items[idx-1];
    items[idx-1]=items[idx];
    items[idx]=temp;
  };
};

// cl_GenIterator class
// --------------------
cl_GenIterator::cl_GenIterator(const cl_GenArray &anArray)
    : theArray(anArray),  index(0){
};
