/*
 *  $Id: metaGen.h,v 1.0 1997/01/21 07:20:50 andre Exp $
 */

#ifndef _METAGEN_H_
#define _METAGEN_H_

#include <metaLang.h>

void EnterRule(cl_MRule& rule,void*& context,void*& subContext,const char* alias);
void ExitRule(cl_MRule& rule,void*& context,void*& subContext,const char* alias);



#endif // _METAGEN_H_
