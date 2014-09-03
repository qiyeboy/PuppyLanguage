/*
	Copyright (c) 2009 Zhang li

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.

	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

/*
	Author zhang li
	Email zlvbvbzl@gmail.com
*/

#include <time.h>
#include "def.h"
#include "value.h"
#include "token.h"
#include "state.h"
#include "error.h"
#include "PuMath.h"

#ifdef _MSC_VER
#include <windows.h> //for win32 Sleep
#pragma warning(disable:4127) // �ж�����Ϊ���������磺while(1)
#else
#include <unistd.h>  //for linux usleep
#endif

extern void get_var(Pu *L, const PuString &name, __pu_value &v, Token *t=0);
PUAPI{
PURESULT pu_push_arr(pu_value varr, const pu_value v);
PURESULT pu_loadbuff(Pu *L, const char *str);
void pu_val2str(Pu *, const pu_value *p, char *b, int buffsize);
void pu_set_return_value(Pu *L, const pu_value v);
}
extern int vm(Pu *L);
extern bool check_complete(Pu *L);
extern int findcurstart(Pu *L);
#define bi_return_null {__pu_value None(L); None.SetType(NIL); pu_set_return_value(L, &None);}return;
void bi_return_null_func(Pu *L)
{
    bi_return_null;
}

void bi_return_num( Pu * L, int v )
{
    __pu_value r = __pu_value(L);
    r.SetType(NUM);
    r.numVal() = v;
    pu_set_return_value(L, &r);
}

const char *get_typestr(__pu_value &v)
{
	static const char *type_string[]={
		"null",
		"",
		"",
		"",
		"",
		"<number>",
		"<string>",
		"<array>",
        "<map>",
		"<coroutine>",
		"<filehandle>",
		"<boolean>",
		"<function>",
        "<cfunction>"
	};

	return type_string[(int)v.type()-NIL];
}

void bi_sleep(Pu *L, int, const pu_value *v)
{
	if (v == NULL || v[0]->type() != NUM)
	{
		error(L, 24);
		bi_return_null;
	}
	unsigned long mili = (unsigned long)(v[0]->numVal());

#ifdef _WIN32
	Sleep(mili);
#else
	usleep(mili*1000);
#endif
    
    bi_return_null;
}

void bi_get_value_len(Pu *L, int, const pu_value *v)
{
	if (v == NULL)
	{
		error(L, 25);
		bi_return_null;
	}

	__pu_value r(L);
	r.SetType(NUM);
	if (v[0]->type() == NUM)
	{
		r.numVal() = 1;
	}
	else if (v[0]->type() == STR)
	{
		r.numVal() = (PU_NUMBER)v[0]->strVal().length();
	}
	else if (v[0]->type() == ARRAY)
	{
		r.numVal() = (PU_NUMBER)v[0]->arr().size();
	}
	pu_set_return_value(L,&r);
}

void bi_time(Pu *L, int, const pu_value*)
{
	struct tm *local; 
	time_t ti; 
	ti=time(NULL); 
	local=localtime(&ti); 
	
	__pu_value t(L);
	t.SetType(NUM);
	
	__pu_value r(L);
	r.createby = PU_USER;

	t.numVal() = (PU_NUMBER)local->tm_year+1900;
	pu_push_arr(&r, &t);

	t.numVal() = (PU_NUMBER)local->tm_mon;
	pu_push_arr(&r, &t);

	t.numVal() = (PU_NUMBER)local->tm_mday;
	pu_push_arr(&r, &t);

	t.numVal() = (PU_NUMBER)local->tm_wday;
	pu_push_arr(&r, &t);

	t.numVal() = (PU_NUMBER)local->tm_hour;
	pu_push_arr(&r, &t);

	t.numVal() = (PU_NUMBER)local->tm_min;
	pu_push_arr(&r, &t);

	t.numVal() = (PU_NUMBER)local->tm_sec;
	pu_push_arr(&r, &t);

	pu_set_return_value(L, &r);
}

void bi_quit(Pu *L, int, const pu_value *)
{
	L->isquit = true;
}

void bi_rand(Pu *L, int, const pu_value *)
{
	__pu_value r(L);

	srand( (unsigned)time( NULL ) ); 

	int n = rand();
	r.SetType(NUM);
	r.numVal() = (PU_NUMBER)n;
	pu_set_return_value(L,&r);
}

#define WRITE_STR(s) \
	printf("%s", s->strVal().c_str());


#define WRITE_NUM(n)					\
	if (PU_INT(n->numVal()) == n->numVal())	\
		printf("%.lf", n->numVal());		\
	else								\
		printf("%lf", n->numVal());


static void write_arr(const __pu_value &arr)
{
	printf("[");
	ValueArr::iterator it = arr.arr().begin();
	ValueArr::iterator ite = arr.arr().end();
	while (it != ite)
	{
		__pu_value temp = *it;
		if (temp.type() == STR)
		{
			printf("\'");
			WRITE_STR((&temp));
			printf("\'");
		}
		else if (temp.type() == NUM)
		{
			WRITE_NUM((&temp));
		}
		else if (temp.type() == ARRAY)
		{
			write_arr(temp);
		}
		++it;

		if (it != ite)
			printf(",");
		else
			break;
	}
	printf("]");
}

void bi_str(Pu *L, int argc, const pu_value *v)
{
	if (argc == 0)
	{
		bi_return_null;
	}

	if (v[0]->type() != STR)
	{
		__pu_value o(L);
		char buff[65536];
		pu_val2str(L, &v[0], buff, sizeof(buff));
        o.SetType(STR);
		o.strVal() = buff;		
		pu_set_return_value(L, &o);
	}
	else
	{
		pu_set_return_value(L, v[0]);
	}
}

void bi_num(Pu *L, int argc, const pu_value *v)
{
	if (argc == 0 || v[0]->type() != STR)
	{
		bi_return_null;
	}
	const char *str = v[0]->strVal().c_str();
	__pu_value o(L);
	o.SetType(NUM);
	if (is_int(str))
	{
		int nv = atoi(str);
		o.numVal() = (PU_NUMBER)nv;
		pu_set_return_value(L, v[0]);
	}
	else if (is_float(str))
	{	
		PU_NUMBER nv = (PU_NUMBER)atof(str);
		o.numVal() = nv;
		pu_set_return_value(L, v[0]);
	}
}

static void writefilehandle(Pu *L, int argc, const pu_value *v)
{
	FILE *pfile = (FILE*)v[0]->userdata();
	for (int i=1; i<argc; ++i)
	{
		char buff[65536];
		pu_val2str(L, &v[i], buff, sizeof(buff));
		fwrite(buff, sizeof(char), strlen(buff), pfile);
		fwrite(" ", sizeof(char), 1, pfile);
	}
}

void bi_write(Pu *L, int argc, const pu_value *v)
{
	if (v == 0)
	{
        bi_return_null;
	}
	
	if (v[0]->type() == FILEHANDLE)
	{
	    writefilehandle(L,argc,v);
	}
	else
	{
		for (int i=0; i<argc; ++i)
		{
			if (v[i]->type() == NUM)
			{
				WRITE_NUM(v[i]);
			}
			else if (v[i]->type() == STR)
			{
				WRITE_STR(v[i]);
			}
			else if (v[i]->type() == ARRAY)
			{
				write_arr(*v[i]);
			}
            else if (v[i]->type() == BOOLEANT)
            {
                printf((v[i]->numVal() != 0)?"true":"false");
            }
			else
			{
				printf(get_typestr(*v[i]));
			}
		}
	}
    bi_return_null;
}


void bi_get_var(Pu *L, int argn, const pu_value *v)
{
	__pu_value temp(L);
	if (argn > 0)
	{
		if (v[0]->type() == STR)
		{
            get_var(L, v[0]->strVal(), temp, NULL);
		}
	}
	pu_set_return_value(L,&temp);
}

void bi_type(Pu *L, int argn, const pu_value *v)
{
	__pu_value o(L);
	o.SetType(STR);
	if (argn>0)
	{
		o.strVal() = get_typestr(*v[0]);
	}
	else
	{
		o.strVal() = "<null>";
	}
	pu_set_return_value(L,&o);
}

// ɾ��β���ķϴ���
static void clear_tailcode(Pu *L, int evstart)
{
	L->cur_token = L->callstack.top();
	L->callstack.pop();
	L->lasterr = -1;
	int max_token = L->tokens.size();
	for (int i=0; i < max_token-evstart; ++i)
	{
		L->tokens.pop_back();
	}
}

int do_string(Pu *L, const char *str)
{
	if (L->mode == PU_CODE_FROM_FILE) 
		L->mode = PU_CODE_FROM_BUFF;
	L->callstack.push(L->cur_token);
	PURESULT load_result = pu_loadbuff(L, str);
	int evstart = findcurstart(L);

	if (load_result == PU_FAILED)
	{
		if (L->lasterr >=0 && L->lasterr != 26)
		{
			clear_tailcode(L, evstart);
			L->mode = PU_CODE_FROM_FILE;
			return 0;
		}
	}
	
	if (!check_complete(L))
	{
		// ����δ��β��ȥ������FINISH
		L->tokens.pop_back();
		L->cur_token = L->callstack.top();
		L->callstack.pop();
		return -2;
	}
	else
	{
		L->cur_token = evstart;
		NEXT_TOKEN
		int ret = vm(L);
		L->cur_token = L->callstack.top();
		L->callstack.pop();
		L->lasterr = -1;
		L->mode = PU_CODE_FROM_FILE;
		return ret;
	}
}

void bi_eval(Pu *L, int argc, const pu_value *v)
{
	if (argc==0)
    {
        bi_return_null;
    }
	do_string(L, v[0]->strVal().c_str());
}
 
 void bi_open(Pu *L, int argc, const pu_value *v)
 {     
 	if (argc==0)
    {
        bi_return_null;
    }

 	__pu_value r(L);    
 	FILE *p = fopen(v[0]->strVal().c_str(), "a+");
    if (p == NULL)
    {
        r.SetType(NIL);
    }
    else
    {
        r.SetType(FILEHANDLE);
        r.userdata() = p;
    }
 	pu_set_return_value(L, &r);
 }
 
 void bi_close(Pu *L, int argc, const pu_value *v)
 {
 	if (argc==0) return;
 	if (v[0]->userdata())
 	{
 		fclose((FILE*)v[0]->userdata());
 	}
    bi_return_null;
 }

 static void readfilehandle(Pu *L, const pu_value *vrr)
 {
	 FILE *pfile = (FILE*)vrr[0]->userdata();
	 const int BUFFSIZE = 65536;
	 char buff[BUFFSIZE]={0};
	 if (vrr[1]->strVal() == "max")
	 {
		 fread(buff,sizeof(char),BUFFSIZE,pfile); // ���һ�ζ�64k
	 }
	 else if (vrr[1]->strVal() == "line")
	 {
		 fgets(buff, BUFFSIZE, pfile);
	 }
	 else if (vrr[1]->strVal() == "word")
	 {
		 fscanf(pfile, "%s", buff);
	 }

	 __pu_value r(L);
	 r.SetType(STR);
	 r.strVal() = buff;
	 pu_set_return_value(L, &r);
 }

void bi_read(Pu *L, int argc, const pu_value *vrr)
{
	if (argc == 0)
	{
		char temp[1024];
		scanf("%s", temp);
		__pu_value v(L);

		if (is_int(temp))
		{
			long nv = atol(temp);
			v.SetType(NUM);
			v.numVal() = (PU_NUMBER)nv;
		}
		else if (is_float(temp))
		{	
			PU_NUMBER nv = (PU_NUMBER)atof(temp);
			v.SetType(NUM);
			v.numVal() = nv;
		}
		else
		{
			v.SetType(STR);
			v.strVal() = temp;
		}

		pu_set_return_value(L,&v);
	}
	else if (argc > 1 && vrr[0]->type() == FILEHANDLE && vrr[1]->type() == STR)
	{
	    readfilehandle(L,vrr);
	}
}