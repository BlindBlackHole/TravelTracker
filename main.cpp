#include <iostream>

#include "test_runner.h"
#include "tests.h"
#include "travel_tracker.h"

using namespace std;

int main() {
	//setlocale(LC_CTYPE, "Russian");
	TestRunner tr;
	tr.RunTest(Tests::TestGraphBuild, "Test#1");
	Tests::TestSvg();
	Tests::TestSvg2();

	TravelManager();
	return 0;
}



