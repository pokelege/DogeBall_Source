#pragma once
#include <QtOpenGL\QGLWidget>
class GameObject;
class Camera;
class DogeBall : public QGLWidget
{
	Q_OBJECT;

	GameObject* player, *player2;
	Camera* camera;
protected:
	void initializeGL();
	void paintGL();
	protected slots:
	void update();
};