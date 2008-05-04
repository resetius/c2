
#include <boost/gil/extension/io/jpeg_dynamic_io.hpp>

#include "c2_gradient.h"

template < typename Img, typename Img2 >
void abs_max_grad(const Img & img, Img2 & img_x, Img2 & img_y)
{
	gray8s_image_t img_out(img.dimensions());
	fill_pixels(view(img_out),bits8s(0));

	image_join(view(img_x), view(img_y), view(img_out),
		make_abs_max_cast_channels (view(img_x)));

	jpeg_write_view("out-abs_max_gradient.jpg",
		color_converted_view<gray8_pixel_t>(const_view(img_x)));
}

template < typename Img, typename Img2 >
void abs_sum_grad(const Img & img, Img2 & img_x, Img2 & img_y)
{
	gray8s_image_t img_out(img.dimensions());
	fill_pixels(view(img_out),bits8s(0));

	image_join(view(img_x), view(img_y), view(img_out),
		make_abs_sum_cast_channels (view(img_x)));

	jpeg_write_view("out-abs_sum_gradient.jpg",
		color_converted_view<gray8_pixel_t>(const_view(img_x)));
}

template < typename Img, typename Img2 >
void sqrt_grad(const Img & img, Img2 & img_x, Img2 & img_y)
{
	gray8s_image_t img_out(img.dimensions());
	fill_pixels(view(img_out),bits8s(0));

	image_join(view(img_x), view(img_y), view(img_out),
		make_sqrt_cast_channels (view(img_x)));

	jpeg_write_view("out-sqrt_gradient.jpg",
		color_converted_view<gray8_pixel_t>(const_view(img_out)));
}

template < typename Img, typename Img2 >
void sum_grad(const Img & img, Img2 & img_x, Img2 & img_y)
{
	gray8s_image_t img_out(img.dimensions());
	fill_pixels(view(img_out),bits8s(0));

	image_join(view(img_x), view(img_y), view(img_out),
		make_halfsum_cast_channels (view(img_x)));

	jpeg_write_view("out-sum_gradient.jpg",
		color_converted_view<gray8_pixel_t>(const_view(img_out)));
}

int main() {
	rgb8_image_t img;
	jpeg_read_image("test.jpg",img);

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

	jpeg_write_view("out-x_gradient.jpg",
		color_converted_view<gray8_pixel_t>(const_view(img_x)));

	jpeg_write_view("out-y_gradient.jpg",
		color_converted_view<gray8_pixel_t>(const_view(img_y)));

	return 0;
}
