#include "Other Graphics Stuff/cGraphicsMain.h"


int main(int argc, char** argv)
{
	cGraphicsMain* graphics = cGraphicsMain::getGraphicsMain();// This initializes it if not yet made 


	while (graphics->Update() == 0)
	{
		// Running...
	}
	graphics->Destroy();

	return 0;
}