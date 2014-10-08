#include <Qt\qapplication.h>
#include "DogeBall.h"
#include <Graphics\CommonGraphicsCommands.h>
int main( int argc , char** argv )
{
#ifdef _DEBUG
	//_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
#endif
	QApplication app( argc , argv );
	DogeBall game;
	game.showFullScreen();
	return app.exec();
}