#pragma once
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QUrl>

#define SEVER_URL "https://spider2.jx3box.com/api/spider/server/server_state"

class ServerInfo
{
public:
    QString ZoneName;
    QString ServerName;
    QString IpAddress;
    QString IpPort;
    QString MainServer;
    bool ConnectState;
    qint64 MaintainTime;
    QString Heat;
};

class JsonFetcher final : public QObject
{
    Q_OBJECT

public:
    explicit JsonFetcher(QObject* parent = nullptr);

    void FetchJson() const;

    [[nodiscard]] QList<ServerInfo>& GetServers();

private slots:
    void OnFinished(QNetworkReply* reply);

signals:
    void JsonFetched();

private:
    QNetworkAccessManager* Manager;
    QList<ServerInfo> Servers;
};

