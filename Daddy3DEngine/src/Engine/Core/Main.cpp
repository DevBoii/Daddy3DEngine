#include <iostream>
#include "Application.h"

int main()
{
	DaddyEngine::Application* app = new DaddyEngine::Application();
	app->run();

	delete app; //free the resources

	return 0;
}