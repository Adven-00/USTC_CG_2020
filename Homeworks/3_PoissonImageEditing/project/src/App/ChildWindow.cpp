#include "ChildWindow.h"

ChildWindow::ChildWindow(void)
{
	imagewidget_ = new ImageWidget(this);
	setCentralWidget(imagewidget_);
}


ChildWindow::~ChildWindow(void)
{
}

bool ChildWindow::LoadFile(QString filename)
{
	imagewidget_->Open(filename);
	current_file_ = filename;
	setWindowTitle(QFileInfo(filename).baseName());

	return true;
}