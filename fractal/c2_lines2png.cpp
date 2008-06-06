#include <stdio.h>
#include <gd.h>
#include "c2_2graph.h"
#include "c2_colormap_vga1.h"

using namespace std;

void init_color_map(int *colors, gdImagePtr & im)
{
	for (uint i = 0; i < 256; ++i) {
		colors[i] = gdImageColorAllocate(im,
										 colormap_vga1[i][0],
										 colormap_vga1[i][1],
										 colormap_vga1[i][2]);
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
	int trans = gdImageColorAllocate(im, 204, 204, 204);

	init_color_map(colors, im);

	gdImageFill(im, w - 1, h - 1, trans);
	gdImageColorTransparent(im, trans);

	double xx = (w - 2) / (max_x - min_x);
        double yy = (h - 2) / (max_y - min_y);
	double kk = std::min(xx, yy);

	for (list < line >::iterator it = ln.begin(); it != ln.end(); ++it)
	{
		int x0 = 1 + (it->x0 - min_x) * kk; 
		int y0 = 1 + (it->y0 - min_y) * kk; y0 = h - y0 - 1;
		int x1 = 1 + (it->x1 - min_x) * kk;
		int y1 = 1 + (it->y1 - min_y) * kk; y1 = h - y1 - 1;
		gdImageLine(im, x0, y0, x1, y1, colors[it->c]);
	}

	gdImagePng(im, f); 
	fclose(f);
	gdImageDestroy(im);
	return fname;
}

