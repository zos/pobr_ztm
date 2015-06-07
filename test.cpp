/*
 *      Author: Zofia Abramowska
 *		File: test.cpp
 */

#include "Common.h"
#define BOOST_TEST_MODULE MyTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE( commons_test_suite )



BOOST_AUTO_TEST_CASE( mask_3x3_test )
{
	std::vector<std::vector<int>> mask =
	{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

	int row = 10, col = 10;

	int iterations = 0;
	for_mask(row, col, mask, [row, col, &mask, &iterations](int r, int c, int m) {
		static int row_good = row - 1;
		static int col_good = col - 1;
		static int mask_good = mask[0][0];
		BOOST_REQUIRE_EQUAL(row_good, r);
		BOOST_REQUIRE_EQUAL(col_good, c);
		BOOST_REQUIRE_EQUAL(mask_good, m);

		col_good++;
		if (col_good > col + 1) {
			col_good = col - 1;
			row_good++;
		}
		mask_good++;
		iterations++;
		return true;
	});


    BOOST_REQUIRE_EQUAL(iterations, 9);
}
BOOST_AUTO_TEST_SUITE_END()
