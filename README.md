# TravelTracker
Travel tracker application

Этот проект похож на Google maps и позволяет прокладывать маршрут от точки А в точку Б с использованием автобусов. 
Для этого в базу данных задаются остновки автобусов, сами автобусы и маршруты. Благодоря тому, что у каждой остановки есть координаты, 
программа просчитывает марштур и расстояние до определенной остновки.
Программа оформлена как сервер, где есть запросы на заполнение БД и обработки данных.
В проекте использовалось 3 библиотеки:
1)graph.h - для создания графа(карты марштуров)
2)router.h - для создания марштура от точки А до точки Б
3)json.h - для парсинга JSON файлов на ввод/вывод, которую мне пришлось доработать.

Примеры маршрутов:
https://d3c33hcgiwev3.cloudfront.net/yfxZgAXSEem6Gg6vVM6M8A_ca855ec005d211e996a4d38897f21547_CourseraBrownFinalE1Map-7.pdf?Expires=1599523200&Signature=CNeYGE0VzkpJkjLdbJt9o2BM9DnOx7Z8kiDak9z5-HoK6pSCSO0pUAZ8AOR6M9N8Kc2eyb8p95EZpNfnBwVutPEF6V-qRUiqZ8zqtPdeoijuP7iQZOkztA9R6pnfF5Ba4nSkKpzITkJaT2OeXy2PsV7qOY9hcPjd62z87UIDvMs_&Key-Pair-Id=APKAJLTNE6QMUY6HBC5A

https://d3c33hcgiwev3.cloudfront.net/OzScjgaYEem6Gg6vVM6M8A_3b809930069811e9a705156c70917e6c_CourseraBrownFinalE2Map-10.pdf?Expires=1599523200&Signature=i4QfmL4l4zS9SKgdL1HSvpdBmDTd4Uif8J8roCSuPQ-0pZOAlOsKcpUr7GeMdAr1YGVUWjfXjRhS51spZ~AuBJoQ1~NAps~fceg78g1yX3zD9JodKU5a9pxfB9KRyrF7Ji2UgdPQfV7pHFfsInSJmVPuFCMDdmxCmelo0Vdvapg_&Key-Pair-Id=APKAJLTNE6QMUY6HBC5A

https://d3c33hcgiwev3.cloudfront.net/1Nbt5yhKRiSW7ecoSpYkKQ_4a80fe34315146c282000224180412ae_CourseraBrownFinalE3Map-1.pdf?Expires=1599523200&Signature=jGsURnw7FpaPvA0aOtlu~Yu1II6ZMF~6rhgCDCvKaxTaxUtt5bFmQcOHjo8gJ6CIUcclVDj8zO91ZT31VkSszy9mu2tFJ-w0flQ28ZHkTcxT-HbrVoVOl7BjQ9EbT9TL8B1Gh1zFAsHyyOA7f43-Lv8pihDE48fv8lom6O66VZI_&Key-Pair-Id=APKAJLTNE6QMUY6HBC5A
