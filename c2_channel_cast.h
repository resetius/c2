#ifndef C2_CHANNEL_CAST_H
#define C2_CHANNEL_CAST_H
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

using namespace boost::gil;

template < typename Out >
struct halfdiff_cast_channels {
    template <typename T> Out operator()(const T& in1, const T& in2) const {
        return Out((in2 - in1) / 2);
    }
};

template < typename Out >
struct halfsum_cast_channels {
    template <typename T> Out operator()(const T& in1, const T& in2) const {
        return Out((in2 + in1) / 2);
    }
};

template < typename Out >
halfsum_cast_channels < typename channel_type< Out >::type > 
make_halfsum_cast_channels(Out)
{
	return halfsum_cast_channels < typename channel_type< Out >::type > ();
}

template < typename Out >
struct sqrt_cast_channels {
    template <typename T> Out operator()(const T& in1, const T& in2) const {
		return Out(sqrt((double)in2 * (double)in2 + (double)in1 * (double)in1));
    }
};

template < typename Out >
sqrt_cast_channels < typename channel_type< Out >::type > 
make_sqrt_cast_channels(Out)
{
	return sqrt_cast_channels < typename channel_type< Out >::type > ();
}

template < typename Out >
struct abs_max_cast_channels {
    template <typename T> Out operator()(const T& in1, const T& in2) const {
        return Out(std::max(std::abs(in2), std::abs(in1)));
    }
};

template < typename Out >
abs_max_cast_channels < typename channel_type< Out >::type > 
make_abs_max_cast_channels(Out)
{
	return abs_max_cast_channels < typename channel_type< Out >::type > ();
}

template < typename Out >
struct abs_sum_cast_channels {
    template <typename T> Out operator()(const T& in1, const T& in2) const {
        return Out((std::abs(in2) + std::abs(in1)) / 2);
    }
};

template < typename Out >
abs_sum_cast_channels < typename channel_type< Out >::type > 
make_abs_sum_cast_channels(Out)
{
	return abs_sum_cast_channels < typename channel_type< Out >::type > ();
}

#endif /* C2_CHANNEL_CAST_H */

