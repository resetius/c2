#ifndef C2_CANNY_H
#define C2_CANNY_H
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

/**
 * Детектор краев Кенни
 */
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

#include "c2_gauss.h"

/**
 * Детектор краев Кенни. На вход view от любой картинки. 
 * На выходе view от черно-белой картинки
 * @param D -- "ширина" фильтра Гаусса. Может быть 3 или 7.
 */
template < int D >
struct Canny {
	int direction(double angle)
	{
		while (angle > 2 * M_PI) {
			angle -= 2 * M_PI;
		}

		if (angle < M_PI / 4 || angle > M_PI + 2 * M_PI + M_PI / 2) {
			// right
			return 0;
		} else if (angle < M_PI / 2 + M_PI / 4) {
			// up
			return 1;
		} else if (angle < M_PI + M_PI / 4) {
			// left
			return 2;
		} else {
			// down
			return 3;
		}
	}

	template < typename View >
	void supress_nonmaxima(const View & grad, const View & grad_x, const View & grad_y)
	{
		int w = grad.width();
		int h = grad.height();

		int pdir [][2] = 
		{
			{1, 0},
			{1, 1},
			{0, 1},
			{-1, 1},
		};

		int mdir [][2] = 
		{
			{-1, 0},
			{-1, -1},
			{0, -1},
			{1, -1},
		};

		for (int y = 0; y < h; ++y) {
			typename View::x_iterator it_grad   = grad.row_begin(y);
			typename View::x_iterator it_grad_x = grad_x.row_begin(y);
			typename View::x_iterator it_grad_y = grad_y.row_begin(y);

			for (int x = 0; x < w; ++x) {
				double angle;
				int dir;
				int dir_x, dir_y;

				if (it_grad_x[y] == 0) {
					angle = M_PI / 2.0;
				} else {
					angle = atan(fabs((double)it_grad_y[x] / (double)it_grad_x[y]));

					if (it_grad_x[y] < 0 && it_grad_y[x]) {
						angle += M_PI;
					} else if (it_grad_y[y] < 0) {
						angle += M_PI + M_PI / 2.0;
					} else if (it_grad_x[y] < 0) {
						angle += M_PI / 2.0;
					}
				}

				angle += M_PI / 8;
				dir = direction(angle);

				dir_x = x + pdir[dir][0];
				dir_y = y + pdir[dir][1];

				if (dir_x >= 0 && dir_x < w
					&& dir_y >= 0 && dir_y < h)
				{
					if (it_grad[x] <= grad.row_begin(dir_y)[dir_x])
					{
						it_grad[x] = 0;
					}
				}

				dir_x = x + mdir[dir][0];
				dir_y = y + mdir[dir][1];

				if (dir_x >= 0 && dir_x < w
					&& dir_y >= 0 && dir_y < h)
				{
					if (it_grad[x] <= grad.row_begin(dir_y)[dir_x]) {
						it_grad[x] = 0;
					}
				}
			}
		}
	}

	template < typename SrcView, typename DstView >
	void operator () (const SrcView & s, const DstView & d, 
		int T_low, int T_high)
	{
		assert(s.width()  == d.width());
		assert(s.height() == d.height());
		assert(d.num_channels() == 1);

		typedef pixel<typename channel_type<SrcView>::type, 
			gray_layout_t> gray_pixel_t;

		{
			//Фильтр Гаусса.
			//Чем больше D тем меньше деталей будет в итоге
			Gauss < D > filter;
			//filter(color_converted_view<gray_pixel_t>(s), view(d));
			filter(s, d);
		}

		{
			//Оператор Робертса -> получаем максимум и направления градиента
			gray8s_image_t grad(gray8s_image_t::point_t(s.width(), s.height()));
			fill_pixels(view(grad),bits8s(0));

			gray8s_image_t grad_x(gray8s_image_t::point_t(s.width(), s.height()));
			fill_pixels(view(grad_x),bits8s(0));

			gray8s_image_t grad_y(gray8s_image_t::point_t(s.width(), s.height()));
			fill_pixels(view(grad_y),bits8s(0));

			apply_matrix(color_converted_view<gray_pixel_t>(d),
				view(grad_x), IntMatrix::Roberts_x());
			apply_matrix(color_converted_view<gray_pixel_t>(d),
				view(grad_y), IntMatrix::Roberts_y());

			image_join(const_view(grad_x), const_view(grad_y), view(grad),
				//make_abs_sum_cast_channels (const_view(grad_x)));
				make_sqrt_cast_channels (const_view(grad_x)));

			//Ищем локальные максимумы градиента
			supress_nonmaxima(view(grad), view(grad_x), view(grad_y));

			//Ищем края
			std::vector < char > E(s.width() * s.height());
			edge_detect(const_view(grad), T_low, T_high, E);
		}
	}

	template < typename View , typename Edges >
	void edge_detect(const View & grad, int T_low, int T_high, Edges & E)
	{
		int w = grad.width();
		int h = grad.height();

		for (int y = 0; y < h; ++y) {
			typename View::x_iterator it_grad   = grad.row_begin(y);
			for (int x = 0; x < w; ++x) {
				if (it_grad[x] >= T_high) {
					follow_edge(x, y, grad, T_low, T_high, E);
				}
			}
		}
	}

	template < typename View , typename Edges >
	void follow_edge(int x, int y, const View & grad, 
		int T_low, int T_high, Edges & E)
	{
		int w = grad.width();
		int h = grad.height();
		int neib [][2] = {
			{1, 0},
			{0, 1},
			{-1, 0},
			{0, -1},

			{1, 1},
			{1, -1},
			{-1, 1},
			{-1, -1},
		};

		int n_neub = 8;

		E[y * w + x] = 1;

		int flag = true;
		while (flag) {
			flag = false;
			for (int i = 0; i < n_neub; ++i) {
				int u = x + neib[i][0];
				int v = y + neib[i][1];

				if (u < 0 || u >= w) {
					continue;
				}

				if (v < 0 || v >= h) {
					continue;
				}

				if (grad.row_begin(v)[u] < T_low) {
					continue;
				}

				flag = true;
				x = u; y = v;
				E[y * w + x] = 1;
				break;
			}
		}
	}
};

#endif //C2_CANNY_H
