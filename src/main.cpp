#include <istd::ostream>
#include <fstream>

#include <genString.h>
#include <genArray.h>
#include <genStreamable.h>
#include <genNode.h>

using namespace std;

class cl_A : public cl_GenStreamable{
public :
  char dm;
  const char* ClassName() const {return"A class";};
  std::ostream& write(std::ostream& os) {os << dm; return os;};
  std::istream& read(std::istream& is) {is >> dm; return is;};

};
class cl_B : public cl_GenStreamable{
public :
  char dm1,dm2;
  const char* ClassName() const {return"B class";};
  std::ostream& write(std::ostream& os) {os << dm1 << dm2; return os;};
  std::istream& read(std::istream& is) {is >> dm1 >> dm2; return is;};

};
class cl_C : public cl_GenStreamable{
public :
  char dm1,dm2,dm3;
  const char* ClassName() const {return"C class";};
  std::ostream& write(std::ostream& os) {os << dm1 << dm2 << dm3; return os;};
  std::istream& read(std::istream& is) {is >> dm1 >> dm2 >> dm3; return is;};

};
class cl_D : public cl_GenStreamable{
public :
  cl_A a;
  cl_B b;
  cl_C c;

  const char* ClassName() const {return"D class";};
  std::ostream& write(std::ostream& os) {os << a << b << c; return os;};
  std::istream& read(std::istream& is) {is >> a >> b >> c; return is;};

};

class cl_E : public cl_GenStreamable{
public :
  cl_GenArray entries;
  cl_E():entries(TRUE){};
  void Add(cl_GenStreamable* s){entries.Add((void*)s);};
  const char* ClassName() const {return"E class";};
  std::ostream& write(std::ostream& os);
  std::istream& read(std::istream& is); 

};
std::ostream& cl_E::write(std::ostream& os){
  cl_GenStreamable* entry=NULL;
  cl_GenIterator iter(entries);
  os.put(entries.NbItems());
  while (entry=((cl_GenStreamable*)iter.Next())){
    os << *entry;
  };
  return os;

};
std::istream& cl_E::read(std::istream& is){
  cl_GenStreamable* entry=NULL;
  unsigned long nbEntries = is.get();
  entries.Clear();
  for (int i=0;i<nbEntries;i++){
    entry =cl_GenStreamable::Create(is);
    if (entry)
      Add(entry);
  };
  return is;

};
cl_GenStreamable* ABCCreator(const char* className){
  switch (*className){
  case ('A') :return (cl_GenStreamable*)new cl_A;
  case ('B') :return (cl_GenStreamable*)new cl_B;
  case ('C') :return (cl_GenStreamable*)new cl_C;
  case ('D') :return (cl_GenStreamable*)new cl_D;
  case ('E') :return (cl_GenStreamable*)new cl_E;
  };
  return NULL;
};

int test0(){

	cl_GenArray myArray(true);
	cl_GenString* s1=new cl_GenString("Hello World!");
	cl_GenString* s2=new cl_GenString("Walk is coming back!");
	myArray.Add(s1);
	myArray.Add(s2);

//	std::std::cout << (const char*)*s1;
//	std::std::cout << (const char*)*s2;
	
	std::std::cout << (const char*)*(cl_GenString*)myArray[0];
	std::std::cout << (const char*)*(cl_GenString*)myArray[1];

//    std::std::cout << "Hello, World!\n";

}
int test1(){
  //write objects in 'test.out' file
  
  cl_A* a= new cl_A;
  cl_B* b= new cl_B;
  cl_C* c= new cl_C;
  cl_D* d= new cl_D;
  cl_E* e= new cl_E;

  ofstream o("test1.out");
  a->dm='1';
  b->dm1='1';b->dm2='2';
  c->dm1='1';c->dm2='2';c->dm3='3';
  d->a.dm='1';
  d->b.dm1='2';d->b.dm2='3';
  d->c.dm1='4';d->c.dm2='5';d->c.dm3='6';

  e->Add(a);
  e->Add(b);
  e->Add(c);
  e->Add(d);
  cl_GenStreamable::WriteClassTable(o);
  o << *e ;
  delete e;


}
int test2(){

  ifstream i("test1.out");
  cl_GenStreamable::ReadClassTable(i);
  const char* className = cl_GenStreamable::CurrentClassName(i);  
  cl_E* e =(cl_E*)cl_GenStreamable::Create(i);

  ofstream o("test2.out");
  cl_GenStreamable::WriteClassTable(o);
  o << *e ;
 
 delete e;
}



int test3(){
/*
    cl_GenSymbolTable st;
    cl_GenGlossary g1(st,FALSE),g2(st,FALSE),g3(st,FALSE);
    cl_GenString s1,s2,s3,s4,s5;
    short  i1,i2,i3,i4,i5;
    i1=1;
    i2=2;
    i3=3;
    i4=4;
    i5=5;

    s1 = "one";
    s2 = "two";
    s3 = "three";
    s4 = "four";
    s5 = "five";
    g1.Add(s1,&i1);
    g1.Add(s2,&i1);
    g2.Add(s2,&i2);
    g2.Add(s3,&i3);
    g3.Add(s4,&i4);
    g3.Add(s5,&i5);

    const char* c;
    c=st[0];
    c= st[1];
    c= st[2];
    c=st[3];
    c= st[4];
    c= st[5];
   short* p;
    p = (short*)g1.FindObject(s1);
    p = (short*)g1.FindObject(s2);
    p = (short*)g2.FindObject(s2);
    p = (short*)g3.FindObject(s5);

    ofstream o("test3.out");
    st.write(o);
    g1.write(o);
    g2.write(o);
    g3.write(o);
*/
}
int main (int argc, char * const argv[]) {
    // insert code here...
  cl_GenStreamable::RegisterClass("A class", ABCCreator);
  cl_GenStreamable::RegisterClass("B class", ABCCreator);
  cl_GenStreamable::RegisterClass("C class", ABCCreator);
  cl_GenStreamable::RegisterClass("D class", ABCCreator);
  cl_GenStreamable::RegisterClass("E class", ABCCreator);
//	test1();
	test2();
//	test3();


    return 0;
}
