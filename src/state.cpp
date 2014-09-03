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

#include "def.h"
#include "value.h"
#include "token.h"
#include "state.h"

extern void bi_get_value_len(Pu *L, int, const pu_value *v);
extern void bi_time(Pu *L, int ,const pu_value*);
extern void bi_get_value_len(Pu *L, int, const pu_value *v);
extern void bi_write(Pu *L, int ,const pu_value *v);
extern void bi_read(Pu *L, int ,const pu_value *v);
extern void bi_rand(Pu *L, int ,const pu_value *v);
extern void bi_sleep(Pu *L, int, const pu_value *v);
extern void bi_type(Pu *L, int, const pu_value *v);
extern void bi_eval(Pu *L, int, const pu_value *v);
extern void bi_quit(Pu *L, int, const pu_value *);
extern void bi_read(Pu *L, int argc, const pu_value *v);
extern void bi_open(Pu *L, int argc, const pu_value *v);
extern void bi_close(Pu *L, int argc, const pu_value *v);
extern void bi_str(Pu *L, int argc, const pu_value *v);
extern void bi_num(Pu *L, int argc, const pu_value *v);
extern void bi_coro_create(Pu *L, int argnum, const pu_value *v);
extern void bi_coro_resume(Pu *L, int argnum, const pu_value *v);
extern void bi_coro_yield(Pu *L, int argnum, const pu_value *v);
extern void bi_get_var(Pu *L, int argnum, const pu_value *v);
extern void force_sweep(Pu *L);
PUAPI void pu_reg_func(Pu *L, const char *funcname, ScriptFunc pfunc);

PUAPI Pu *pu_open()
{
	Pu *L = new Pu;
	return L;
}

void regbuiltin(Pu *L)
{
	pu_reg_func(L, "len", bi_get_value_len);
	pu_reg_func(L, "write", bi_write);
	pu_reg_func(L, "read", bi_read);
	pu_reg_func(L, "rand", bi_rand);
	pu_reg_func(L, "time", bi_time);
	pu_reg_func(L, "sleep", bi_sleep);
	pu_reg_func(L, "type", bi_type);
	pu_reg_func(L, "eval", bi_eval);
	pu_reg_func(L, "quit", bi_quit);
	pu_reg_func(L, "coro_create", bi_coro_create);
	pu_reg_func(L, "coro_resume", bi_coro_resume);
	pu_reg_func(L, "coro_yield", bi_coro_yield);
	pu_reg_func(L, "open", bi_open);
	pu_reg_func(L, "read", bi_read);
	pu_reg_func(L, "close", bi_close);
	pu_reg_func(L, "str", bi_str);
	pu_reg_func(L, "num", bi_num);
	pu_reg_func(L, "get_var", bi_get_var);
}

void clear_state(Pu *L)
{
	if (L->source.pf)
	{
		fclose(L->source.pf);
		L->source.pf=NULL;
	}

	L->source.str.buff = 0;
	L->source.str.pos = 0;
	L->source.type = -1;
	L->callstack.clear();
	L->cur_token = 0;
	L->isquit = false;
	L->isreturn = false;
	L->return_value = __pu_value(L);
	force_sweep(L);
}

PUAPI void pu_close(Pu *L)
{
	clear_state(L);
	delete L;
}