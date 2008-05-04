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

	IntMatrix() : w(0), h(0), d(0) {}
	IntMatrix(int w1, int h1): w(w1), h(h1), d(new int[w * h]) {}
	~IntMatrix() { delete [] d; }

	static IntMatrix * Soebel(int w, int h) {
		int d[] = {
			1, 2, 1,
			0, 0, 0,
			1, 2, 1,
		};

		IntMatrix * m = new IntMatrix(3, 3);
		memcpy(m->d, d, 3 * 3 * sizeof(int));

		return m;
	}
};

namespace c2_impl {
	template < typename SrcView, typename DstView, typename Matrix >
	void apply_matrix(const SrcView & s, const DstView & d, 
		const Matrix & m)
	{
		int channels = s.num_channels();
		int w = m.w;
		int h = m.h;

		for (int y = 0; y < h; ++y) {
			for (int c = 0; c < channels; ++c) {
				typename SrcView::x_iterator it_s = s.row_begin(y);
				typename DstView::x_iterator it_d = d.row_begin(y);

				for (int x = 0; x < w; ++x) {
					it_d[x][c] += it_s[x][c] * m.d[x * h + y];
				}
			}
		}
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

	for (int x = 0; x < w - m.w; ++x) {
		for (int y = 0; y < h - m.h; ++y) {
			c2_impl::apply_matrix(
				subimage_view(s, 
					typename SrcView::point_t(x, y), 
					typename SrcView::point_t(m.w, m.h)), 
				subimage_view(d, 
					typename DstView::point_t(x, y),
					typename DstView::point_t(m.w, m.h)), m);
		}
	}
}
