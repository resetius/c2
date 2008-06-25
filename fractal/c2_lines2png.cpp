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
#include <gd.h>
#include "c2_2graph.h"
#include "common/c2_colormap_vga1.h"

using namespace std;

static void init_color_map(int *colors, gdImagePtr & im)
{
	for (uint i = 0; i < 256; ++i) {
		colors[i] = gdImageColorAllocate(im,
										 colormap_vga2[i][0],
										 colormap_vga2[i][1],
										 colormap_vga2[i][2]);
	}
}

string print_lines2png(Group & g, list < line > & ln,
                       double min_x, double max_x, double min_y, double max_y)
{
	string fname = "output.png";
	int colors[256];

	if (!g.name.empty()) fname = g.name + ".png";
        FILE * f = fopen(fname.c_str(), "wb");
	if (!f) return "";

	gdImagePtr im;
	int w = 1024;
	int h = 1024;
	im    = gdImageCreate(w, h);
	int trans = gdImageColorAllocate(im, 0, 0, 0);

	init_color_map(colors, im);

	gdImageFill(im, w - 1, h - 1, trans);
//	gdImageColorTransparent(im, trans);

	double xx = (w - 2) / (max_x - min_x);
        double yy = (h - 2) / (max_y - min_y);
	double kk = std::min(xx, yy);

	for (list < line >::iterator it = ln.begin(); it != ln.end(); ++it)
	{
		int x0 = (int)(1 + (it->x0 - min_x) * kk); 
		int y0 = (int)(1 + (it->y0 - min_y) * kk); y0 = h - y0 - 1;
		int x1 = (int)(1 + (it->x1 - min_x) * kk);
		int y1 = (int)(1 + (it->y1 - min_y) * kk); y1 = h - y1 - 1;
		gdImageLine(im, x0, y0, x1, y1, colors[it->c]);
	}

	gdImagePng(im, f); 
	fclose(f);
	gdImageDestroy(im);
	return fname;
}

