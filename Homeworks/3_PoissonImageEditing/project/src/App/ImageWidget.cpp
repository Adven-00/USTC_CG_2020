#include "ImageWidget.h"

#include <QImage>
#include <iostream>
#include <QPainter>

using std::cout;
using std::endl;

ImageWidget::ImageWidget(ChildWindow* relatewindow)
{
	image_ = new QImage();
	image_backup_ = new QImage();

	draw_status_ = DrawStatus::kNone;

	point_start_ = QPoint(0, 0);
	point_end_ = QPoint(0, 0);

	source_window_ = nullptr;
	image_source_ = nullptr;
}

ImageWidget::~ImageWidget(void)
{
}

int ImageWidget::ImageWidth()
{
	return image_->width();
}

int ImageWidget::ImageHeight()
{
	return image_->height();
}

void ImageWidget::set_draw_status_to_choose()
{
	draw_status_ = DrawStatus::kChoose;
}

void ImageWidget::set_draw_status_to_paste()
{
	draw_status_ = DrawStatus::kPaste;
}

QImage* ImageWidget::image()
{
	return image_;
}

void ImageWidget::set_source_window(ChildWindow* childwindow)
{
	source_window_ = childwindow;
}

void ImageWidget::paintEvent(QPaintEvent* paintevent)
{
	QPainter painter;
	painter.begin(this);

	// Draw background
	painter.setBrush(Qt::lightGray);
	QRect back_rect(0, 0, width(), height());
	painter.drawRect(back_rect);

	// Draw image
	QRect rect = QRect(0, 0, image_->width(), image_->height());
	painter.drawImage(rect, *image_);

	// Draw choose region
	painter.setBrush(Qt::NoBrush);
	painter.setPen(Qt::red);
	painter.drawRect(point_start_.x(), point_start_.y(),
		point_end_.x() - point_start_.x(), point_end_.y() - point_start_.y());

	painter.end();
}

void ImageWidget::mousePressEvent(QMouseEvent* mouseevent)
{
	if (Qt::LeftButton == mouseevent->button())
	{
		switch (draw_status_)
		{
		case DrawStatus::kChoose:
			point_start_ = point_end_ = mouseevent->pos();
			break;

		case DrawStatus::kPaste:
		{
			// Start point in object image
			int xpos = mouseevent->pos().rx();
			int ypos = mouseevent->pos().ry();

			// Start point in source image
			int xsourcepos = source_window_->imagewidget_->point_start_.rx();
			int ysourcepos = source_window_->imagewidget_->point_start_.ry();

			// Width and Height of rectangle region
			int w = source_window_->imagewidget_->point_end_.rx()
				- source_window_->imagewidget_->point_start_.rx() + 1;
			int h = source_window_->imagewidget_->point_end_.ry()
				- source_window_->imagewidget_->point_start_.ry() + 1;

			// Paste
			if ((xpos + w < image_->width()) && (ypos + h < image_->height()))
			{
				// Restore image
			//	*(image_) = *(image_backup_);

				// Paste
				for (int i = 0; i < w; i++)
				{
					for (int j = 0; j < h; j++)
					{
						image_->setPixel(xpos + i, ypos + j, 
							source_window_->imagewidget_->image()->pixel(xsourcepos + i, ysourcepos + j));
					}
				}
			}
		}

		update();
		break;
		}
	}
}

void ImageWidget::mouseMoveEvent(QMouseEvent* mouseevent)
{
	switch (draw_status_)
	{
	case DrawStatus::kChoose:

		// Store point position for rectangle region
		point_end_ = mouseevent->pos();
		break;

	case DrawStatus::kPaste:
		// Paste rectangle region to object image
			// Start point in object image
		int xpos = mouseevent->pos().rx();
		int ypos = mouseevent->pos().ry();

		// Start point in source image
		int xsourcepos = source_window_->imagewidget_->point_start_.rx();
		int ysourcepos = source_window_->imagewidget_->point_start_.ry();

		// Width and Height of rectangle region
		int w = source_window_->imagewidget_->point_end_.rx()
			- source_window_->imagewidget_->point_start_.rx() + 1;
		int h = source_window_->imagewidget_->point_end_.ry()
			- source_window_->imagewidget_->point_start_.ry() + 1;

		// Paste
		if ((xpos > 0) && (ypos > 0) && (xpos + w < image_->width()) && (ypos + h < image_->height()))
		{
			// Restore image 
			*(image_) = *(image_backup_);

			// Paste
			for (int i = 0; i < w; i++)
			{
				for (int j = 0; j < h; j++)
				{
					image_->setPixel(xpos + i, ypos + j, 
						source_window_->imagewidget_->image()->pixel(xsourcepos + i, ysourcepos + j));
				}
			}
		}
	}

	update();
}

void ImageWidget::mouseReleaseEvent(QMouseEvent* mouseevent)
{
	switch (draw_status_)
	{
	case DrawStatus::kChoose:
	{
		point_end_ = mouseevent->pos();
		draw_status_ = DrawStatus::kNone;

		int h = point_start_.y() - point_end_.y();
		int w = point_start_.x() - point_end_.x();
		image_source_ = new QImage(w, h, QImage::Format_RGB888);

		for (int i = 0; i < w; i++)
			for (int j = 0; j < h; j++)
				image_source_->setPixel(i, j, image_->pixel(point_start_ + QPoint(i, j));
	}

	case DrawStatus::kPaste:
	{
		draw_status_ = DrawStatus::kNone;
	}
	}

	update();
}

void ImageWidget::Open(QString filename)
{
	// Load file
	if (!filename.isEmpty())
	{
		image_->load(filename);
		*(image_backup_) = *(image_);
	}

	//	setFixedSize(image_->width(), image_->height());
	//	relate_window_->setWindowFlags(Qt::Dialog);
	//	relate_window_->setFixedSize(QSize(image_->width(), image_->height()));
	//	relate_window_->setWindowFlags(Qt::SubWindow);

		//image_->invertPixels(QImage::InvertRgb);
		//*(image_) = image_->mirrored(true, true);
		//*(image_) = image_->rgbSwapped();
	cout << "image size: " << image_->width() << ' ' << image_->height() << endl;
	update();
}

void ImageWidget::Save()
{
	SaveAs();
}

void ImageWidget::SaveAs()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
	if (filename.isNull())
	{
		return;
	}

	image_->save(filename);
}

void ImageWidget::Invert()
{
	for (int i = 0; i < image_->width(); i++)
	{
		for (int j = 0; j < image_->height(); j++)
		{
			QRgb color = image_->pixel(i, j);
			image_->setPixel(i, j, qRgb(255 - qRed(color), 255 - qGreen(color), 255 - qBlue(color)));
		}
	}

	// equivalent member function of class QImage
	// image_->invertPixels(QImage::InvertRgb);
	update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	QImage image_tmp(*(image_));
	int width = image_->width();
	int height = image_->height();

	if (ishorizontal)
	{
		if (isvertical)
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_->setPixel(i, j, image_tmp.pixel(width - 1 - i, height - 1 - j));
				}
			}
		}
		else
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_->setPixel(i, j, image_tmp.pixel(i, height - 1 - j));
				}
			}
		}

	}
	else
	{
		if (isvertical)
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_->setPixel(i, j, image_tmp.pixel(width - 1 - i, j));
				}
			}
		}
	}

	// equivalent member function of class QImage
	//*(image_) = image_->mirrored(true, true);
	update();
}

void ImageWidget::TurnGray()
{
	for (int i = 0; i < image_->width(); i++)
	{
		for (int j = 0; j < image_->height(); j++)
		{
			QRgb color = image_->pixel(i, j);
			int gray_value = (qRed(color) + qGreen(color) + qBlue(color)) / 3;
			image_->setPixel(i, j, qRgb(gray_value, gray_value, gray_value));
		}
	}

	update();
}

void ImageWidget::Restore()
{
	*(image_) = *(image_backup_);
	point_start_ = point_end_ = QPoint(0, 0);
	update();
}

QImage ImageWidget::Mat2QImage(cv::Mat& mat)
{
	cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
	QImage qim((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
	return qim;
}

cv::Mat ImageWidget::QImage2Mat(QImage& qim, int height, int width)
{
	cv::Mat mat;
	if (height == -1 && width == -1)
		mat = cv::Mat(qim.height(), qim.width(), CV_8UC3, (void*)qim.constBits(), qim.bytesPerLine());
	else
		mat = cv::Mat(height, width, CV_8UC3, (void*)qim.constBits(), qim.bytesPerLine());

	return mat;
}
