#include <QTextCodec>
#include <QRegExp>
#include <Windows.h>
#include "hudz.h"

Hudz::Hudz(QWidget* parent)
	: QMainWindow(parent),
	BaseTimer(new QTimer(this)),
	PingTimer(new QTimer(this)),
	ShotTimer(new QTimer(this))
{
    Ui.setupUi(this);

    JsonFetcher.FetchJson();

    connect(&JsonFetcher, &JsonFetcher::JsonFetched, this, &Hudz::ShowJson);

	PingTimer->setInterval(1000);
	connect(PingTimer, &QTimer::timeout, this, &Hudz::TestPing);

	BaseTimer->setInterval(16);
	connect(BaseTimer, &QTimer::timeout, [this]() {Ui.DTEdit_Current->setDateTime(QDateTime::currentDateTime()); });
	BaseTimer->start();

	ShotTimer->setSingleShot(true);
	connect(ShotTimer, &QTimer::timeout, this, &Hudz::SimMouseClicked);

	Preset();
}

Hudz::~Hudz()
= default;

void Hudz::ShowJson()
{
	for(auto severList = JsonFetcher.GetServers(); 
		auto& server : severList)
    {
		Ui.CBox_Sever_list->insertItem(0, server.ServerName);
		connect(Ui.CBox_Sever_list, &QComboBox::currentTextChanged, this, &Hudz::ChangeCurrentServer);
    }
	PingTimer->start();
}

void Hudz::TestPing()
{
	auto process = new QProcess(this);

	connect(process, &QProcess::finished,
		[this, process](const int exitCode, const QProcess::ExitStatus exitStatus)
		{
			if (!Ui.LEdit_Ping)
				return;
			if (exitStatus == QProcess::NormalExit && exitCode == 0)
			{
				const QString output = process->readAllStandardOutput();
				if (QRegExp regex(QString("Æ½¾ù = (\\d+)ms|Average = (\\d+)ms"));
					regex.indexIn(output) != -1)
				{
					const QString averageLatency = regex.cap(1).isEmpty() ? regex.cap(2) : regex.cap(1);
					Ui.LEdit_Ping->setText(averageLatency + " ms " + TipsList.at(TipCount++%4));
				}
				else
				{
					Ui.LEdit_Ping->setText(QString("Failed to parse ping result."));
				}
			}
			else
			{
				Ui.LEdit_Ping->setText(QString("Ping failed."));
			}

			process->deleteLater();
		});


	process->start("ping", QStringList() << Ui.LEdit_IpAddress->text() << "-n" << "1");

	if (!process->waitForFinished(3000))
	{
		process->kill();
	}
}

void Hudz::PButtonStartClicked() const
{
	const auto delta = Ui.DTEdit_Current->dateTime().msecsTo(Ui.DTEdit_Target->dateTime());
	if(delta < 0)
	{
		Ui.plainTextEdit->appendPlainText("Invalid target time.");
		return;
	}


	ShotTimer->start(static_cast<int>(delta) + Ui.SBox_Delay->value());
}

void Hudz::PButtonPauseClicked() const
{
	if(ShotTimer->isActive())
	{
		ShotTimer->stop();
	}
}

void Hudz::Preset() const
{
	switch (const QDate date = QDate::currentDate();
		date.dayOfWeek())
	{
	case 1:
		break;
	case 2:
		Ui.DTEdit_Target->setDateTime(QDateTime(QDate::currentDate(), QTime(19, 30, 0, 0)));
		break;
	case 3:
		break;
	case 4:
		Ui.DTEdit_Target->setDateTime(QDateTime(QDate::currentDate(), QTime(19, 30, 0, 0)));
		break;
	case 5:
		break;
	case 6:
	case 7:
		if(QTime::currentTime() < QTime(13,00,0,0))
		{
			Ui.DTEdit_Target->setDateTime(QDateTime(QDate::currentDate(), QTime(12, 30, 0, 0)));
		}
		else
		{
			Ui.DTEdit_Target->setDateTime(QDateTime(QDate::currentDate(), QTime(18, 30, 0, 0)));
		}
		break;
	default:
		break;
	}

}

void Hudz::ChangeCurrentServer(const QString& newServerName)
{
	const auto newServer = std::ranges::find_if(JsonFetcher.GetServers(), 
		[&newServerName](auto& server)
		{
			if (server.ServerName == newServerName)
			{
				return true;
			}
			return false;
	});

	Ui.LEdit_ZoneName->setText(newServer->ZoneName);
	Ui.LEdit_IpAddress->setText(newServer->IpAddress);
	Ui.LEdit_IpPort->setText(newServer->IpPort);
	Ui.LEdit_MainSever->setText(newServer->MainServer);
	Ui.LEdit_ConnectState->setText(newServer->ConnectState ? "true" : "false");
	Ui.LEdit_MaintainTime->setText(QString::number(newServer->MaintainTime) + "-->" + QDateTime::fromSecsSinceEpoch(newServer->MaintainTime).toString());
	Ui.LEdit_Heat->setText(newServer->Heat);
}

void Hudz::SimMouseClicked() const
{
	Ui.plainTextEdit->appendPlainText("Shot!");
	INPUT inputDown = { 0 };
	inputDown.type = INPUT_MOUSE;
	inputDown.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

	INPUT inputUp = { 0 };
	inputUp.type = INPUT_MOUSE;
	inputUp.mi.dwFlags = MOUSEEVENTF_LEFTUP;

	for (int time = 0; time < Ui.SBox_RepeatTimes->value(); ++time)
	{
		SendInput(1, &inputDown, sizeof(INPUT));
		Sleep(15);
		SendInput(1, &inputUp, sizeof(INPUT));
		Ui.plainTextEdit->appendPlainText("Triggered! " + QString::number(time));
	}
}
