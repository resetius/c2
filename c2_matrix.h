/*$Id$*/

/* Copyright (c) 2008 Alexey Ozeritsky
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

#include <assert.h>

struct IntMatrix {
	int w;
	int h;

	int * d;

	double coef;
	bool owns;

	IntMatrix() : w(0), h(0), d(0), coef(1.0), owns(false) {}
	IntMatrix(int w1, int h1)
		: w(w1), h(h1), d(new int[w * h]), coef(1.0), owns(true) 
	{}

	IntMatrix(int * d1, int w1, int h1, double coef1 = 1.0, bool o = true)
		: w(w1), h(h1), d(o ? new int[w * h] : d1), coef(coef1), owns(o)
	{
		if (!owns) { memcpy(d, d1, w * h * sizeof(int)); }
	}

	~IntMatrix() { if (owns) delete [] d; }

	static IntMatrix & Soebel_x() {
		static int d[] = {
			-1, 0, 1,
			-2, 0, 2,
			-1, 0, 1,
		};

		static IntMatrix m(&d[0], 3, 3, 1.0 / 6.0, false);

		return m;
	}

	static IntMatrix & Soebel_y() {
		static int d[] = {
			1, 2, 1,
			0, 0, 0,
			-1, -2, -1,
		};

		static IntMatrix m(&d[0], 3, 3, 1.0 / 6.0, false);

		return m;
	}

	static IntMatrix & Previt_x() {
		static int d[] = {
			-1, 0, 1,
			-1, 0, 1,
			-1, 0, 1,
		};

		static IntMatrix m(&d[0], 3, 3, 1.0 / 6.0, false);

		return m;
	}

	static IntMatrix & Previt_y() {
		static int d[] = {
			1, 1, 1,
			0, 0, 0,
			-1, -1, -1,
		};

		static IntMatrix m(&d[0], 3, 3, 1.0 / 6.0, false);

		return m;
	}

	static IntMatrix & Roberts_x() {
		static int d[] = {
			0, 1,
			-1, 0,
		};

		static IntMatrix m(&d[0], 2, 2, 0.5, false);

		return m;
	}

	static IntMatrix & Roberts_y() {
		static int d[] = {
			1, 0,
			0, -1,
		};

		static IntMatrix m(&d[0], 2, 2, 0.5, false);

		return m;
	}

	static IntMatrix & Gauss_3_3() {
		static int d[] = {
			1, 2, 1,
			2, 4, 2,
			1, 2, 1,
		};

		static IntMatrix m(&d[0], 3, 3, 0.0625, false);

		return m;
	}

	static IntMatrix & Gauss_7_7() {
		static int d[] = {
			1, 3,  7,  9,  7,  3,  1,
			3, 12, 26, 33, 26, 12, 3,
			7, 26, 55, 70, 55, 26, 7,
			9, 33, 70, 90, 70, 33, 9,
			7, 26, 55, 70, 55, 26, 7,
			3, 12, 26, 33, 26, 12, 3,
			1, 3,  7,  9,  7,  3,  1,
		};

		static IntMatrix m(&d[0], 3, 3, 1.0 / 7.0 / 7.0, false);

		return m;
	}

	static IntMatrix & LOG_3_3() {
		static int d[] = {
			0, -1,  0,
			-1, 4, -1,
			0, -1,  0,
		};

		static IntMatrix m(&d[0], 3, 3, 0.5, false);

		return m;
	}
};

namespace c2_impl {
	template < typename SrcView, typename Matrix >
	int apply_matrix(const SrcView & s, const Matrix & m)
	{
		int w = m.w;
		int h = m.h;

		int col = 0;
		for (int y = 0; y < h; ++y) {
			typename SrcView::x_iterator it_s = s.row_begin(y);

			for (int x = 0; x < w; ++x) {
				col += it_s[x] * m.d[x * h + y];
			}
		}

		return col;
	}
}

template < typename SrcView, typename DstView, typename Matrix >
void apply_matrix(const SrcView & s, const DstView & d, const Matrix & m)
{
	assert(s.width()        == d.width());
	assert(s.height()       == d.height());
	assert(s.num_channels() == d.num_channels());

	int w = s.width();
	int h = s.height();

	if (w < m.w || h < m.h) {
		return; //cannot apply
	}

	int channels = s.num_channels();

#pragma omp parallel for
	for (int y = 0; y < h - m.h; ++y) {
		typename DstView::x_iterator it_d = d.row_begin(y);

		for (int x = 0; x < w - m.w; ++x) {
			for (int c = 0; c < channels; ++c) {
				int col = c2_impl::apply_matrix(
					nth_channel_view(subimage_view(s, 
						typename SrcView::point_t(x, y), 
						typename SrcView::point_t(m.w, m.h)), c), m);

				col = int((double)col * m.coef);
				it_d[x][c] = col;
			}
		}
	}
}
