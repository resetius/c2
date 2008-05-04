#ifndef C2_GRADIENT_H
#define C2_GRADIENT_H

#include <assert.h>

using namespace boost::gil;

template < typename Out >
struct halfdiff_cast_channels {
    template <typename T> Out operator()(const T& in1, const T& in2) const {
        return Out((in2-in1)/2);
    }
};


template <typename SrcView, typename DstView>
void x_gradient(const SrcView& src, const DstView& dst) {
    typedef typename channel_type<DstView>::type dst_channel_t;

    typename SrcView::x_coord_t h = src.height();
    typename SrcView::y_coord_t w = src.width();

#pragma omp parallel for
    for (int y = 0; y < h; ++y) {
        typename SrcView::x_iterator src_it = src.row_begin(y);
        typename DstView::x_iterator dst_it = dst.row_begin(y);

        for (int x = 1; x < w - 1; ++x) {
            static_transform(src_it[x - 1], src_it[x + 1], dst_it[x],
                             halfdiff_cast_channels<dst_channel_t>());
        }
    }
}

template <typename SrcView, typename DstView>
void y_gradient(const SrcView& src, const DstView& dst) {
    x_gradient(rotated90cw_view(src), rotated90cw_view(dst));
}

template <typename SrcView, typename DstView>
void x_luminosity_gradient(const SrcView& src, const DstView& dst) {
    typedef pixel<typename channel_type<SrcView>::type, gray_layout_t> gray_pixel_t;
    x_gradient(color_converted_view<gray_pixel_t>(src), dst);
}

template <typename SrcView, typename DstView>
void y_luminosity_gradient(const SrcView& src, const DstView& dst) {
    typedef pixel<typename channel_type<SrcView>::type, gray_layout_t> gray_pixel_t;
    y_gradient(color_converted_view<gray_pixel_t>(src), dst);
}

template <typename View_t >
void image_sum(const View_t & v1, const View_t & v2, const View_t & d ) {
    typedef typename channel_type<View_t>::type dst_channel_t;

    assert(v1.height() == v2.height() && v1.height() == d.height());
    assert(v1.width()  == v2.width()  && v1.width()  == d.width());

    int channels = v1.num_channels();

    typename View_t::x_coord_t h = v1.height();
    typename View_t::y_coord_t w = v1.width();

#pragma omp parallel for
    for (int y = 0; y < h; ++y) {
        typename View_t::x_iterator it_v1 = v1.row_begin(y);
        typename View_t::x_iterator it_v2 = v2.row_begin(y);
        typename View_t::x_iterator it_d  = d.row_begin(y);

        for (int x = 0 ; x < w; ++x) {
            static_transform(it_v1[x], it_v2[x], it_d[x],
                             halfdiff_cast_channels<dst_channel_t>());
        }
    }
}

#endif // C2_GRADIENT_H

