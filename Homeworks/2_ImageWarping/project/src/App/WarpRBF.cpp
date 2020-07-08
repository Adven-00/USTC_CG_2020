#include "WarpRBF.h"

WarpRBF::WarpRBF() : u_(1.0f)
{
}

QPoint WarpRBF::Output(QPoint p)
{
	float xo = b11_ + p.x() * a11_ + p.y() * a12_, 
		yo = b21_ + p.x() * a21_ + p.y() * a22_;

	for (int i = 0; i < begin_points_.size(); i++)
	{
		xo += alphax_[i] * RBF(p, i);
		yo += alphay_[i] * RBF(p, i);
	}
	return QPoint(xo, yo);
}

float WarpRBF::RBF(QPoint p, int i)
{
	QVector2D vec(p.x() - begin_points_[i].x(), p.y() - begin_points_[i].y());
	return powf(vec.lengthSquared() + r_[i] * r_[i], u_ / 2.0f);
}

void WarpRBF::CalculateR()
{
	for (int i = 0; i < begin_points_.size(); i++)
	{
		float min_r_i = 999999;
		for (int j = 0; j < begin_points_.size(); j++)
		{
			if (i != j)
			{
				QVector2D vec(begin_points_[i].x() - begin_points_[j].x(),
					begin_points_[i].y() - begin_points_[j].y());

				if (vec.length() < min_r_i)
					min_r_i = vec.length();
			}
		}
		r_.push_back(min_r_i);
	}
}

void WarpRBF::CalculateCoff()
{
	int n = begin_points_.size();

	MatrixXf qx(n + 3, 1), qy(n + 3, 1);

	for (int i = 0; i < n; i++)
	{
		qx(i, 0) = end_points_[i].x();
		qy(i, 0) = end_points_[i].y();
	}

	for (int i = n; i < n + 3; i++)
	{
		qx(i, 0) = qy(i, 0) = 0;
	}

	MatrixXf Coff(n + 3, n + 3);

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			Coff(i, j) = RBF(begin_points_[i], j);
		}
		Coff(i, n) = begin_points_[i].x();
		Coff(i, n + 1) = begin_points_[i].y();
		Coff(i, n + 2) = 1;
	}

	for (int i = 0; i < n; i++)
	{
		Coff(n, i) = begin_points_[i].x();
		Coff(n + 1, i) = begin_points_[i].y();
		Coff(n + 2, i) = 1;
	}

	for (int i = n; i < n + 3; i++)
	{
		for (int j = n; j < n + 3; j++)
		{
			Coff(i, j) = 0;
		}
	}

	MatrixXf resultx = Coff.colPivHouseholderQr().solve(qx);
	MatrixXf resulty = Coff.colPivHouseholderQr().solve(qy);

	for (int i = 0; i < n; i++)
	{
		alphax_.push_back(resultx(i, 0));
		alphay_.push_back(resulty(i, 0));
	}

	a11_ = resultx(n, 0), a12_ = resultx(n + 1, 0);
	a21_ = resulty(n, 0), a22_ = resulty(n + 1, 0);
	b11_ = resultx(n + 2, 0), b21_ = resulty(n + 2, 0);
}

void WarpRBF::Render(QImage* ptr_image)
{
	QImage image_tmp(*(ptr_image));

	int width = ptr_image->width();
	int height = ptr_image->height();

	ptr_image->fill(Qt::lightGray);

	CalculateR();
	CalculateCoff();

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			QPoint p(i, j);
			QPoint po = Output(p);

			if (po.x() >= 0 && po.x() < width && po.y() >= 0 && po.y() < height)
				ptr_image->setPixel(Output(p), image_tmp.pixel(p));

			//FixHole(ptr_image);
		}
	}
}
