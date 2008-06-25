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

#include <stdio.h>
#include "c2_lsystem.h"
#include "c2_2graph.h"

using namespace std;

string print_lines2mgl(Group & g, list < line > & ln,
                       double min_x, double max_x, double min_y, double max_y)
{
	string fname = "output.mgl";

	if (!g.name.empty()) fname = g.name + ".mgl"; 
	FILE * f = fopen(fname.c_str(), "w");

	fprintf(f, "axis %lf %lf %lf %lf\n", min_x, min_y, max_x, max_y);
	for (list < string> :: iterator it = g.mgl.begin(); it != g.mgl.end(); ++it)
	{
		fprintf(f, "%s\n", it->c_str());
	}

	for (list < line >::iterator it = ln.begin(); it != ln.end(); ++it)
	{
		fprintf(f, "line %.16lf %.16lf %.16lf %.16lf\n",
		            it->x0, it->y0, it->x1, it->y1);
	}

	fclose(f);
	return fname;
}

