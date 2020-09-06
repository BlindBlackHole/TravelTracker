#include <iostream>

#include "test_runner.h"
#include "tests.h"
#include "travel_tracker.h"

using namespace std;

int main() {
	TestRunner tr;
	tr.RunTest(Tests::TestGraphBuild, "Test#1");

	TravelManager();
	return 0;
}



