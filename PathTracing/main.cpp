#include <iostream>
#include "PathTracer.h"


int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	// [Important]
	// You can run one of the three parts below ( not at the same time )
	// Make sure you use x64 if you use windows and the freeglut I provide.
	// Make sure you use Release if you want to see the result.
#define RUN_NUMBER 2

//--------------------------------- the first scene -------------------------------------------------------------------------------
#if RUN_NUMBER == 1
	PathTracer pt;

	//if (pt.loadObject("../scene_1_obj/scene01.obj")) {								//normal mesh file
	if (pt.loadObject("../../scene_1_obj/scene01_perfect_sphere.obj")) {				//the sphere is replaced by geometry parameters to get smooth sphere

		pt.setCamera(Vector3f(0, 4.5, 14.5), Vector3f(0, 0, -1), Vector3f(0, 1, 0));	//’‚∏ˆŒª÷√ «’˝≥£Œª÷√  normal camera positon																		
		//pt.setCamera(Vector3f(-2.3,1.6,4), Vector3f(0, 0, -1), Vector3f(0, 1, 0));	//’‚∏ˆŒª÷√”√¿¥¥’Ω¸π€≤Ïæµ√Ê«Ú alternative camera position to watch the mirror sphere closely

		pt.setPixelSize(400, 300);
		pt.sample_num = 100;															//sample number.  bigger number can get better quality but costs more time.
		pt.env_color = Vector3f(1, 1, 1);												//the color of the environment
		pt.env_intensity = 0;															//the intensity of the color of the environment
		pt.light_color = Vector3f(1, 1, 1);												//the color of the light
		pt.light_intensity = 50;														//the intensity of the color of the light
		pt.max_depth = 7;																//the max number a ray can reflect or refract


		pt.glInit("1");																	//the init of the openGL, input is the name of the window
		pt.render();																	//start path tracing!

		glutMainLoop();
	}
#endif
	//--------------------------------- the first scene -------------------------------------------------------------------------------



	//--------------------------------- the second scene ------------------------------------------------------------------------------
#if RUN_NUMBER == 2
	PathTracer pt;

	if (pt.loadObject("../../scene_2_obj/scene02.obj")) {							//below is the parameters for scene2.obj

		pt.setCamera(Vector3f(1.5, 8.0, 19), Vector3f(0, -0.35, -1), Vector3f(0, 1, 0));
		pt.setPixelSize(400, 300);
		pt.sample_num = 100;
		pt.env_color = Vector3f(1.0, 1.0, 1.0);
		pt.env_intensity = 0.95;
		pt.env_color *= pt.env_intensity;
		pt.light_color = Vector3f(1, 1, 1);
		pt.light_intensity = 4;
		pt.max_depth = 5;

		pt.glInit("2");
		pt.render();

		glutMainLoop();
	}
	
#endif
	//--------------------------------- the second scene ------------------------------------------------------------------------------


	//--------------------------------- use parameter file ----------------------------------------------------------------------------
#if RUN_NUMBER == 3
	// Or use parameter file to run without visual studio
	PathTracer pt;
	if (pt.loadParameterFile("../../parameter.txt")) {
		pt.glInit("3");
		pt.render();
		glutMainLoop();
	}
#endif
	//--------------------------------- use parameter file ----------------------------------------------------------------------------
	system("pause");
	return 0;
}
