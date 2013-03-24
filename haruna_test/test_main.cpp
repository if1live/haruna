// Ŭnicode please 
#include "test_stdafx.h"

int zlib_test(int argc, char *argv[])
{
	/*
	int ret;

	// do compression if no arguments 
	if (argc == 1) {
		ret = def(stdin, stdout, Z_DEFAULT_COMPRESSION);
		if (ret != Z_OK)
			zerr(ret);
		return ret;
	}

	// do decompression if -d specified 
	else if (argc == 2 && strcmp(argv[1], "-d") == 0) {
		ret = inf(stdin, stdout);
		if (ret != Z_OK)
			zerr(ret);
		return ret;
	}

	else {
		fputs("zpipe usage: zpipe [-d] < source > dest\n", stderr);
		return 1;
	}
	*/
	return 0;
}
int main(int argc, char *argv[])
{
	zlib_test(argc, argv);


	if(!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	int width = 320;
	int height = 240;
	if(!glfwOpenWindow(width, height, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	GLenum err = glewInit();
	if(GLEW_OK != err) {
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));		
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	::testing::InitGoogleTest(&argc, argv);
	int result = RUN_ALL_TESTS();

	glfwTerminate();

	getchar();
	return 0;	
}
