#include "tests.h"
#include <QCoreApplication>
#include <QHttpServer>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QFileInfo>

#include <QJsonDocument>
#include <QJsonObject>

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

    return QHttpServerResponse{QString::fromStdString(str)};
}


int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);
    //Tests::TestSvg1();

    RouteManager manager;

    manager.loadSavedMaps();

    static constexpr auto mapFile = "./data/maps/Kyiv.json";

    manager.initMap(mapFile);

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

    server.route("/map/upload", QHttpServerRequest::Method::Post, [&manager](const QHttpServerRequest& request) {
        auto body = QString::fromUtf8(request.body()).simplified().toStdString();

        size_t nameStart = body.find("filename=") + 10;
        size_t nameEnd = body.find_first_of('"', nameStart);
        auto mapFile = body.substr(nameStart, nameEnd - nameStart);
        qDebug() << "Parsed filename: " << mapFile;

        size_t start = body.find_first_of(L'{');
        size_t end = body.find_last_of(L'}');
        auto result = body.substr(start, end - start + 1);


        auto [mapId, mapName] = manager.addMap(std::move(mapFile), std::move(result));

        QJsonObject obj{
            {"mapId", mapId.data()},
            {"name", mapName.data()}
        };

        return QHttpServerResponse{obj};
    });

//    server.route("/map", [&manager](const QHttpServerRequest&) {
//        auto result = QString::fromStdString(manager.addMap());
//        auto response = QHttpServerResponse{result};
//        response.setHeader("Content-Type", "application/json");
//        return response;
//    });

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

    server.route("/map/svg", [&manager](const QHttpServerRequest &request) {
        if (!request.query().hasQueryItem("id")) {
            qDebug() << "No map id param";
            return QHttpServerResponse{QHttpServerResponder::StatusCode::BadRequest};
        }

        auto mapName = manager.getMapName(request.query().queryItemValue("id").toStdString());

        if (mapName.empty()) {
            return QHttpServerResponse{QHttpServerResponder::StatusCode::NotFound};
        }

        manager.initMap("./data/maps/" + mapName + ".json");

        return sendLocalFile(mapName + ".svg");
    });

    server.route("/stops", [&manager](const QHttpServerRequest&) {
        auto result = QString::fromStdString(manager.getStops());

        auto response = QHttpServerResponse{result};
        response.setHeader("Content-Type", "application/json");
        return response;
    });

    server.route("/buses", [&manager](const QHttpServerRequest&) {
        auto result = QString::fromStdString(manager.getBuses());

        auto response = QHttpServerResponse{result};
        response.setHeader("Content-Type", "application/json");
        return response;
    });

    server.route("/maps", [&manager](const QHttpServerRequest&) {
        auto result = QString::fromStdString(manager.getMaps());

        auto response = QHttpServerResponse{result};
        response.setHeader("Content-Type", "application/json");
        return response;
    });


    server.listen(QHostAddress::LocalHost, PORT);
    qDebug() << "Server is listening on port: " << PORT;


    return a.exec();
}
