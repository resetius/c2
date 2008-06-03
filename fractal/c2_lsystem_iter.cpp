extern "C" {
#include "c2_turtle_scanner.h"
#include "c2_turtle.h"
}
#include "c2_lsystem.h"
#include <string>

using namespace std;

string lsystem_iter(string & src, Group & gr)
{
	int result;
	YY_BUFFER_STATE buf_state;

	buf_state = turtle_scan_string(&src[0]);
	string r;
	while ((result = turtlelex()) != 0)
	{
		switch (result) {
		case '@':
		case '!':
		case '+':
		case '-':
			r += (char)result;
			break;
		case NUMBER:
			r += st.str;
			break;
		default:
			r += gr.r[(char)result];
			break;
		}
	}
}

string lsystem(Group & gr, int level) {
	string W = gr.axiom;

	for (uint i = 0; i < level; ++i) {
		W = lsystem_iter(W, gr);
	}

	return W;
}

