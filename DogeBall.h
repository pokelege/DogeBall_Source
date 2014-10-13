#pragma once
#include <QtOpenGL\QGLWidget>
#include <glm.hpp>
class GameObject;
class Camera;
class TwoDZoomCamera;
class TwoDPlaneInput;
class QTimer;
class Life;
class Gun;
class DogeBall : public QGLWidget
{
	Q_OBJECT;

	GameObject* player, *player2, *level;
	Life* life1 , *life2;
	Gun* gun1 , *gun2;
	Camera* camera;
	TwoDZoomCamera* zoomer;
	QTimer* timer;
	TwoDPlaneInput* planeInput, *planeInput2;
	int theTex;
	glm::vec4 defaultColor;
protected:
	
	void initializeGL();
	void paintGL();
	protected slots:
	void update();
public:
	~DogeBall();
};