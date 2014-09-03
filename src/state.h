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

#ifndef _pu_STATE_
#define _pu_STATE_

#include "PuMap.h"
#include "PuStack.h"
#include "PuVector.h"
typedef PuStack<int> CallStack;
typedef PuStack<Var*> VarStack;
typedef PuMap<PuString, int> LabelMap;
typedef PuVector<FuncPos> FuncList;
struct FuncPos
{
	int start;
	int end;
	PuVector<PuString, 4> argnames;
	ScriptFunc pfunc;

    FuncPos()
        :start(-1)
        ,end(-1)
    {

    }
};

struct Pustrbuff
{
	Pustrbuff():pos(0),buff(0)
	{}
	int			pos;
	const char *buff;
};

struct Pusource
{
	Pusource():pf(0),type(-1)
	{}
	FILE		*pf;
	Pustrbuff	str;
	int			type;

	enum __SOURCETYPE
	{
		BUFFER,
		FILE
	};
};

struct coro
{
	Var		*varmap;
	int		begin;
	int		cur;
	int		end;
	int		funpos;
    int     id;
};

typedef PuVector<coro> CoroList;

struct Pu
{
	Pu()
	:token(NULL),
	cur_token(0),
	line(1),
	isreturn(false),
	isquit(false),
    mode(0),
	err_handle(0),	
	output_handle(0),	
	lasterr(-1),
    isyield(false),
	upvalue(0),
	cur_nup(0),
	gclink(0)
	{
		varstack.push(new Var);
	}

	~Pu()
	{
		Var *nd = varstack.bottom();
		delete nd;
	}

	Token				*token;
	int					cur_token;
	int					line;
	bool				isreturn;
	bool				isquit;
	int					mode;
    PuVector<int>       funstack;
	ErrHandle			err_handle;
	OutputHandle		output_handle;
	int					lasterr;
	bool				isyield;
	TokenList			tokens;
	Pusource			source;
	LabelMap			labelmap;
	FuncList			funclist;
	VarStack			varstack;
	CallStack			callstack;
	__pu_value			return_value;
	PuVector<PuString>	current_filename;
	CoroList			coros;
	PuStack<int>		uncomdef;
	PuStack<int>		jumpstack;
	Var					*upvalue;
	_up_value			*cur_nup;
	_up_value			*gclink;
};


#endif