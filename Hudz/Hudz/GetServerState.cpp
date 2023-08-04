#include "GetServerState.h"

JsonFetcher::JsonFetcher(QObject* parent): QObject(parent)
{
	Manager = new QNetworkAccessManager(this);
	connect(Manager, &QNetworkAccessManager::finished, this, &JsonFetcher::OnFinished);
}

void JsonFetcher::FetchJson() const
{
	QNetworkRequest request(QUrl(SEVER_URL));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	Manager->get(request);
}

QList<ServerInfo>& JsonFetcher::GetServers()
{
	return Servers;
}

void JsonFetcher::OnFinished(QNetworkReply* reply)
{
	if (reply->error() == QNetworkReply::NoError) 
	{
		const QByteArray response = reply->readAll();
		const QJsonDocument doc = QJsonDocument::fromJson(response);
		QJsonArray jsonArray = doc.array();
		for (auto value : jsonArray) 
		{
			QJsonObject obj = value.toObject();
			ServerInfo server;
			server.ZoneName = obj["zone_name"].toString();
			server.ServerName = obj["server_name"].toString();
			server.IpAddress = obj["ip_address"].toString();
			server.IpPort = obj["ip_port"].toString();
			server.MainServer = obj["main_server"].toString();
			server.ConnectState = obj["connect_state"].toBool();
			server.MaintainTime = obj["maintain_time"].toInt();
			server.Heat = obj["heat"].toString();
			Servers.append(server);
		}
		emit JsonFetched();
	}
	else 
	{
		// Handle error
		qDebug() << "Error fetching JSON:" << reply->errorString();
	}
	reply->deleteLater();
}
