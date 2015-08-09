#include <DogeBall.h>
#include <Qt\qtimer.h>
#include <Core\WindowInfo.h>
#include <GlobalItems.h>
#include <DebugMemory.h>
void DogeBall::initializeGL()
{
	GlobalItems::global.changeState(GameStates::Start);
	timer = new QTimer();
	connect( timer , SIGNAL( timeout() ) , this , SLOT( update() ) );
	timer->start( 0 );
}
void DogeBall::paintGL()
{
	try
	{
		GlobalItems::global.draw();
	}
	catch (...)
	{
		int i = 0;
		++i;
	}
}

void DogeBall::update()
{
	WindowInfo::width = width();
	WindowInfo::height = height();
	GlobalItems::global.update();
	repaint();
}

DogeBall::~DogeBall()
{
	delete timer;
}