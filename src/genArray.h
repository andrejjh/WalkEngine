/*
 *  $Id: genArray.h,v 1.0 1996/06/011 09:30:00 andre Exp $
 */

#ifndef _GENARRAY_H_
#define _GENARRAY_H_
typedef unsigned long ulong;
typedef unsigned char boolean;
#define TRUE 0xFF
#define FALSE 0x00

#ifndef ULONG_MAX
    #define ULONG_MAX 1234567U
//    #define ULONG_MAX 4294967295U
    #endif

// InDirect Array :
//	dynamically resizable, hold pointer of items

class cl_GenArray {
private:
    boolean ownsItems;
    ulong nbItems, maxSize, idx;
    void** items;
    void Grow();

public:
//	static const ulong FREE_ITEM;
    static const ulong NOT_IN_ARRAY;
    static const ulong MAX_ARRAY_SIZE;
    enum {DefaultDelete, Delete, NoDelete};
    cl_GenArray( boolean owns=TRUE);
    ~cl_GenArray();
    void* operator[](const ulong) const;
    ulong Add(void *anItem);
    ulong AddAt(void *anItem, ulong at);
    ulong Remove(void *anItem);
    ulong RemoveAt (ulong at);
    ulong NbItems() const;
    ulong MaxSize() const;
    void Clear();
    ulong Find(void *anItem) const;
    void Swap(const ulong idx);
};
inline ulong cl_GenArray::NbItems() const{
    return nbItems;
};
inline ulong cl_GenArray::MaxSize() const{
    return maxSize;
};
inline void* cl_GenArray::operator[](const ulong idx) const{
    if (idx < maxSize)
	return items[idx];
    else
	return 0;
};

class cl_GenIterator {
private:
    const cl_GenArray& theArray;
    ulong index; // = array index +1;
public:
    cl_GenIterator(const cl_GenArray &anArray);
    void* Current();
    void* Next();
    void Reset();
    ulong CurrentIndex() const {return index-1;};
    short IsFirst(); // return -1 if current item is the first one, 0 otherwise
    short IsLast(); // return -1 if current item is the last one, 0 otherwise
};
inline void *cl_GenIterator::Current(){
  if (index==0)
	  return NULL;
  else
    return	theArray[index-1];
};

inline void* cl_GenIterator::Next(){
  do {
    index++;
  }while ((index <= theArray.MaxSize()) && (theArray[index-1]==NULL));
  return theArray[index-1];
};

inline void cl_GenIterator::Reset(){
  index=0;
};

inline short cl_GenIterator::IsFirst(){
  return (index == 1);
};

inline short cl_GenIterator::IsLast(){
  return (index == theArray.NbItems());
};
#endif
