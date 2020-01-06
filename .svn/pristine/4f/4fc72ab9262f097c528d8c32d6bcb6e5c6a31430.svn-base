#ifndef BOOST_SPREADSORT_CONSTANTS
#define BOOST_SPREADSORT_CONSTANTS
namespace boost {
namespace detail {
//Tuning constants
//Sets the minimum number of items per bin.
static const unsigned LOG_MEAN_BIN_SIZE = 2;
//This should be tuned to your processor cache; if you go too large you get cache misses on bins
//The smaller this number, the less worst-case memory usage.  If too small, too many recursions slow down spreadsort
static const unsigned MAX_SPLITS = 10;
//Used to force a comparison-based sorting for small bins, if it's faster.  Minimum value 0
static const unsigned LOG_MIN_SPLIT_COUNT = 5;
//There is a minimum size below which it is not worth using spreadsort
static const long MIN_SORT_SIZE = 1000;
//This is the constant on the log base n of m calculation; make this larger the faster std::sort is relative to spreadsort
static const unsigned LOG_CONST = 2;
}
}
#endif
