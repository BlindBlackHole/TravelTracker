#pragma once

#include <sstream>

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

	void TestSvg() {
		istringstream in(R"({
    "routing_settings": {
        "bus_wait_time": 2,
        "bus_velocity": 30
    },
    "render_settings": {
        "width": 1200,
        "height": 1200,
        "padding": 50,
        "stop_radius": 5,
        "line_width": 14,
        "stop_label_font_size": 20,
        "stop_label_offset": [
            7,
            -3
        ],
        "underlayer_color": [
            255,
            255,
            255,
            0.85
        ],
        "underlayer_width": 3,
        "color_palette": [
            "green",
            [
                255,
                160,
                0
            ],
            "red"
        ]
    },
    "base_requests": [
        {
            "type": "Bus",
            "name": "14",
            "stops": [
                "Liza Chaikina street",
                "Electroseti",
                "Riverskyi Most",
                "Gostinica Sochy",
                "Kybanska street",
                "Po trebovaniy",
                "Dokycheva street",
                "Liza Chaikina street"
            ],
            "is_roundtrip": true
        },
        {
            "type": "Bus",
            "name": "24",
            "stops": [
                "Dokycheva street",
                "Parallelnaya street",
                "Electroseti",
                "Sanatoriy Rodina"
            ],
            "is_roundtrip": false
        },
        {
            "type": "Bus",
            "name": "114",
            "stops": [
                "Morskoy Vikzal",
                "Riverskyi Most"
            ],
            "is_roundtrip": false
        },
        {
            "type": "Stop",
            "name": "Liza Chaikina street",
            "latitude": 43.590317,
            "longitude": 39.746833,
            "road_distances": {
                "Electroseti": 4300,
                "Dokycheva street": 2000
            }
        },
        {
            "type": "Stop",
            "name": "Morskoy Vikzal",
            "latitude": 43.581969,
            "longitude": 39.719848,
            "road_distances": {
                "Riverskyi Most": 850
            }
        },
        {
            "type": "Stop",
            "name": "Electroseti",
            "latitude": 43.598701,
            "longitude": 39.730623,
            "road_distances": {
                "Sanatoriy Rodina": 4500,
                "Parallelnaya street": 1200,
                "Riverskyi Most": 1900
            }
        },
        {
            "type": "Stop",
            "name": "Riverskyi Most",
            "latitude": 43.587795,
            "longitude": 39.716901,
            "road_distances": {
                "Morskoy Vikzal": 850,
                "Gostinica Sochy": 1740
            }
        },
        {
            "type": "Stop",
            "name": "Gostinica Sochy",
            "latitude": 43.578079,
            "longitude": 39.728068,
            "road_distances": {
                "Kybanska street": 320
            }
        },
        {
            "type": "Stop",
            "name": "Kybanska street",
            "latitude": 43.578509,
            "longitude": 39.730959,
            "road_distances": {
                "Po trebovaniy": 370
            }
        },
        {
            "type": "Stop",
            "name": "Po trebovaniy",
            "latitude": 43.579285,
            "longitude": 39.733742,
            "road_distances": {
                "Dokycheva street": 600
            }
        },
        {
            "type": "Stop",
            "name": "Dokycheva street",
            "latitude": 43.585586,
            "longitude": 39.733879,
            "road_distances": {
                "Parallelnaya street": 1100
            }
        },
        {
            "type": "Stop",
            "name": "Parallelnaya street",
            "latitude": 43.590041,
            "longitude": 39.732886,
            "road_distances": {}
        },
        {
            "type": "Stop",
            "name": "Sanatoriy Rodina",
            "latitude": 43.601202,
            "longitude": 39.715498,
            "road_distances": {}
        }
    ],
    "stat_requests": [
        {
            "id": 826874078,
            "type": "Bus",
            "name": "14"
        },
        {
            "id": 1086967114,
            "type": "Route",
            "from": "Morskoy Vikzal",
            "to": "Parallelnaya street"
        },
        {
            "id": 1218663236,
            "type": "Map"
        }
    ]
})");
	ostringstream out;
	TravelManager(in, cout);
	}

	void TestSvg2() {
		istringstream in(R"({
    "stat_requests": [
        {
            "type": "Map",
            "id": 1821171961
        }
    ],
    "render_settings": {
        "underlayer_width": 3,
        "height": 950,
        "stop_radius": 3,
        "underlayer_color": [
            255,
            255,
            255,
            0.85
        ],
        "width": 1500,
        "stop_label_font_size": 13,
        "padding": 50,
        "color_palette": [
            "red",
            "green",
            "blue",
            "brown",
            "orange"
        ],
        "stop_label_offset": [
            7,
            -3
        ],
        "line_width": 10
    },
    "base_requests": [
        {
            "stops": [
                "Санаторий Салют",
                "Санаторная улица",
                "Пансионат Нева",
                "Санаторий Радуга",
                "Санаторий Родина",
                "Спортивная",
                "Парк Ривьера",
                "Морской вокзал",
                "Органный зал",
                "Театральная",
                "Пансионат Светлана",
                "Цирк",
                "Стадион",
                "Санаторий Металлург",
                "Улица Бытха"
            ],
            "name": "23",
            "type": "Bus",
            "is_roundtrip": false
        },
        {
            "stops": [
                "Улица Лизы Чайкиной",
                "Пионерская улица, 111",
                "Садовая",
                "Театральная"
            ],
            "name": "13",
            "type": "Bus",
            "is_roundtrip": false
        },
        {
            "stops": [
                "Морской вокзал",
                "Сбербанк",
                "Автовокзал",
                "Отель Звёздный",
                "Магазин Быт",
                "Хлебозавод",
                "Кинотеатр Юбилейный",
                "Новая Заря",
                "Деревообр. комбинат",
                "Целинная улица, 5",
                "Целинная улица, 57",
                "Целинная улица"
            ],
            "name": "36",
            "type": "Bus",
            "is_roundtrip": false
        },
        {
            "stops": [
                "Пансионат Светлана",
                "Улица Лысая Гора",
                "Улица В. Лысая Гора"
            ],
            "name": "44к",
            "type": "Bus",
            "is_roundtrip": false
        },
        {
            "stops": [
                "Краево-Греческая улица",
                "Улица Бытха",
                "Санаторий им. Ворошилова",
                "Санаторий Приморье",
                "Санаторий Заря",
                "Мацеста",
                "Мацестинская долина"
            ],
            "name": "90",
            "type": "Bus",
            "is_roundtrip": false
        },
        {
            "name": "Краево-Греческая улица",
            "latitude": 43.565551,
            "type": "Stop",
            "longitude": 39.776858,
            "road_distances": {
                "Улица Бытха": 1780
            }
        },
        {
            "name": "Санаторий им. Ворошилова",
            "latitude": 43.557935,
            "type": "Stop",
            "longitude": 39.764452,
            "road_distances": {
                "Санаторий Приморье": 950
            }
        },
        {
            "name": "Санаторий Приморье",
            "latitude": 43.554202,
            "type": "Stop",
            "longitude": 39.77256,
            "road_distances": {
                "Санаторий Заря": 2350
            }
        },
        {
            "name": "Санаторий Заря",
            "latitude": 43.549618,
            "type": "Stop",
            "longitude": 39.780908,
            "road_distances": {
                "Мацеста": 800
            }
        },
        {
            "name": "Мацеста",
            "latitude": 43.545509,
            "type": "Stop",
            "longitude": 39.788993,
            "road_distances": {
                "Мацестинская долина": 2350
            }
        },
        {
            "name": "Мацестинская долина",
            "latitude": 43.560422,
            "type": "Stop",
            "longitude": 39.798219,
            "road_distances": {}
        },
        {
            "name": "Улица Лысая Гора",
            "latitude": 43.577997,
            "type": "Stop",
            "longitude": 39.741685,
            "road_distances": {
                "Улица В. Лысая Гора": 640
            }
        },
        {
            "name": "Улица В. Лысая Гора",
            "latitude": 43.58092,
            "type": "Stop",
            "longitude": 39.744749,
            "road_distances": {}
        },
        {
            "name": "Морской вокзал",
            "latitude": 43.581969,
            "type": "Stop",
            "longitude": 39.719848,
            "road_distances": {
                "Сбербанк": 870,
                "Органный зал": 570
            }
        },
        {
            "name": "Сбербанк",
            "latitude": 43.585969,
            "type": "Stop",
            "longitude": 39.725175,
            "road_distances": {
                "Автовокзал": 870
            }
        },
        {
            "name": "Автовокзал",
            "latitude": 43.592956,
            "type": "Stop",
            "longitude": 39.727798,
            "road_distances": {
                "Отель Звёздный": 700
            }
        },
        {
            "name": "Отель Звёздный",
            "latitude": 43.596585,
            "type": "Stop",
            "longitude": 39.721151,
            "road_distances": {
                "Магазин Быт": 1000
            }
        },
        {
            "name": "Магазин Быт",
            "latitude": 43.604025,
            "type": "Stop",
            "longitude": 39.724492,
            "road_distances": {
                "Хлебозавод": 420
            }
        },
        {
            "name": "Хлебозавод",
            "latitude": 43.607364,
            "type": "Stop",
            "longitude": 39.726643,
            "road_distances": {
                "Кинотеатр Юбилейный": 2110
            }
        },
        {
            "name": "Кинотеатр Юбилейный",
            "latitude": 43.623382,
            "type": "Stop",
            "longitude": 39.720626,
            "road_distances": {
                "Новая Заря": 450
            }
        },
        {
            "name": "Новая Заря",
            "latitude": 43.626842,
            "type": "Stop",
            "longitude": 39.717802,
            "road_distances": {
                "Деревообр. комбинат": 530
            }
        },
        {
            "name": "Деревообр. комбинат",
            "latitude": 43.631035,
            "type": "Stop",
            "longitude": 39.714624,
            "road_distances": {
                "Целинная улица, 5": 840
            }
        },
        {
            "name": "Целинная улица, 5",
            "latitude": 43.633353,
            "type": "Stop",
            "longitude": 39.710257,
            "road_distances": {
                "Целинная улица, 57": 1270
            }
        },
        {
            "name": "Целинная улица, 57",
            "latitude": 43.640536,
            "type": "Stop",
            "longitude": 39.713253,
            "road_distances": {
                "Целинная улица": 1050
            }
        },
        {
            "name": "Целинная улица",
            "latitude": 43.647968,
            "type": "Stop",
            "longitude": 39.717733,
            "road_distances": {}
        },
        {
            "name": "Санаторий Салют",
            "latitude": 43.623238,
            "type": "Stop",
            "longitude": 39.704646,
            "road_distances": {
                "Санаторная улица": 1500
            }
        },
        {
            "name": "Санаторная улица",
            "latitude": 43.620766,
            "type": "Stop",
            "longitude": 39.719058,
            "road_distances": {
                "Пансионат Нева": 670
            }
        },
        {
            "name": "Пансионат Нева",
            "latitude": 43.614288,
            "type": "Stop",
            "longitude": 39.718674,
            "road_distances": {
                "Санаторий Радуга": 520
            }
        },
        {
            "name": "Санаторий Радуга",
            "latitude": 43.609951,
            "type": "Stop",
            "longitude": 39.72143,
            "road_distances": {
                "Санаторий Родина": 1190
            }
        },
        {
            "name": "Санаторий Родина",
            "latitude": 43.601202,
            "type": "Stop",
            "longitude": 39.715498,
            "road_distances": {
                "Спортивная": 1100
            }
        },
        {
            "name": "Спортивная",
            "latitude": 43.593689,
            "type": "Stop",
            "longitude": 39.717642,
            "road_distances": {
                "Парк Ривьера": 640
            }
        },
        {
            "name": "Парк Ривьера",
            "latitude": 43.588296,
            "type": "Stop",
            "longitude": 39.715956,
            "road_distances": {
                "Морской вокзал": 730
            }
        },
        {
            "name": "Органный зал",
            "latitude": 43.57926,
            "type": "Stop",
            "longitude": 39.725574,
            "road_distances": {
                "Театральная": 770
            }
        },
        {
            "name": "Пансионат Светлана",
            "latitude": 43.571807,
            "type": "Stop",
            "longitude": 39.735866,
            "road_distances": {
                "Цирк": 520,
                "Улица Лысая Гора": 1070
            }
        },
        {
            "name": "Цирк",
            "latitude": 43.569207,
            "type": "Stop",
            "longitude": 39.739869,
            "road_distances": {
                "Стадион": 860
            }
        },
        {
            "name": "Стадион",
            "latitude": 43.565301,
            "type": "Stop",
            "longitude": 39.749485,
            "road_distances": {
                "Санаторий Металлург": 950
            }
        },
        {
            "name": "Санаторий Металлург",
            "latitude": 43.561005,
            "type": "Stop",
            "longitude": 39.760511,
            "road_distances": {
                "Улица Бытха": 900
            }
        },
        {
            "name": "Улица Бытха",
            "latitude": 43.566135,
            "type": "Stop",
            "longitude": 39.762109,
            "road_distances": {
                "Санаторий им. Ворошилова": 1160
            }
        },
        {
            "name": "Улица Лизы Чайкиной",
            "latitude": 43.590317,
            "type": "Stop",
            "longitude": 39.746833,
            "road_distances": {
                "Пионерская улица, 111": 950
            }
        },
        {
            "name": "Пионерская улица, 111",
            "latitude": 43.587257,
            "type": "Stop",
            "longitude": 39.740325,
            "road_distances": {
                "Садовая": 520
            }
        },
        {
            "name": "Садовая",
            "latitude": 43.58395,
            "type": "Stop",
            "longitude": 39.736938,
            "road_distances": {
                "Театральная": 1300
            }
        },
        {
            "name": "Театральная",
            "latitude": 43.57471,
            "type": "Stop",
            "longitude": 39.731954,
            "road_distances": {
                "Пансионат Светлана": 390
            }
        }
    ],
    "routing_settings": {
        "bus_wait_time": 2,
        "bus_velocity": 30
    }
})");
		ostringstream out;
		TravelManager(in, cout);
	}
}