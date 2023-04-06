#include "tests.h"
#include <QCoreApplication>
#include <QHttpServer>

constexpr size_t PORT = 8080;

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);
    //Tests::TestSvg1();

    //TravelManager();

    RouteDatabase data;
    TravelTracker travel(cin, cout);
    travel.ReadDataFromJson(data).QueryProcessing(data);

    QHttpServer server;

    server.route("/", [](const QHttpServerRequest &request) {
        return "Hello World";
    });

    server.route("/upload", [](const QHttpServerRequest &request) {
        std::istringstream in(request.body().toStdString());
        return "Hello World";
    });

    server.route("/stops", [](const QHttpServerRequest &request) {
        return "Hello World";
    });

    server.route("/buses", [](const QHttpServerRequest &request) {
        return "Hello World";
    });

    server.route("/map", [](const QHttpServerRequest &request) {
        return "Hello World";
    });

    server.route("/route", [](const QHttpServerRequest &request) {
        return "Hello World";
    });



    server.listen(QHostAddress::LocalHost, PORT);
    qDebug() << "Server is listening on port: " << PORT;


    return a.exec();
}
