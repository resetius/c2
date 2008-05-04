#ifndef C2_CHANNEL_CAST_H
#define C2_CHANNEL_CAST_H

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
		return Out(sqrt(double(in2 * in2 + in1 * in1)));
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
