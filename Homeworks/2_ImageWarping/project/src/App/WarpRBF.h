#pragma once

#include "Warp.h"

#include <QVector2D>
#include <QWidget>
#include <cmath>
#include <Eigen/Dense>

using namespace Eigen;

class WarpRBF : public Warp
{
public:
	WarpRBF();
	~WarpRBF() = default;
	void Render(QImage* ptr_image);

protected:
	QPoint Output(QPoint p);

private:
	float RBF(QPoint p, int i);
	void CalculateR(); 	         // r_
	void CalculateCoff();		 // alpha, A, b

	float u_;
	QVector<float> r_;
	QVector<float> alphax_;
	QVector<float> alphay_;

	float a11_, a12_, a21_, a22_;
	float b11_, b21_;
};
