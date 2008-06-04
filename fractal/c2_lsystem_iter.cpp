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

	Context(double alpha, double x, double y)
		: a(alpha), x0(x), y0(y), r(1.0), sgn(1.0) {}
};

list < line > turtle(Group & p, const string & src)
{
	stack < Context > stk;
	list < line > ret;
	Context c (p.alpha, 0, 0);

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
			c.x0 += c.r * cos(c.a); c.y0 += c.r * sin(c.a);
			break;
			/*branch open*/
		case '[':
			stk.push(c);
			break;
			/*branch close*/
		case ']':
			c = stk.top();
			stk.pop();
			break;
			/*forward*/
		case 'F':
			line l;
			l.x0 = c.x0; l.y0 = c.y0;
			c.x0 += c.r * cos(c.a); c.y0 += c.r * sin(c.a);
			l.x1 = c.x0; l.y1 = c.y0;
			ret.push_back(l);
			break;
		case '!': c.sgn = -c.sgn; break;
		case COLOR:
			/*TODO:*/
			break;
		case NUMBER:
			c.r *= st.m; free(st.str);
			break;
		case INCNUMBER:
			c.a += (double)st.i; free(st.str);
		default:
			break;
		}
	}

	turtle_delete_buffer(buf_state);

	return ret;
}

