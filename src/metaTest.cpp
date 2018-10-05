#include <stdlib.h>
#include <iostream>
#include <genNode.h>
#include <genParser.h>
#include <metaLang.h>


void BeginPlugIn(boolean v);
void EndPlugIn(std::ostream& os, void* root);
short verbose;

int CompileProgram(const char* p, const char* o){
  cl_GenParser parser;
  cl_GenBlock *program=NULL;
  std::ifstream gis(p);
  cl_GenStatement::SetVerbose(verbose);
  std::cout << "Parsing your WALK program..." << std::endl;
  parser.Load(gis);
  program = parser.ParseBlock("main");
  if (program){
    std::cout << "... end of WALK parsing " << std::endl;
    std::ofstream os (o,std::ios::binary);
    std::cout << "Saving your compiled WALK program as " << o << std::endl;
    program->StreamTo(os);
    std::cout << "... end of save" << std::endl;
    std::cout << "Deleting program" << std::endl;
    delete program;
  }
  else {
    std::cout << "No program..." << std::endl;
    return -1;
  };
  return 0;
};
int RunPlugsIn(const char* m,const char* s, const char* o){
  void* root=NULL;
  std::ifstream mis(m);
  std::ifstream sis(s);
  cl_MRule::SetVerbose(verbose);
  cl_GenNode::verbose = verbose;
  std::cout << "Meta parsing..." << std::endl;
  cl_MParserIStream::IFile = m;
  cl_MRule* g=cl_MRule::ParseGrammar(mis);
  if (g){
    std::cout << "... end meta parsing" << std::endl;
    std::cout << "Source parsing..." << std::endl;
    cl_MParserIStream::IFile = s;
    cl_MRule::ParseSource(sis);

    if (cl_MRule::GeneratedCode()){
      std::cout << "... end source parsing" << std::endl;
      PRINT ("Generated code at :" << (void*)cl_MRule::GeneratedCode());
      if (verbose)
		((cl_GenNode*)cl_MRule::GeneratedCode())->Show();
	  cl_GenStatement::AddPointer("top",(cl_GenNode*)cl_MRule::GeneratedCode());
      std::cout << "Running plugs-in..." << std::endl;
      BeginPlugIn(verbose);
      ((cl_GenNode*)cl_MRule::GeneratedCode())->PlugIn(root);
      std::cout << "... end of execution" << std::endl;
      std::cout << "Deleting source" << std::endl;
      delete ((cl_GenNode*)cl_MRule::GeneratedCode());

	  if (root){
//      std::cout << "Saving root object in \"" << o << "\"" << std::endl;
      std::ofstream os (o);
      EndPlugIn(os,root);
      std::cout << "Deleting root object" << std::endl;
      delete (root);
    }
    }
    else{
      std::cout << "No source generated..." << std::endl;
      return -1;
    };
    std::cout << "Deleting meta" << std::endl;
    delete g;
    }
  else {
    std::cout << "No grammar..." << std::endl;
    return -1;
  };
  return 0;
};

int Full(const char* m,const char* s, const char* p, const char* o){
  cl_GenParser *parser=NULL;
  cl_GenBlock *program=NULL;
//  ifstream mis(m, ios::binary);
//  ifstream sis(s, ios::binary);
	std::ifstream mis(m);
	std::ifstream sis(s);
  cl_MRule::SetVerbose(verbose);
  cl_GenNode::verbose = verbose;
  cl_GenStatement::SetVerbose(verbose);
  std::cout << "Meta parsing..." << std::endl;
  cl_MParserIStream::IFile = m;
  cl_MRule* g=cl_MRule::ParseGrammar(mis);
  if (g){
	std::cout << "... end meta parsing" << std::endl;
    std::cout << "Source parsing..." << std::endl;
    cl_MParserIStream::IFile = s;
    cl_MRule::ParseSource(sis);

    if (cl_MRule::GeneratedCode()){
      std::cout << "... end source parsing" << std::endl;
      cl_MParserIStream::IFile = NULL;
      PRINT ("Generated code at :" << (void*)cl_MRule::GeneratedCode());
      if (verbose)
	((cl_GenNode*)cl_MRule::GeneratedCode())->Show();
      //      cl_GenNode::Save(nos,(cl_GenNode*)cl_MRule::GeneratedCode());
      parser= new cl_GenParser;
      if (parser){
		  std::ifstream gis(p,std::ios::binary);
		if (gis.peek() == GENBLOCK) {
		  std::cout << "Loading your WALK program..." << std::endl;
		  program = (cl_GenBlock*)cl_GenStatement::StreamFrom(gis);
		}
      else {
		std::cout << "Parsing your WALK program..." << std::endl;
		parser->Load(gis);
		program = parser->ParseBlock("main");
	  };
	if (program){
	  cl_GenStatement::SetMainBlock(program);
	  cl_GenStatement::AddPointer("top",(cl_GenNode*)cl_MRule::GeneratedCode());
	  std::cout << "... end of WALK program load/parse " << std::endl;
	  cl_GenStatement::SetOfstream(o);
	  std::cout << "Executing your WALK program..." << std::endl;
	  program->Run();
	  std::cout << "... end of execution(" << cl_GenStatement::NbStatements() << " statements)." << std::endl;
	  std::cout << "Deleting program" << std::endl;
	  delete program;
	}
	else {
	  std::cout << "No program to load/parse..." << std::endl;
	  return -1;
	};


	std::cout << "Deleting source" << std::endl;
	delete ((void*)cl_MRule::GeneratedCode());
	std::cout << "Deleting parser" << std::endl;
	delete parser;
      }
      else {
	std::cout << "No parser..." << std::endl;
	return -1;
      };

    }
    else{
      std::cout << "No source generated..." << std::endl;
      return -1;
    };
    std::cout << "Deleting meta" << std::endl;
    delete g;
    }
  else {
    std::cout << "No grammar..." << std::endl;
    return -1;
  };
  return 0;
};

int main (int argc, const char *argv[]){

  verbose = FALSE;
  while (*argv[argc-1]=='-'){
	  switch (*(argv[argc-1]+1)){
	  case('v'):  verbose = TRUE;break;
	  default: std::cerr << "Wrong option. Valid options are -v" <<std::endl;
		std::cerr << "-v = verbose" << std::endl;
		return -1;
	  };
	  argc--;
  };


  switch (argc) {
/*
  case(1):
  	verbose=true;
	return Full("CSV.meta", "test.csv", "html.gdl", "output.html");
*/
  case(3) : // program -> compiled program
    return CompileProgram(argv[1],argv[2]);

  case (4) : // source -> compiled node
    return RunPlugsIn (argv[1],argv[2],argv[3]);

  case(5):
    return Full(argv[1],argv[2],argv[3],argv[4]); // complete version
  default :
    break;
  };
  std::cerr << "WALK Engine - release 1.0.0" << std::endl;
  std::cerr << "usage: " << argv[0] << " meta source program output [-v]" << std::endl;
  std::cerr << "\tor " << argv[0] << " meta source output [-v] (for a plug-in execution)" << std::endl;
  std::cerr << "\tor " << argv[0] << " program output [-v] (for a program compilation))" << std::endl;
  return -1;

};
