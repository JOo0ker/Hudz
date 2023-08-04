#pragma once

#include <QtWidgets/QMainWindow>
#include <QTimer>
#include "ui_hudz.h"

#include "GetServerState.h"

class Hudz final : public QMainWindow
{
    Q_OBJECT

public:
    explicit Hudz(QWidget *parent = nullptr);
    ~Hudz() override;

public slots:
    void ShowJson();
    void TestPing();
    void PButtonStartClicked() const;
    void PButtonPauseClicked() const;

private:
    void Preset() const;
    void ChangeCurrentServer(const QString& newServerName);
    void SimMouseClicked() const;

    Ui::HudzClass Ui{};
    QTimer* BaseTimer;
    QTimer* PingTimer;
    QTimer* ShotTimer;
    JsonFetcher JsonFetcher;
    int TipCount = 0;
	QVector<QString> TipsList{ "-", "--", "---", "----" };
};
