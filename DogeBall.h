#pragma once
#include <QtOpenGL\QGLWidget>
class GameObject;
class Camera;
class TwoDZoomCamera;
class TwoDPlaneInput;
class QTimer;
class Life;
class DogeBall : public QGLWidget
{
	Q_OBJECT;

	GameObject* player, *player2;
	Life* life1 , *life2;
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