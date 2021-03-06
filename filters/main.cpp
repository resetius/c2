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

#include <boost/gil/extension/io/jpeg_dynamic_io.hpp>

#include "c2_gradient.h"
#include "c2_matrix.h"
#include "c2_gauss.h"
#include "c2_canny.h"

template < typename Img, typename Img2 >
void abs_max_grad(const Img & img, const Img2 & img_x, const Img2 & img_y)
{
	gray8s_image_t img_out(img.dimensions());
	fill_pixels(view(img_out),bits8s(0));

	image_join(const_view(img_x), const_view(img_y), view(img_out),
		make_abs_max_cast_channels (view(img_out)));

	jpeg_write_view("out-abs_max_gradient.jpg",
		color_converted_view<gray8_pixel_t>(const_view(img_out)));
}

template < typename Img, typename Img2 >
void abs_sum_grad(const Img & img, const Img2 & img_x, const Img2 & img_y)
{
	gray8s_image_t img_out(img.dimensions());
	fill_pixels(view(img_out),bits8s(0));

	image_join(const_view(img_x), const_view(img_y), view(img_out),
		make_abs_sum_cast_channels (view(img_out)));

	jpeg_write_view("out-abs_sum_gradient.jpg",
		color_converted_view<gray8_pixel_t>(const_view(img_out)));
}

template < typename Img, typename Img2 >
void sqrt_grad(const Img & img,  const Img2 & img_x, const Img2 & img_y)
{
	gray8s_image_t img_out(img.dimensions());
	fill_pixels(view(img_out),bits8s(0));

	image_join(const_view(img_x), const_view(img_y), view(img_out),
		make_sqrt_cast_channels (const_view(img_x)));

	jpeg_write_view("out-sqrt_gradient.jpg",
		color_converted_view<gray8_pixel_t>(const_view(img_out)));
}

template < typename Img, typename Img2 >
void sum_grad(const Img & img, const Img2 & img_x, const Img2 & img_y)
{
	gray8s_image_t img_out(img.dimensions());
	fill_pixels(view(img_out),bits8s(0));

	image_join(const_view(img_x), const_view(img_y), view(img_out),
		make_halfsum_cast_channels (view(img_out)));

	jpeg_write_view("out-sum_gradient.jpg",
		color_converted_view<gray8_pixel_t>(const_view(img_out)));
}

template < typename View >
void soebel(const View & v)
{
	gray8s_image_t img_out(gray8s_image_t::point_t(v.width(), v.height()));
	fill_pixels(view(img_out),bits8s(0));

	gray8s_image_t img_x(gray8s_image_t::point_t(v.width(), v.height()));
	fill_pixels(view(img_x),bits8s(0));

	gray8s_image_t img_y(gray8s_image_t::point_t(v.width(), v.height()));
	fill_pixels(view(img_y),bits8s(0));

	typedef pixel<typename channel_type<View>::type, 
		gray_layout_t> gray_pixel_t;

	apply_matrix(color_converted_view<gray_pixel_t>(v),
		view(img_x), IntMatrix::Soebel_x());
//	jpeg_write_view("out-soebel_x.jpg",
//		color_converted_view<gray_pixel_t>(const_view(img_x)));

	apply_matrix(color_converted_view<gray_pixel_t>(v),
		view(img_y), IntMatrix::Soebel_y());
//	jpeg_write_view("out-soebel_y.jpg",
//		color_converted_view<gray_pixel_t>(const_view(img_y)));

	image_join(const_view(img_x), const_view(img_y), view(img_out),
		make_abs_sum_cast_channels (const_view(img_out)));

	jpeg_write_view("out-soebel.jpg",
		color_converted_view<gray_pixel_t>(const_view(img_out)));
}

template < typename View >
void previt(const View & v)
{
	gray8s_image_t img_out(gray8s_image_t::point_t(v.width(), v.height()));
	fill_pixels(view(img_out),bits8s(0));

	gray8s_image_t img_x(gray8s_image_t::point_t(v.width(), v.height()));
	fill_pixels(view(img_x),bits8s(0));

	gray8s_image_t img_y(gray8s_image_t::point_t(v.width(), v.height()));
	fill_pixels(view(img_y),bits8s(0));

	typedef pixel<typename channel_type<View>::type, 
		gray_layout_t> gray_pixel_t;

	apply_matrix(color_converted_view<gray_pixel_t>(v),
		view(img_x), IntMatrix::Previt_x());
	apply_matrix(color_converted_view<gray_pixel_t>(v),
		view(img_y), IntMatrix::Previt_y());
	image_join(const_view(img_x), const_view(img_y), view(img_out),
		make_abs_sum_cast_channels (const_view(img_x)));

	jpeg_write_view("out-previt.jpg",
		color_converted_view<gray8_pixel_t>(const_view(img_out)));
}

template < typename View >
void roberts(const View & v)
{
	gray8s_image_t img_out(gray8s_image_t::point_t(v.width(), v.height()));
	fill_pixels(view(img_out),bits8s(0));

	gray8s_image_t img_x(gray8s_image_t::point_t(v.width(), v.height()));
	fill_pixels(view(img_x),bits8s(0));

	gray8s_image_t img_y(gray8s_image_t::point_t(v.width(), v.height()));
	fill_pixels(view(img_y),bits8s(0));

	typedef pixel<typename channel_type<View>::type, 
		gray_layout_t> gray_pixel_t;

	apply_matrix(color_converted_view<gray_pixel_t>(v),
		view(img_x), IntMatrix::Roberts_x());
	apply_matrix(color_converted_view<gray_pixel_t>(v),
		view(img_y), IntMatrix::Roberts_y());
	image_join(const_view(img_x), const_view(img_y), view(img_out),
		make_abs_sum_cast_channels (const_view(img_x)));

	jpeg_write_view("out-roberts.jpg",
		color_converted_view<gray8_pixel_t>(const_view(img_out)));
}

template < typename View >
void log_3_3(const View & v)
{
	gray8s_image_t img_out(gray8s_image_t::point_t(v.width(), v.height()));
	fill_pixels(view(img_out),bits8s(0));

	typedef pixel<typename channel_type<View>::type, 
		gray_layout_t> gray_pixel_t;

	apply_matrix(color_converted_view<gray_pixel_t>(v),
		view(img_out), IntMatrix::LOG_3_3());

	jpeg_write_view("out-log_3_3.jpg",
		color_converted_view<gray8_pixel_t>(const_view(img_out)));
}

template < typename View >
void log_11_11(const View & v)
{
	gray8s_image_t img_out(gray8s_image_t::point_t(v.width(), v.height()));
	fill_pixels(view(img_out),bits8s(0));

	typedef pixel<typename channel_type<View>::type, 
		gray_layout_t> gray_pixel_t;

	apply_matrix(color_converted_view<gray_pixel_t>(v),
		view(img_out), IntMatrix::LOG_11_11());

	jpeg_write_view("out-log_11_11.jpg",
		color_converted_view<gray8_pixel_t>(const_view(img_out)));
}

template < typename View >
void gauss_3_3(const View & v)
{
	gray8s_image_t img_out(gray8s_image_t::point_t(v.width(), v.height()));
	fill_pixels(view(img_out),bits8s(0));

	//	jpeg_write_view("out-tttt.jpg",
	//		color_converted_view<gray8_pixel_t>(v));

	typedef pixel<typename channel_type<View>::type, 
		gray_layout_t> gray_pixel_t;

	Gauss < 3 > filter;
	filter(color_converted_view<gray_pixel_t>(v), view(img_out));

	//	transform_1(color_converted_view<gray8_pixel_t>(v),
	//		view(img_out));

	//	copy_and_convert_pixels(color_converted_view<gray8_pixel_t>(v),
	//		view(img_out));

	jpeg_write_view("out-gauss_3_3.jpg",
		color_converted_view<gray8_pixel_t>(const_view(img_out)));
}

template < typename View >
void gauss_7_7(const View & v)
{
	gray8s_image_t img_out(gray8s_image_t::point_t(v.width(), v.height()));
	fill_pixels(view(img_out),bits8s(0));

	typedef pixel<typename channel_type<View>::type, 
		gray_layout_t> gray_pixel_t;

	Gauss < 7 > filter;
	filter(color_converted_view<gray_pixel_t>(v), view(img_out));

	jpeg_write_view("out-gauss_7_7.jpg",
		color_converted_view<gray8_pixel_t>(const_view(img_out)));
}

template < typename View >
void gauss_3_3_color(const View & v)
{
	rgb8_image_t img_out(rgb8_image_t::point_t(v.width(), v.height()));

	apply_matrix(v, view(img_out), IntMatrix::Gauss_3_3(), -1, true);

	jpeg_write_view("out-gauss_3_3_color.jpg", const_view(img_out));
}

template < typename View >
void gauss_7_7_color(const View & v)
{
	rgb8_image_t img_out(rgb8_image_t::point_t(v.width(), v.height()));
//	fill_pixels(view(img_out),bits8s(0));

	apply_matrix(v,	view(img_out), IntMatrix::Gauss_7_7(), -1, true);

	jpeg_write_view("out-gauss_7_7_color.jpg", const_view(img_out));
}

int main() {
	rgb8_image_t img;
	jpeg_read_image("test.jpg", img);

	jpeg_write_view("out-lum.jpg",
		color_converted_view<gray8_pixel_t>(const_view(img)));

	gray8s_image_t img_x(img.dimensions());
	gray8s_image_t img_y(img.dimensions());
	fill_pixels(view(img_x),bits8s(0));
	fill_pixels(view(img_y),bits8s(0));

	x_luminosity_gradient(const_view(img), view(img_x));
	y_luminosity_gradient(const_view(img), view(img_y));

	abs_max_grad(img, img_x, img_y);
	abs_sum_grad(img, img_x, img_y);
	sqrt_grad(img, img_x, img_y);
	sum_grad(img, img_x, img_y);

	soebel(const_view(img));
	previt(const_view(img));
	roberts(const_view(img));
	log_3_3(const_view(img));
	log_11_11(const_view(img));
	gauss_3_3(const_view(img));
	gauss_3_3_color(const_view(img));
	gauss_7_7(const_view(img));
	gauss_7_7_color(const_view(img));

	Canny < 3 > c;
	c(const_view(img), view(img_x), 10, 10);

	jpeg_write_view("out-x_gradient.jpg",
		color_converted_view<gray8_pixel_t>(const_view(img_x)));

	jpeg_write_view("out-y_gradient.jpg",
		color_converted_view<gray8_pixel_t>(const_view(img_y)));

	return 0;
}
