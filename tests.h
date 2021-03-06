#pragma once

#include <sstream>
#include <fstream>

#include "test_runner.h"
#include "travel_tracker.h"

namespace Tests {

	void TestGraphBuild() {
		istringstream in(R"({
  "routing_settings": {
    "bus_wait_time": 2,
    "bus_velocity": 30
  },
  "base_requests": [
    {
      "type": "Bus",
      "name": "297",
      "stops": [
        "Biryulyovo Zapadnoye",
        "Biryulyovo Tovarnaya",
        "Universam",
        "Biryusinka",
        "Apteka",
        "Biryulyovo Zapadnoye"
      ],
      "is_roundtrip": true
    },
    {
      "type": "Bus",
      "name": "635",
      "stops": [
        "Biryulyovo Tovarnaya",
        "Universam",
        "Biryusinka",
        "TETs 26",
        "Pokrovskaya",
        "Prazhskaya"
      ],
      "is_roundtrip": false
    },
    {
      "type": "Bus",
      "name": "828",
      "stops": [
        "Biryulyovo Zapadnoye",
        "TETs 26",
        "Biryusinka",
        "Universam",
        "Pokrovskaya",
        "Rossoshanskaya ulitsa"
      ],
      "is_roundtrip": false
    },
    {
      "type": "Stop",
      "road_distances": {
        "Biryulyovo Tovarnaya": 2600,
        "TETs 26": 1100
      },
      "longitude": 37.6517,
      "name": "Biryulyovo Zapadnoye",
      "latitude": 55.574371
    },
    {
      "type": "Stop",
      "road_distances": {
        "Biryusinka": 760,
        "Biryulyovo Tovarnaya": 1380,
        "Pokrovskaya": 2460
      },
      "longitude": 37.645687,
      "name": "Universam",
      "latitude": 55.587655
    },
    {
      "type": "Stop",
      "road_distances": {
        "Universam": 890
      },
      "longitude": 37.653656,
      "name": "Biryulyovo Tovarnaya",
      "latitude": 55.592028
    },
    {
      "type": "Stop",
      "road_distances": {
        "Apteka": 210,
        "TETs 26": 400
      },
      "longitude": 37.64839,
      "name": "Biryusinka",
      "latitude": 55.581065
    },
    {
      "type": "Stop",
      "road_distances": {
        "Biryulyovo Zapadnoye": 1420
      },
      "longitude": 37.652296,
      "name": "Apteka",
      "latitude": 55.580023
    },
    {
      "type": "Stop",
      "road_distances": {
        "Pokrovskaya": 2850
      },
      "longitude": 37.642258,
      "name": "TETs 26",
      "latitude": 55.580685
    },
    {
      "type": "Stop",
      "road_distances": {
        "Rossoshanskaya ulitsa": 3140
      },
      "longitude": 37.635517,
      "name": "Pokrovskaya",
      "latitude": 55.603601
    },
    {
      "type": "Stop",
      "road_distances": {
        "Pokrovskaya": 3210
      },
      "longitude": 37.605757,
      "name": "Rossoshanskaya ulitsa",
      "latitude": 55.595579
    },
    {
      "type": "Stop",
      "road_distances": {
        "Pokrovskaya": 2260
      },
      "longitude": 37.603938,
      "name": "Prazhskaya",
      "latitude": 55.611717
    },
    {
      "type": "Bus",
      "name": "750",
      "stops": [
        "Tolstopaltsevo",
        "Rasskazovka"
      ],
      "is_roundtrip": false
    },
    {
      "type": "Stop",
      "road_distances": {
        "Rasskazovka": 13800
      },
      "longitude": 37.20829,
      "name": "Tolstopaltsevo",
      "latitude": 55.611087
    },
    {
      "type": "Stop",
      "road_distances": {},
      "longitude": 37.333324,
      "name": "Rasskazovka",
      "latitude": 55.632761
    }
  ],
  "stat_requests": [
    {
      "type": "Bus",
      "name": "297",
      "id": 1
    },
    {
      "type": "Bus",
      "name": "635",
      "id": 2
    },
    {
      "type": "Bus",
      "name": "828",
      "id": 3
    },
    {
      "type": "Stop",
      "name": "Universam",
      "id": 4
    },
    {
      "type": "Route",
      "from": "Biryulyovo Zapadnoye",
      "to": "Apteka",
      "id": 5
    },
    {
      "type": "Route",
      "from": "Biryulyovo Zapadnoye",
      "to": "Pokrovskaya",
      "id": 6
    },
    {
      "type": "Route",
      "from": "Biryulyovo Tovarnaya",
      "to": "Pokrovskaya",
      "id": 7
    },
    {
      "type": "Route",
      "from": "Biryulyovo Tovarnaya",
      "to": "Biryulyovo Zapadnoye",
      "id": 8
    },
    {
      "type": "Route",
      "from": "Biryulyovo Tovarnaya",
      "to": "Prazhskaya",
      "id": 9
    },
    {
      "type": "Route",
      "from": "Apteka",
      "to": "Biryulyovo Tovarnaya",
      "id": 10
    },
    {
      "type": "Route",
      "from": "Rossoshanskaya ulitsa",
      "to": "Pokrovskaya",
      "id": 11
    }
  ]
})");

		//ofstream out("output2.txt");
		ostringstream out;
		const string answer = R"([
{
 "route_length":5880,
"request_id":1,
"curvature":1.36159,
"stop_count":6,
"unique_stop_count":5
},
{
 "route_length":14810,
"request_id":2,
"curvature":1.12195,
"stop_count":11,
"unique_stop_count":6
},
{
 "route_length":15790,
"request_id":3,
"curvature":1.31245,
"stop_count":11,
"unique_stop_count":6
},
{
"buses": [
"297",
"635",
"828"
],
"request_id":4
},
{
"request_id":5,
"total_time": 7.42,
"items": [
{
 "time": 2,
"stop_name": "Biryulyovo Zapadnoye",
"type": "Wait"
},
{
 "time": 3,
"bus": "828",
"span_count": 2,
"type": "Bus"
},
{
 "time": 2,
"stop_name": "Biryusinka",
"type": "Wait"
},
{
 "time": 0.42,
"bus": "297",
"span_count": 1,
"type": "Bus"
}
]},
{
"request_id":6,
"total_time": 11.44,
"items": [
{
 "time": 2,
"stop_name": "Biryulyovo Zapadnoye",
"type": "Wait"
},
{
 "time": 9.44,
"bus": "828",
"span_count": 4,
"type": "Bus"
}
]},
{
"request_id":7,
"total_time": 10.7,
"items": [
{
 "time": 2,
"stop_name": "Biryulyovo Tovarnaya",
"type": "Wait"
},
{
 "time": 1.78,
"bus": "635",
"span_count": 1,
"type": "Bus"
},
{
 "time": 2,
"stop_name": "Universam",
"type": "Wait"
},
{
 "time": 4.92,
"bus": "828",
"span_count": 1,
"type": "Bus"
}
]},
{
"request_id":8,
"total_time": 8.56,
"items": [
{
 "time": 2,
"stop_name": "Biryulyovo Tovarnaya",
"type": "Wait"
},
{
 "time": 6.56,
"bus": "297",
"span_count": 4,
"type": "Bus"
}
]},
{
"request_id":9,
"total_time": 16.32,
"items": [
{
 "time": 2,
"stop_name": "Biryulyovo Tovarnaya",
"type": "Wait"
},
{
 "time": 14.32,
"bus": "635",
"span_count": 5,
"type": "Bus"
}
]},
{
"request_id":10,
"total_time": 12.04,
"items": [
{
 "time": 2,
"stop_name": "Apteka",
"type": "Wait"
},
{
 "time": 2.84,
"bus": "297",
"span_count": 1,
"type": "Bus"
},
{
 "time": 2,
"stop_name": "Biryulyovo Zapadnoye",
"type": "Wait"
},
{
 "time": 5.2,
"bus": "297",
"span_count": 1,
"type": "Bus"
}
]},
{
"request_id":11,
"total_time": 8.42,
"items": [
{
 "time": 2,
"stop_name": "Rossoshanskaya ulitsa",
"type": "Wait"
},
{
 "time": 6.42,
"bus": "828",
"span_count": 1,
"type": "Bus"
}
]}
])";
		TravelManager(in, out);
		ASSERT_EQUAL(out.str(), answer);
	}

    void TestSvg1() {
        ifstream in("SVGtest1.txt");
        if (!in) {
            cerr << "file: " << "SVGtest1.txt" << " is not found." << endl;
            return;
        }
        TravelManager(in, cout);
    }

    void TestSvg2() {
        ifstream in("SVGtest2.txt");
        if (!in) {
            cerr << "file: " << "SVGtest2.txt" << " is not found." << endl;
            return;
        }
        TravelManager(in, cout);
    }

    void TestSvg3() {
        ifstream in("SVGtest3.txt");
        if (!in) {
            cerr << "file: " << "SVGtest3.txt" << " is not found." << endl;
            return;
        }
        TravelManager(in, cout);
    }

    void TestSvg10() {
        ifstream in("SVGtest10.txt");
        if (!in) {
            cerr << "file: " << "SVGtest10.txt" << " is not found." << endl;
            return;
        }
        TravelManager(in, cout);
    }

    void TestSvg10Sized() {
        ifstream in("SVGtest10Sized.txt");
        if (!in) {
            cerr << "file: " << "SVGtest10.txt" << " is not found." << endl;
            return;
        }
        TravelManager(in, cout);
    }

    void TestSvg5() {
        ifstream in("SVGtest5.txt");
        if (!in) {
            cerr << "file: " << "SVGtest5.txt" << " is not found." << endl;
            return;
        }
        TravelManager(in, cout);
    }

}
