#include <ctype.h>
#include <regex.h>
#include <stdlib.h>
#include <fstream>
#include <genString.h>
#include <genSymbolTable.h>
#include <genStreamable.h>

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
	if (r==0 && m.rm_so==0){
    std::cout << "regexec=" << (void*)m.rm_so << ",eo=" << (void*)m.rm_eo << std::endl;
		after= s + m.rm_eo;
  }
	return after;
};
void my_regfree(char* re){
	regfree((regex_t*)re);
}

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
int main (){
  const char* def="^.+";
  const char* source="abc\ndef\n\n";
  char *comp; // compiled regular expression
  char *after; // point to next unmatched character
  char *start; // point to first token character
  comp = my_regcmp ((const char*)def);
  if (comp) {
    start = (char*)source;
    std::cout << "source=" << (void*)start << ",=" << start << std::endl;
    while (*start != 0) {
      after= my_regex(comp,start);
      std::cout << "start=" << (void*)start << ",after=" << (void*)after << std::endl;
      start++;
    }
    my_regfree (comp);
}
/*
  cl_GenStreamable::RegisterClass("A class", ABCCreator);
  cl_GenStreamable::RegisterClass("B class", ABCCreator);
  cl_GenStreamable::RegisterClass("C class", ABCCreator);
  cl_GenStreamable::RegisterClass("D class", ABCCreator);
  cl_GenStreamable::RegisterClass("E class", ABCCreator);
*/
/*
  cl_A* a= new cl_A;
  cl_B* b= new cl_B;
  cl_C* c= new cl_C;
  cl_D* d= new cl_D;
  cl_E* e= new cl_E;

  std::ofstream o("test");
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
*/

/*
  cl_E *e;

  std::ifstream i("test");
  std::ofstream o("test2");
  cl_GenStreamable::ReadClassTable(i);
  cl_GenStreamable::WriteClassTable(o);
  const char* className = cl_GenStreamable::CurrentClassName(i);
  e =(cl_E*)cl_GenStreamable::Create(i);
  o << *e ;

 delete e;





*/

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

    ofstream o("test");
    st.write(o);
    g1.write(o);
    g2.write(o);
    g3.write(o);
    */

};
