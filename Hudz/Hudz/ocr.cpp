#include "ocr.h"

Ocr::Ocr(const QString& trainDataPath, const QString& lang):
	Api(new ocr)
{
	// Initialize tesseract-ocr with English, without specifying tessdata path
	if (Api->Init(trainDataPath.toStdString().c_str(), lang.toStdString().c_str(), tesseract::OEM_DEFAULT))
	{
		qDebug() << stderr;
	}
	else
	{
		Inited = true;
	}
}

Ocr::~Ocr()
{
	if(Inited)
	{
		Api->End();
	}
}

QPixmap Ocr::CvMatToQPixmap(const cv::Mat& mat)
{
    cv::Mat temp;
    cvtColor(mat, temp, cv::COLOR_BGR2RGB); // cvtColor Makes a copy, that what i need
    QImage dest(static_cast<const uchar*>(temp.data), temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    dest.bits(); // enforce deep copy, see documentation 
    // of QImage::QImage ( const uchar *, int, int, Format )
    // "The buffer must remain valid throughout the life of the QImage."
    // The QImage constructed with the raw cv::Mat data
    // has to be copied, so that the data buffer stays valid after return.
    return QPixmap::fromImage(dest);
}

cv::Mat Ocr::QPixmapToCvMat(const QPixmap& pixmap)
{
	QImage tempImage = pixmap.toImage();
	cv::Mat mat;

	switch (tempImage.format()) {
	case QImage::Format_ARGB32:
	case QImage::Format_RGB32:
	case QImage::Format_ARGB32_Premultiplied:
		mat = cv::Mat(tempImage.height(), tempImage.width(), CV_8UC4,
		              const_cast<uchar*>(tempImage.bits()), static_cast<size_t>(tempImage.bytesPerLine()));
		cv::cvtColor(mat, mat, cv::COLOR_BGRA2BGR);
		break;
	case QImage::Format_RGB888:
		mat = cv::Mat(tempImage.height(), tempImage.width(), CV_8UC3,
		              const_cast<uchar*>(tempImage.bits()), static_cast<size_t>(tempImage.bytesPerLine()));
		cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
		break;
	case QImage::Format_Grayscale8:
		mat = cv::Mat(tempImage.height(), tempImage.width(), CV_8UC1,
		              const_cast<uchar*>(tempImage.bits()), static_cast<size_t>(tempImage.bytesPerLine()));
		break;
	default:
		break;
	}

	// Convert to gray scale
	cv::cvtColor(mat, mat, cv::COLOR_BGR2GRAY);

	return mat;
}

void Ocr::CreateShowWidget(const cv::Mat& mat)
{
	CreateShowWidget(CvMatToQPixmap(mat));
}

void Ocr::CreateShowWidget(const QPixmap& pixmap)
{
	const auto sca = new QScrollArea;
	sca->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	sca->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	const auto img = new QLabel();
	img->setPixmap(pixmap);

	const auto widget = new QWidget();
	const auto layout = new QHBoxLayout();
	layout->addWidget(img);
	widget->setLayout(layout);

	sca->setWidget(widget);
	sca->show();
}

QPixmap Ocr::CaptureScreen()
{
	QScreen* screen = QGuiApplication::primaryScreen();
	if (const QWindow* window = QGuiApplication::focusWindow())
		screen = window->screen();
	return screen->grabWindow(0);
}

cv::Rect Ocr::FindMatchedRect(const cv::Mat& screen, const cv::Mat& templateImg) const
{
	cv::Mat result;
	cv::matchTemplate(screen, templateImg, result, cv::TM_CCOEFF_NORMED);
	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

	const auto timeRect = cv::Rect(maxLoc, templateImg.size());

	return timeRect;
}

QString Ocr::GetTxt(const cv::Mat& img) const
{
	Api->SetImage((uchar*)img.data, img.size().width, img.size().height, 
		img.channels(), static_cast<int>(img.step1()));
	Api->Recognize(nullptr);
	const char* text = Api->GetUTF8Text();
	return text;
}

QString Ocr::FindMatText(const cv::Mat& templateMat, const QPixmap&& src)
{
	if(!Inited)
	{
		return "";
	}
	CurrentSrcMat = QPixmapToCvMat(src);
	CurrentMatchedRect = FindMatchedRect(CurrentSrcMat, templateMat);
	CurrentMatchedMat = CurrentSrcMat(CurrentMatchedRect);
	return GetTxt(CurrentMatchedMat);
}
