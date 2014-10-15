#pragma once
#include <Core\CommonWarningDisables.h>
#include <QtOpenGL\QGLWidget>
#include <glm.hpp>

class QTimer;

class DogeBall : public QGLWidget
{
	Q_OBJECT;


	QTimer* timer;

protected:
	
	void initializeGL();
	void paintGL();
	protected slots:
	void update();
public:
	~DogeBall();
};