
#include <boost/gil/extension/io/jpeg_dynamic_io.hpp>

#include "c2_gradient.h"

int main() {
    rgb8_image_t img;
    jpeg_read_image("test.jpg",img);
    
    gray8s_image_t img_out(img.dimensions());
    fill_pixels(view(img_out),bits8s(0));

    gray8s_image_t img_x(img.dimensions());
    gray8s_image_t img_y(img.dimensions());
    fill_pixels(view(img_x),bits8s(0));
    fill_pixels(view(img_y),bits8s(0));

    x_luminosity_gradient(const_view(img), view(img_x));
    y_luminosity_gradient(const_view(img), view(img_y));

    image_sum(view(img_x), view(img_y), view(img_out));

    jpeg_write_view("out-x_gradient.jpg",color_converted_view<gray8_pixel_t>(const_view(img_out)));

    return 0;
}

