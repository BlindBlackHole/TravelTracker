#include "tests.h"
#include <QCoreApplication>
#include <QHttpServer>

constexpr size_t PORT = 8080;

auto sendLocalFile(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        qDebug() << "File not found";
        return QHttpServerResponse{QHttpServerResponse::StatusCode::NotFound};
    }

    string str;
    if (file) {
       ostringstream ss;
       ss << file.rdbuf();
       str = ss.str();
    }

    qDebug() << str;

    return QHttpServerResponse{QString::fromStdString(str)};
}

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);
    //Tests::TestSvg1();

    RouteManager manager;

    QHttpServer server;

    server.afterRequest([] (QHttpServerResponse &&response) {
        response.setHeader("Access-Control-Allow-Origin", "*");
        response.setHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        response.setHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
        return std::move(response);
    });

    server.route("/upload", QHttpServerRequest::Method::Post, [&manager](const QHttpServerRequest& request) {
         std::istringstream in(request.body().toStdString());
         manager.uploadDatabase(in);
         return QHttpServerResponder::StatusCode::Created;
     });

    server.route("/map", [&manager](const QHttpServerRequest&) {
        auto result = QString::fromStdString(manager.getMap());
        auto response = QHttpServerResponse{result};
        response.setHeader("Content-Type", "application/json");
        return response;
    });

    server.route("/route", [&manager](const QHttpServerRequest &request) {
        if (!request.query().hasQueryItem("from") || !request.query().hasQueryItem("to")) {
            return QHttpServerResponse{QHttpServerResponder::StatusCode::BadRequest};
        }

        const auto from = request.query().queryItemValue("from").toStdString();
        const auto to = request.query().queryItemValue("to").toStdString();

        auto result = QString::fromStdString(manager.getRoute(from, to));

        auto response = QHttpServerResponse{result};
        response.setHeader("Content-Type", "application/json");
        return response;
    });

    server.route("/route/svg", []() {
        return sendLocalFile("route0.svg");
    });

    server.route("/map/svg", []() {
        return sendLocalFile("map0.svg");
    });

    server.listen(QHostAddress::LocalHost, PORT);
    qDebug() << "Server is listening on port: " << PORT;


    return a.exec();
}
