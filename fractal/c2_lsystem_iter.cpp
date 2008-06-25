/*$Id$*/

/* Copyright (c) 2008 Alexey Ozeritsky (Алексей Озерицкий)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Alexey Ozeritsky.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

extern "C" {
#include "c2_turtle_scanner.h"
#include "c2_turtle.h"
}
#include "c2_lsystem.h"

#include <math.h>
#include <string>
#include <stack>

using namespace std;

int turtlewrap()
{
	return 1;
}

string lsystem_iter(string & src, Group & gr)
{
	int result;
	YY_BUFFER_STATE buf_state;

	buf_state = turtle_scan_string(&src[0]);
	string r;
	while ((result = turtlelex()) != 0)
	{
		switch (result) {
		case '!': /*inverse +/-*/
		case '+':
		case '-':
		case '[':
		case ']':
			r += (char)result;
			break;
		case INCNUMBER:
		case INCCOLOR:
		case COLOR:
		case NUMBER:
			r += st.str; free(st.str);
			break;
		default:
			if (gr.r.find((char)result) != gr.r.end()) {
				r += gr.r[(char)result];
			} else {
//				cerr << "not replaced " << (char)result << "\n";
				r += (char)result;
			}
			break;
		}
	}

	turtle_delete_buffer(buf_state);

	return r;
}

string lsystem(Group & gr, int level) {
	string W = gr.axiom;

	for (uint i = 0; i < level; ++i) {
		W = lsystem_iter(W, gr);
//		cerr << i << ":" << W << "\n";
	}

	return W;
}

struct Context {
	double a;
	double x0;
	double y0;
	double r;
	double sgn;
	int col;

	Context(double alpha, double x, double y)
		: a(alpha), x0(x), y0(y), r(1.0), sgn(1.0), col(0) {}
};

list < line > turtle(Group & p, const string & src)
{
	double inc;
	line l;
	stack < Context > stk;
	list < line > ret;
	Context c (p.alpha, 0, 0);
	int col = 0;

	int result;
	YY_BUFFER_STATE buf_state;

	buf_state = turtle_scan_string(&src[0]);
	string r;
	while ((result = turtlelex()) != 0)
	{
		switch (result) {
		case '+': c.a += c.sgn * p.theta; break;
		case '-': c.a -= c.sgn * p.theta; break;
			/*пропуск*/
		case 'G':
		case 'M':
			c.x0 += c.r * cos(c.a); c.y0 += c.r * sin(c.a);
			break;
			/*branch open*/
		case '[':
			//printf("branch [\n");
			stk.push(c);
			break;
			/*branch close*/
		case ']':
			//printf("branch ]\n");
			c = stk.top();
			stk.pop();
			break;
			/*forward*/
		case 'F':
		case 'D':
			//printf("draw\n");
			l.x0 = c.x0; l.y0 = c.y0;
			c.x0 += c.r * cos(c.a); c.y0 += c.r * sin(c.a);
			l.x1 = c.x0; l.y1 = c.y0;
			l.c  = c.col;
			ret.push_back(l);
			break;
		case '!':
			//printf("reverse sign\n");
			c.sgn = -c.sgn;
			break;
		case COLOR:
			c.col = st.i; free(st.str);
			break;
		case INCCOLOR:
			c.col = (256 + c.col + st.i) % 256;
			free(st.str);
			break;
		case NUMBER:
//			printf("mult r %lf\n", st.m);
			c.r *= st.m; free(st.str);
			break;
		case INCNUMBER:
			//printf("inc angle by %d\n", st.i);
			inc = c.sgn * (double)st.i * M_PI / 180.0;
			c.a += inc; 
			free(st.str);
			break;
		default:
			break;
		}
	}

	turtle_delete_buffer(buf_state);

	return ret;
}

