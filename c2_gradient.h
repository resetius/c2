#ifndef C2_GRADIENT_H
#define C2_GRADIENT_H
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
#include <math.h>

#include "c2_channel_cast.h"

using namespace boost::gil;

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

template <typename SrcView, typename DstView, typename Join_t >
void image_join(const SrcView & v1, const SrcView & v2, 
				const DstView & d,  Join_t f) 
{
    assert(v1.height() == v2.height() && v1.height() == d.height());
    assert(v1.width()  == v2.width()  && v1.width()  == d.width());

    int channels = v1.num_channels();

    typename SrcView::x_coord_t h = v1.height();
    typename SrcView::y_coord_t w = v1.width();

#pragma omp parallel for
    for (int y = 0; y < h; ++y) {
        typename SrcView::x_iterator it_v1 = v1.row_begin(y);
        typename SrcView::x_iterator it_v2 = v2.row_begin(y);
        typename DstView::x_iterator it_d  = d.row_begin(y);

        for (int x = 0 ; x < w; ++x) {
            static_transform(it_v1[x], it_v2[x], it_d[x], f);
        }
    }
}

#endif // C2_GRADIENT_H

