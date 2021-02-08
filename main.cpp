#include <iostream>

#include "test_runner.h"
#include "tests.h"
#include "travel_tracker.h"

using namespace std;

int main() {
	//setlocale(LC_CTYPE, "Russian");
	Tests::TestSvg1();
	Tests::TestSvg2();

	TravelManager();

	return 0;
}



