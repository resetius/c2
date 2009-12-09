#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <bitset>
#include <vector>
#include <map>
#include <complex>
#ifdef WIN32
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif

using namespace std;

typedef complex < float >   cmpl;
typedef vector < char > volume_t;

struct Point
{
	short x;
	short y;
	short z;

	Point(short i, short j, short k): x(i), y(j), z(k) {}
	Point(): x(0), y(0), z(0) {}

	bool operator == (const Point & p) const
	{
		return x == p.x && y == p.y && z == p.z;
	}
};

struct Triangle
{
	int p1;
	int p2;
	int p3;

	Triangle(int i, int j, int k): p1(i), p2(j), p3(k) {}
	Triangle(): p1(0), p2(0), p3(0) {}
};

struct point_hash : public std::unary_function<Point, std::size_t> {
	std::size_t operator()(Point const& p) const
	{
		return p.x + p.y + p.z;
	}
};

typedef vector < Point > boundary_t;
typedef vector < Point > points_t;
typedef map < int, int > offset2bnd_t;
typedef vector < Triangle > triangles_t;
typedef map < int, vector < int > > p2trs_t;
typedef tr1::unordered_map < Point, int, point_hash > points_cache_t;

struct Mesh
{
	points_t ps;
	triangles_t trs;
	p2trs_t p2trs;
};

#ifndef WIN32
#include <gd.h>
void draw(volume_t & mask, int l, int w, int h) {
	gdImagePtr im = gdImageCreateTrueColor(w, h);
	gdImageFill(im, 0, 0, gdImageColorExact(im, 255, 255, 255));
	for (int i = 0; i < l; ++i) {
		for (int j = 0; j < w; ++j) {
			for (int k = 0; k < h; ++k) {
				long offset = (long)i * (long)w * (long)h + (long)j * (long)h + (long)k;
				if (mask[offset] > 0) {
					int color = ((double)k / h) * 256.;
					gdImageSetPixel(im, i, j, gdImageColorExact(im, 0, 0, color));
					break;
				}
			}
		}
	}
	FILE * f = fopen("mandelbulb.png", "wb");
	gdImagePng(im, f);
	fclose(f);
	gdImageDestroy(im);
}


void draw_bnd(boundary_t & bnd, int l, int w, int h) {
	gdImagePtr im = gdImageCreateTrueColor(w, h);
	gdImageFill(im, 0, 0, gdImageColorExact(im, 255, 255, 255));
	for (boundary_t::iterator it = bnd.begin(); it < bnd.end(); ++it) {
		int i = it->x;
		int j = it->y;
		int k = it->z;

		long offset = (long)i * (long)w * (long)h + (long)j * (long)h + (long)k;
		int blue  =  gdImageBlue(im, gdImageGetPixel(im, i, j));
		int color = ((double)k / h) * 256.;
		if (color < blue) {
			gdImageSetPixel(im, i, j, gdImageColorExact(im, 0, 0, color));
		}
	}

	FILE * f = fopen("mandelbulb.png", "wb");
	gdImagePng(im, f);
	fclose(f);
	gdImageDestroy(im);
}


#endif

float ipow(float a, int p)
{
	float r = 1.0;
	int i;
	for (i = 0; i < p; ++i)
	{
		r *= a;
	}
	return r;
}

static int get_point(points_cache_t & cache, int i, int j, int k)
{
	Point p(i, j, k);
	points_cache_t::iterator it = cache.find(p);
	if (it == cache.end()) 
	{
		int r = (int)cache.size();
		cache.insert(make_pair(p, r));
		return r;
	}
	return it->second;
}

static void gen_side(Mesh & mesh, 
					 points_cache_t & cache,
					 int i1, int i2, 
					 int j1, int j2,
					 int k1, int k2,
					 int p1_i, int p1_j, int p1_k,
					 int p2_i, int p2_j, int p2_k,
					 int p3_i, int p3_j, int p3_k,
					 int p4_i, int p4_j, int p4_k)
{
	points_t &     ps = mesh.ps;
	triangles_t & trs = mesh.trs;
	p2trs_t & p2trs   = mesh.p2trs;

	for (int i = i1; i < i2; ++i) {
		for (int j = j1; j < j2; ++j) {
			for (int k = k1; k < k2; ++k) {
				int p1  = get_point(cache, i + p1_i, j + p1_j, k + p1_k);
				int p2  = get_point(cache, i + p2_i, j + p2_j, k + p2_k);
				int p3  = get_point(cache, i + p3_i, j + p3_j, k + p3_k);
				int p4  = get_point(cache, i + p4_i, j + p4_j, k + p4_k);
				int tr1 = trs.size(); trs.push_back(Triangle(p1, p2, p3));
				int tr2 = trs.size(); trs.push_back(Triangle(p2, p3, p4));
				p2trs[p1].push_back(tr1);
				p2trs[p2].push_back(tr1);
				p2trs[p3].push_back(tr1);

				p2trs[p2].push_back(tr2);
				p2trs[p3].push_back(tr2);
				p2trs[p4].push_back(tr2);
			}
		}
	}
}

void init_mesh(Mesh & mesh, int l, int w, int h)
{
	// триангуляция куба
	mesh.ps.reserve(6 * w * h);
	points_cache_t cache;

	points_t &     ps = mesh.ps;
	triangles_t & trs = mesh.trs;
	p2trs_t & p2trs   = mesh.p2trs;

	gen_side(mesh, cache, 
		0, 1, 0, w - 1, 0, h - 1, 
		0, 0, 0,
		0, 1, 0,
		0, 0, 1,
		0, 1, 1);

	gen_side(mesh, cache, 
		l - 1, l, 0, w - 1, 0, h - 1, 
		0, 0, 0, 
		0, 1, 0,
		0, 0, 1,
		0, 1, 1);

	gen_side(mesh, cache, 
		0, l - 1, 0, 1, 0, h - 1, 
		0, 0, 0, 
		1, 0, 0,
		0, 0, 1,
		1, 0, 1);

	gen_side(mesh, cache, 
		0, l - 1, w - 1, w, 0, h - 1, 
		0, 0, 0, 
		1, 0, 0,
		0, 0, 1,
		1, 0, 1);

	gen_side(mesh, cache, 
		0, l - 1, 0, w - 1, 0, 1, 
		0, 0, 0, 
		1, 0, 0,
		0, 1, 0,
		1, 1, 0);

	gen_side(mesh, cache, 
		0, l - 1, 0, w - 1, h - 1, h, 
		0, 0, 0, 
		1, 0, 0,
		0, 1, 0,
		1, 1, 0);

	ps.resize(cache.size());
	for (points_cache_t::iterator it = cache.begin(); it != cache.end(); ++it)
	{
		ps[it->second] = it->first;
	}
}

inline int dist(const Point & p1, const Point & p2)
{
	int x = abs(p1.x - p2.x);
	int y = abs(p1.y - p2.y);
	int z = abs(p1.z - p2.z);
	int m = std::max(x, std::max(y, z));

	return m;
}

bool check_sphere(float x, float y, float z)
{
	return x * x + y * y + z * z <= 1;
}

bool check_mandelbulb(float c1, float c2, float c3)
{
	int order = 8;
	float z1 = 0, z2 = 0, z3 = 0;
	for (int iter = 0; iter < 256; ++iter)
	{
		float r     = sqrtf(z1*z1+z2*z2+z3*z3);
		float theta = atan2f(sqrtf(z1*z1+z2*z2), z3);
		float phi   = atan2f(z2, z1);
		float ro    = ipow(r, order);

		z1 = ro * sinf(theta*order) * cosf(phi*order) + c1;
		z2 = ro * sinf(theta*order) * sinf(phi*order) + c2;
		z3 = ro * cosf(theta*order) + c3;

		if (sqrtf(z1*z1+z2*z2+z3*z3) > 2) {
			break;
		}
	}

	return sqrtf(z1*z1+z2*z2+z3*z3) < 2;
}

static int diff1[][3] = {
	// 6
	{ 1,  0,  0},
	{-1,  0,  0},
	{ 0,  1,  0},
	{ 0, -1,  0},
	{ 0,  0,  1},
	{ 0,  0, -1},

	// 12
	{ 0,  1,  1},
	{ 0,  1, -1},
	{ 0, -1,  1},
	{ 0, -1, -1},

	{ 1,  0,  1},
	{ 1,  0, -1},
	{-1,  0,  1},
	{-1,  0, -1},

	{ 1,  1,  0},
	{ 1, -1,  0},
	{-1,  1,  0},
	{-1, -1,  0},

	// 8
	{ 1,  1,  1},
	{ 1,  1, -1},
	{ 1, -1,  1},
	{ 1, -1, -1},
	{-1,  1,  1},
	{-1,  1, -1},
	{-1, -1,  1},
	{-1, -1, -1},
};
static int diff1_size = 6 + 12 + 8;

/* 
  0 changed
  1 do nothing
  2 remove point
 */
bool len1_move(int q, Mesh & mesh, volume_t & vol, int l, int w, int h)
{
	triangles_t & trs = mesh.trs;
	points_t & ps     = mesh.ps;
	Point & p = ps[q];
	bool r    = 2;

	unsigned min_max = -1;
	Point min_max_v;
	long min_offset;

	for (int q1 = 0; q1 < diff1_size; ++q1) 
	{
		int i = p.x + diff1[q1][0];
		int j = p.y + diff1[q1][1];
		int k = p.z + diff1[q1][2];
		long offset = (long)i * (long)w * (long)h + (long)j * (long)h + (long)k;
		if (!(0 <= i && i < l)) continue;
		if (!(0 <= j && j < w)) continue;
		if (!(0 <= k && k < h)) continue;

		if (i == p.x && j == p.y && k == p.z) continue;

		if (vol[offset]) continue;
		r = 1;

		Point new_p(i, j, k);

		vector < int > & tr_in_p = mesh.p2trs[q];
		int max_dist = 0;
		for (int trk = 0; trk < (int)tr_in_p.size(); ++trk)
		{
			Triangle & tr = trs[tr_in_p[trk]];
			max_dist = std::max(max_dist, dist(ps[tr.p1], new_p));
			max_dist = std::max(max_dist, dist(ps[tr.p2], new_p));
			max_dist = std::max(max_dist, dist(ps[tr.p3], new_p));
		}

		if (max_dist == 1) 
		{
			vol[offset] = 1;
			p       = new_p;
			return true;
		}
/*
		if (min_max > max_dist)
		{
			min_max    = max_dist;
			min_max_v  = new_p;
			min_offset = offset;
		}
*/
	}

/*
	if (min_max != (unsigned)-1) 
	{
		vol[min_offset] = 1;
		p = min_max_v;
		return true;
	}
*/
	return false;
}

bool len2_move(int q, 
			   vector < char > & tr_flags,
			   Mesh & mesh, volume_t & vol, 
			   int l, int w, int h)
{
	triangles_t & trs = mesh.trs;
	points_t & ps     = mesh.ps;
	Point & p = ps[q];

	for (int q1 = 0; q1 < diff1_size; ++q1) 
	{
		int i = p.x + diff1[q1][0];
		int j = p.y + diff1[q1][1];
		int k = p.z + diff1[q1][2];
		long offset = (long)i * (long)w * (long)h + (long)j * (long)h + (long)k;
		if (!(0 <= i && i < l)) continue;
		if (!(0 <= j && j < w)) continue;
		if (!(0 <= k && k < h)) continue;

		if (i == p.x && j == p.y && k == p.z) continue;

		if (vol[offset]) continue;

		Point new_p(i, j, k);

		vector < int > & tr_in_p = mesh.p2trs[q];
		vector < int > pp; pp.reserve(3);
		int trn;

		int max_dist = 0;

		for (int trk = 0; trk < (int)tr_in_p.size(); ++trk)
		{
			trn = tr_in_p[trk];
			Triangle & tr = trs[trn];

			pp.clear();

			if (dist(ps[tr.p1], new_p) == 1) pp.push_back(tr.p1);
			if (dist(ps[tr.p2], new_p) == 1) pp.push_back(tr.p2);
			if (dist(ps[tr.p3], new_p) == 1) pp.push_back(tr.p3);

			if (pp.size() == 3) {
				break;
			}
		}

		if (pp.size() == 3) 
		{
			// вместо одного треугольника делаем три
			tr_flags[trn] = 1;
			int p0 = ps.size(); ps.push_back(new_p);
			
			int tr1 = trs.size(); trs.push_back(Triangle(p0, pp[0], pp[1]));
			int tr2 = trs.size(); trs.push_back(Triangle(p0, pp[1], pp[2]));
			int tr3 = trs.size(); trs.push_back(Triangle(p0, pp[2], pp[0]));

			// обновляем треугольники в точке
			mesh.p2trs[p0].push_back(tr1);
			mesh.p2trs[p0].push_back(tr2);
			mesh.p2trs[p0].push_back(tr3);

			mesh.p2trs[pp[0]].push_back(tr1);
			mesh.p2trs[pp[0]].push_back(tr3);

			mesh.p2trs[pp[1]].push_back(tr1);
			mesh.p2trs[pp[1]].push_back(tr2);

			mesh.p2trs[pp[2]].push_back(tr2);
			mesh.p2trs[pp[2]].push_back(tr3);

			for (int i = 0; i < 3; ++i) {
				vector < int > & tr_in_p = mesh.p2trs[pp[0]];
				vector < int > new_tr_in_p;

				for (int j = 0; j < (int)tr_in_p.size(); ++j) {
					if (tr_in_p[j] != trn) {
						new_tr_in_p.push_back(tr_in_p[j]);
					}
				}

				tr_in_p.swap(new_tr_in_p);
			}

			vol[offset] = 1;
			return true;
		}
	}
	return false;
}

bool iterate_mesh_(Mesh & mesh, volume_t & vol, int l, int w, int h)
{
	float s1 = -2.0f;
	float s2 =  2.0f;

	float xx = (float)l / (s2 - s1);
	float yy = (float)w / (s2 - s1);
	float zz = (float)h / (s2 - s1);

	triangles_t & trs = mesh.trs;
	points_t & ps     = mesh.ps;

	bool changed = false;
	int ps_size  = (int)ps.size();
	int trs_size = (int)trs.size();

	// каждая точка может породить 2 точки, поэтому с запасом
	vector < char > ps_flags(2 * ps_size);
	// каждый треугольник может породить 3 треугольника, поэтому с запасом
	vector < char > tr_flags(3 * trs_size);

	for (int q = 0; q < ps_size; ++q) 
	{
		Point & p = ps[q];

		float x = p.x / xx + s1;
		float y = p.y / yy + s1;
		float z = p.z / zz + s1;

//		if (check_sphere(x, y, z)) 
		if (check_mandelbulb(x, y, z)) 
		{
			continue;
		}

		bool flag = len1_move(q, mesh, vol, l, w, h);
//		if (!flag) {
//			flag = len2_move(q, tr_flags, mesh, vol, l, w, h);
//		}
		changed |= flag;
/*
		if (!flag) 
		{
			// remove point
			ps_flags[q] = 1;
			vector < int > & tr_in_p = mesh.p2trs[q];

			// remove triangles
			for (int trk = 0; trk < (int)tr_in_p.size(); ++trk)
			{
				tr_flags[tr_in_p[trk]] = 1;
			}
		}
*/
	}

	// rebuild mesh
	points_t new_ps;
	triangles_t new_trs;
	p2trs_t new_ps2trs;

	ps_size = ps.size();
	trs_size = trs.size();

	vector < int > oldp2new(ps_size);

	new_ps.reserve(ps_size);
	new_trs.reserve(trs_size);

	for (int i = 0, j = 0; i < ps_size; ++i)
	{
		if (ps_flags[i] == 0) {
			new_ps.push_back(ps[i]);
			oldp2new[i] = j++;
		}
	}

	for (int i = 0; i < trs_size; ++i)
	{
		if (tr_flags[i] == 0) {
			int p1 = oldp2new[trs[i].p1];
			int p2 = oldp2new[trs[i].p2];
			int p3 = oldp2new[trs[i].p3];

			int tr = new_trs.size(); new_trs.push_back(Triangle(p1, p2, p3));
			new_ps2trs[p1].push_back(tr);
			new_ps2trs[p2].push_back(tr);
			new_ps2trs[p3].push_back(tr);
		}
	}

	mesh.ps.swap(new_ps);
	mesh.trs.swap(new_trs);
	mesh.p2trs.swap(new_ps2trs);

	return changed;
}

void iterate_mesh(Mesh & mesh, int l, int w, int h)
{
	// каждая точка сетки двигается так, 
	// что "длина" прилегающего ребра остается равным единице
	// если это не возможно, то разбиваем ребро на два
	// длина = max(x1 - x0, y1 - y0, z1 - z0)

	volume_t vol(l * w * h);

	for (int i = 0; i < mesh.ps.size(); ++i) {
		Point & p = mesh.ps[i];
		long offset = (long)p.x * (long)w * (long)h + (long)p.y * (long)h + (long)p.z;
		vol[offset] = 1;
	}

	int iter = 0;
	//while (iterate_mesh_(mesh, vol, l, w, h)) { iter ++; }
	iterate_mesh_(mesh, vol, l, w, h);
}

void print_mesh(Mesh & mesh)
{
	FILE * f = fopen("output.txt", "w");
	fprintf(f, "#\n");
	for (int i = 0; i < (int)mesh.ps.size(); ++i) {
		fprintf(f, "%d %d %d\n", mesh.ps[i].x, mesh.ps[i].y, mesh.ps[i].z);
	}
	fprintf(f, "#\n");
	for (int i = 0; i < (int)mesh.trs.size(); ++i) {
		fprintf(f, "%d %d %d\n", mesh.trs[i].p1 + 1, mesh.trs[i].p2 + 1, mesh.trs[i].p3 + 1);
	}
	fclose(f);
}

void init_boundary(boundary_t & bnd, volume_t & vol, int l, int w, int h)
{
	bnd.reserve(6 * w * h);
	back_insert_iterator < boundary_t > it = std::back_inserter(bnd);

	for (int i = 0; i < 1; ++i) {
		for (int j = 1; j < w - 1; ++j) {
			for (int k = 1; k < h - 1; ++k) {
				long offset = (long)i * (long)w * (long)h + (long)j * (long)h + (long)k;
				vol[offset] = -1;
				*it++ = Point(i, j, k);
			}
		}
	}

	for (int i = l - 1; i < l; ++i) {
		for (int j = 1; j < w - 1; ++j) {
			for (int k = 1; k < h - 1; ++k) {
				long offset = (long)i * (long)w * (long)h + (long)j * (long)h + (long)k;
				vol[offset] = -1;
				*it++ = Point(i, j, k);
			}
		}
	}

	for (int i = 1; i < l - 1; ++i) {
		for (int j = 0; j < 1; ++j) {
			for (int k = 1; k < h - 1; ++k) {
				long offset = (long)i * (long)w * (long)h + (long)j * (long)h + (long)k;
				vol[offset] = -1;
				*it++ = Point(i, j, k);
			}
		}
	}

	for (int i = 1; i < l - 1; ++i) {
		for (int j = w - 1; j < w; ++j) {
			for (int k = 1; k < h - 1; ++k) {
				long offset = (long)i * (long)w * (long)h + (long)j * (long)h + (long)k;
				vol[offset] = -1;
				*it++ = Point(i, j, k);
			}
		}
	}

	for (int i = 1; i < l - 1; ++i) {
		for (int j = 1; j < w - 1; ++j) {
			for (int k = 0; k < 1; ++k) {
				long offset = (long)i * (long)w * (long)h + (long)j * (long)h + (long)k;
				vol[offset] = -1;
				*it++ = Point(i, j, k);
			}
		}
	}

	for (int i = 1; i < l - 1; ++i) {
		for (int j = 1; j < w - 1; ++j) {
			for (int k = h - 1; k < h; ++k) {
				long offset = (long)i * (long)w * (long)h + (long)j * (long)h + (long)k;
				vol[offset] = -1;
				*it++ = Point(i, j, k);
			}
		}
	}
}

bool build_boundary_(boundary_t & answer, 
					 offset2bnd_t & offset2bnd,
					 boundary_t & bnd, 
					 volume_t & vol, 
					 int l, int w, int h)
{
	boundary_t new_bnd;
	new_bnd.reserve(bnd.size());
	back_insert_iterator < boundary_t > insert = std::back_inserter(new_bnd);
	back_insert_iterator < boundary_t > answ   = std::back_inserter(answer);
	bool updated = false;

	int diff1[][3] = {
		// 6
		{ 1,  0,  0},
		{-1,  0,  0},
		{ 0,  1,  0},
		{ 0, -1,  0},
		{ 0,  0,  1},
		{ 0,  0, -1},
	};
	int diff1_size = 6;

	for (boundary_t::iterator it = bnd.begin(); it != bnd.end(); ++it)
	{
		int x = it->x;
		int y = it->y;
		int z = it->z;
		long offset = (long)x * (long)w * (long)h + (long)y * (long)h + (long)z;
		if (vol[offset] == 2) {
			// already on the surface set
			continue;
		}

		for (int q1 = 0; q1 < diff1_size; ++q1) {
			int x = it->x + diff1[q1][0];
			int y = it->y + diff1[q1][1];
			int z = it->z + diff1[q1][2];

			if (!(0 <= x && x < l)) {
				continue;
			}
			if (!(0 <= y && y < w)) {
				continue;
			}
			if (!(0 <= z && z < h)) {
				continue;
			}
			long offset = (long)x * (long)w * (long)h + (long)y * (long)h + (long)z;

			if (vol[offset] < 0 || vol[offset] == 2) {
				continue;
			}

			if (vol[offset] == 1) {
				vol[offset] = 2;
				offset2bnd[offset] = (int)answer.size();
				*answ++     = Point(x, y, z);
			} else {
				vol[offset] = -1;
				*insert++   = Point(x, y, z);
				updated     = true;
			}
		}
	}
	new_bnd.swap(bnd);
	return updated;
}

void build_boundary(boundary_t & answer, 
					offset2bnd_t & offset2bnd,
					boundary_t & bnd, 
					volume_t & vol, 
					int l, int w, int h)
{
	while (build_boundary_(answer, offset2bnd, bnd, vol, l, w, h))
	{
	}
}

void build_triangulation(triangles_t & tri,
						 boundary_t & bnd,
						 offset2bnd_t & offset2bnd,
						 volume_t & vol, int l, int w, int h)
{
	int diff1[][3] = {
		// 6
		{ 1,  0,  0},
		{-1,  0,  0},
		{ 0,  1,  0},
		{ 0, -1,  0},
		{ 0,  0,  1},
		{ 0,  0, -1},

		// 12
		{ 0,  1,  1},
		{ 0,  1, -1},
		{ 0, -1,  1},
		{ 0, -1, -1},

		{ 1,  0,  1},
		{ 1,  0, -1},
		{-1,  0,  1},
		{-1,  0, -1},

		{ 1,  1,  0},
		{ 1, -1,  0},
		{-1,  1,  0},
		{-1, -1,  0},

		// 8
		{ 1,  1,  1},
		{ 1,  1, -1},
		{ 1, -1,  1},
		{ 1, -1, -1},
		{-1,  1,  1},
		{-1,  1, -1},
		{-1, -1,  1},
		{-1, -1, -1},
	};
	int diff1_size = 6 + 12 + 8;

	for (int p1 = 0; p1 < (int)bnd.size(); ++p1)
	{
		int p2 = -1;
		int p3 = -1;
		int q1, q2;
		for (q1 = 0; q1 < diff1_size; ++q1) {
			int x = bnd[p1].x + diff1[q1][0];
			int y = bnd[p1].y + diff1[q1][1];
			int z = bnd[p1].z + diff1[q1][2];
			long offset = (long)x * (long)w * (long)h + (long)y * (long)h + (long)z;

			if (vol[offset] == 2) 
			{
				p2 = offset2bnd[offset];
				break;
			} else {
				// not a boundary
			}
		}

		if (p2 < 0) {
			continue;
		}

		for (q2 = 0; q2 < diff1_size; ++q2) {
			int x_off = diff1[q1][0] + diff1[q2][0];
			int y_off = diff1[q1][1] + diff1[q2][1];
			int z_off = diff1[q1][2] + diff1[q2][2];

			if (abs(x_off) > 1 || abs(y_off) > 1 || abs(z_off) > 1)
			{
				// за пределами нашего куба
				continue;
			}

			if (x_off == 0 && y_off == 0 && z_off == 0) {
				// центр
				continue;
			}

			int x = bnd[p1].x + x_off;
			int y = bnd[p1].y + y_off;
			int z = bnd[p1].z + z_off;
			long offset = (long)x * (long)w * (long)h + (long)y * (long)h + (long)z;

			if (vol[offset] == 2) {
				p3 = offset2bnd[offset];
				break;
			}
		}

		if (p3 < 0) {
			continue;
		}

		tri.push_back(Triangle(p1, p2, p3));
	}
}

void print_triangulation(triangles_t & tri,
						 boundary_t & bnd)
{
	FILE * f = fopen("output.txt", "w");
	fprintf(f, "# xxx\n");
	for (boundary_t::iterator it = bnd.begin(); it < bnd.end(); ++it)
	{
		fprintf(f, "%d %d %d\n", it->x, it->y, it->z);
	}
	fprintf(f, "# xxx\n");
	for (triangles_t::iterator it = tri.begin(); it < tri.end(); ++it)
	{
		fprintf(f, "%d %d %d\n", it->p1, it->p2, it->p3);
	}
	fclose(f);
}

void do_all(int l, int w, int h, int order)
{
	volume_t mask(l * w * h);
	boundary_t bnd;
	boundary_t answer;
	offset2bnd_t offset2bnd;
	triangles_t tri;

	float s1 = -2.0f;
	float s2 =  2.0f;

	float xx = (float)l / (s2 - s1);
	float yy = (float)w / (s2 - s1);
	float zz = (float)h / (s2 - s1);

#pragma omp parallel for
	for (int i = 0; i < l; ++i) {
		float c1 = i / xx + s1;
		for (int j = 0; j < w; ++j) {
			float c2 = j / yy + s1;
			for (int k = 0; k < h; ++k) {
				float c3 = k / zz + s1; 
				long offset = (long)i * (long)w * (long)h + (long)j * (long)h + (long)k;

				float z1 = 0, z2 = 0, z3 = 0;
				for (int iter = 0; iter < 256; ++iter)
				{
					float r     = sqrtf(z1*z1+z2*z2+z3*z3);
					float theta = atan2f(sqrtf(z1*z1+z2*z2), z3);
					float phi   = atan2f(z2, z1);
					float ro    = ipow(r, order);

					z1 = ro * sinf(theta*order) * cosf(phi*order) + c1;
					z2 = ro * sinf(theta*order) * sinf(phi*order) + c2;
					z3 = ro * cosf(theta*order) + c3;

					if (sqrtf(z1*z1+z2*z2+z3*z3) > 2) {
						break;
					}
				}

				if (sqrtf(z1*z1+z2*z2+z3*z3) < 2) {
					mask[offset] = 1;
				}
			}
		}
	}

	fprintf(stderr, "init bnd\n");
	init_boundary(bnd, mask, l, w, h);
	fprintf(stderr, "build bnd\n");
	build_boundary(answer, offset2bnd, bnd, mask, l, w, h);
	build_triangulation(tri, answer, offset2bnd, mask, l, w, h);
	print_triangulation(tri, answer);

#ifndef WIN32
	fprintf(stderr, "draw\n");
	//draw(mask, l, w, h);
	draw_bnd(answer, l, w, h);
#endif

	fprintf(stderr, "ok\n");
}

int main(int argc, char ** argv)
{
	int l, w, h, order;
	if (argc > 1) {
		l = w = h = atoi(argv[1]);
	} else {
		l = w = h = 64;
	}

	if (argc > 2) {
		order = atoi(argv[2]);
	} else {
		order = 8;
	}

	Mesh mesh;
	init_mesh(mesh, l, w, h);
	iterate_mesh(mesh, l, w, h);
	print_mesh(mesh);

	//do_all(l, w, h, order);
}

