#pragma once
#include <QScreen>
#include <QGuiApplication>
#include <QLabel>
#include <QScrollArea>
#include <QPixmap>
#include <QHBoxLayout>
#include <QWindow>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>

class Ocr
{
    typedef tesseract::TessBaseAPI ocr;

public:
    explicit Ocr(const QString& trainDataPath, const QString& lang);
    ~Ocr();

    static QPixmap CvMatToQPixmap(const cv::Mat& mat);
    static cv::Mat QPixmapToCvMat(const QPixmap& pixmap);
    static void CreateShowWidget(const cv::Mat& mat);
    static void CreateShowWidget(const QPixmap& pixmap);
    static QPixmap CaptureScreen();

    [[nodiscard]] cv::Mat& GetCurrentMatchedMat() { return CurrentMatchedMat; }

    QString FindMatText(const cv::Mat& templateMat, const QPixmap&& src);

private:
    [[nodiscard]] cv::Rect FindMatchedRect(const cv::Mat& screen, const cv::Mat& templateImg) const;
    [[nodiscard]] QString GetTxt(const cv::Mat& img) const;

    bool Inited = false;
    ocr* Api = nullptr;

    cv::Mat CurrentSrcMat;
    cv::Mat CurrentMatchedMat;
    cv::Rect CurrentMatchedRect;
};


