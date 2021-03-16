//------------------------------------------------------------------------------
// main.cc
// (C) 2015-2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"


//------------------------------------------------------------------------------
/*
#include "exampleapp.h"

int main(int argc, const char** argv)
{
	Example::ExampleApp app;
	if (app.Open())
	{
		app.Run();
		app.Close();
	}
	app.Exit();
	
}
*/

#include "Rasterizer.h"

int main()
{
	Rasterizer TheRasterizer;
	TheRasterizer.OnUserCreate();
	
	while(true)
	{
		TheRasterizer.OnUserUpdate();
	}
}
