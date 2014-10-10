#pragma once
#include <QtOpenGL\QGLWidget>
class GameObject;
class Camera;
class TwoDZoomCamera;
class TwoDPlaneInput;
class QTimer;
class DogeBall : public QGLWidget
{
	Q_OBJECT;

	GameObject* player, *player2;
	Camera* camera;
	TwoDZoomCamera* zoomer;
	QTimer* timer;
	TwoDPlaneInput* planeInput, *planeInput2;
	int theTex;
protected:
	
	void initializeGL();
	void paintGL();
	protected slots:
	void update();
public:
	~DogeBall();
};