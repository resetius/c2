#ifndef C2_CANNY_H
#define C2_CANNY_H
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

/**
 * Детектор краев Кенни
 */

#include "c2_gauss.h"

/**
 * Детектор краев Кенни. На вход view от любой картинки. 
 * На выходе view от черно-белой картинки
 * @param D -- "ширина" фильтра Гаусса. Может быть 3 или 7.
 */
template < int D >
struct Canny {
	template < typename SrcView, typename DstView >
	void operator () (const SrcView & s, const DstView & d)
	{
		assert(s.width()  == d.width());
		assert(s.height() == d.height());
		assert(d.num_channels() == 1);

		typedef pixel<typename channel_type<SrcView>::type, 
			gray_layout_t> gray_pixel_t;

		//Фильтр Гаусса.
		//Чем больше D тем меньше деталей будет в итоге
		Gauss < D > filter;
		filter(color_converted_view<gray_pixel_t>(s), view(d));

		//Оператор Робертса -> получаем максимум и направления градиента
		//Ищем локальные максимумы градиента
		//Ищем края
	}
};

#endif //C2_CANNY_H
