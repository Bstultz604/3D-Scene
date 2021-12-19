#include <iostream>			//cout,cerr
#include <cstdlib>			//EXIT_FAILURE
#include <GL/glew.h>		//GLEW library
#include <GLFW/glfw3.h>		//GLFW library

//Texture Library
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

//Shader source files
#include "FragmentShaderSource.h"
#include "VertexShaderSource.h"
#include "lampFragmentShader.h"
#include "lampVertexShader.h"

//GLM Math Header Inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Camera class
#include <learnOpengl/camera.h>



using namespace std; // Uses the standard namespace

namespace {

	const char* const WINDOW_TITLE = "Brandon Stultz -- 3D Textures"; // macro for window title

	//variables for window width and height
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;

	//Stores the GL data telative to a given mesh
	struct GLMesh {
		GLuint vaos[5];				//handles vertex array objects
		GLuint vbos[10];			//handles vertex buffer objects

		GLuint ex_vaos[1];				//handles vertex array objects
		GLuint ex_vbos[2];			//handles vertex buffer objects

		GLuint eraser_N_indices;	//handles indices for eraser object
		GLuint plane_N_indices;		//Handles indices for 2D plane
		GLuint lamp_N_indices;		//Handles indices for light lamp
		GLuint pad_N_indices;		//Handles indices for pad object
		GLuint book_N_indices;		//Handles indices for marker object
	};

	//Traingle mesh data
	GLMesh mesh;

	//Main GLFW window
	GLFWwindow* window = nullptr;

	//Texture
	GLuint texture1;
	GLuint texture2;
	GLuint texture3;
	GLuint texture4;

	//Shader Program
	GLuint programID;
	GLuint lampID;

	//camera
	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
	float lastX = WINDOW_WIDTH / 2.0f;
	float lastY = WINDOW_HEIGHT / 2.0f;
	bool firstMouse = true;

	//timing
	float deltaTime = 0.0f;  //timing between current frame and last frame
	float lastFrame = 0.0f;

	//------------------KEY LIGHT-----------------------
	// key light color
	glm::vec3 keyObjectColor(1.0f, 0.2f, 0.0f);
	glm::vec3 keyLightColor(1.0f, 1.0f, 1.0f);

	//Key Light Intensity
	float keyLightIntensity(1.0f);

	// key Light position and scale
	glm::vec3 keyLightPosition(1.5f, 0.5f, 3.0f);
	glm::vec3 keyLightScale(0.3f);

	// Lamp animation
	bool gIsLampOrbiting = true;

}

//---------------------------------- - WINDOW------------------------------------------
// glfw: function is called when ever window size changes
void UResizeWindow(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void MousePositionCallback(GLFWwindow*, double xpos, double ypos);
void MouseScrollCallback(GLFWwindow*, double xoffset, double yoffset);
void MouseButtonCallback(GLFWwindow*, int button, int action, int mods);

//Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window) {

	//GLFW: intitialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//---------------------Initialzation and configuration for APPLE devices-------------
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FOWARD_COMPAT, GL_TRUE);
#endif
	//-----------------------------------------------------------------------------------

		//GLFW: window creation
	* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);

	//window terminating logic 
	if (*window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	//sets window as current context
	glfwMakeContextCurrent(*window);
	glfwSetFramebufferSizeCallback(*window, UResizeWindow);

	//Mouse call back function initialization
	glfwSetCursorPosCallback(*window, MousePositionCallback);
	glfwSetScrollCallback(*window, MouseScrollCallback);
	glfwSetMouseButtonCallback(*window, MouseButtonCallback);

	//Tell GlFW to capture out mouse 
	glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//GLEW: initialize
	glewExperimental = GL_TRUE;
	GLenum GlewInitResult = glewInit();

	//logic check to ensure GLEW was initialized successfully
	if (GLEW_OK != GlewInitResult) {
		std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
		return false;
	}

	//Display GPU OpenGL version
	std::cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

	return true;
}

//------------------------------------------------------------------------------------
//************************************************************************************
//--------------------------------------INPUT-----------------------------------------
void KeyBoardInput(GLFWwindow* window) {

	static float cameraSpeed = 0.15f;

	//Checks if escape key is pressed; if it is terminates window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraOffset = cameraSpeed * deltaTime;

	//Checks if the 'w' key is pressed; moves camera up
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}

	//checks if the 'S' key is pressed; moves camera down
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}

	//checks if the 'A' key is pressed; moves camera left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}

	//checks if 'D' key is pressed; moves camera right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	//Checks if 'Q' key is pressed; Moves camera up
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		camera.Position += cameraSpeed * camera.WorldUp;
	}

	//Checks if 'E' key is pressed; Moves camera down
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		camera.Position -= cameraSpeed * camera.WorldUp;
	}

}

//glfw: Whenever the mouse moves, this callback is called
void MousePositionCallback(GLFWwindow*, double xpos, double ypos) {

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // Reversed since y-coordinate goes from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

//glfw: Whenever the mouse scroll moves, this callback is called
void MouseScrollCallback(GLFWwindow*, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}

//glfw: Whenever the mouse buttons are pressed, this callback is called
void MouseButtonCallback(GLFWwindow*, int button, int action, int mods) {

	switch (button) {

	case GLFW_MOUSE_BUTTON_MIDDLE: {

		if (action == GLFW_PRESS)
			std::cout << "Middle mouse button pressed" << std::endl;
		else
			std::cout << "Middle mouse button released" << std::endl;
	}
								 break;

	case GLFW_MOUSE_BUTTON_LEFT: {

		if (action == GLFW_PRESS)
			std::cout << "Left mouse button pressed" << std::endl;
		else
			std::cout << "Left mouse button released" << std::endl;
	}
							   break;

	case GLFW_MOUSE_BUTTON_RIGHT: {

		if (action == GLFW_PRESS)
			std::cout << "Right mouse button pressed" << std::endl;
		else
			std::cout << "Right button released" << std::endl;
	}
								break;

	default:
		std::cout << "Unhandled mouse button event" << std::endl;
		break;
	}
}
//***********************************************************************************
//-------------------------------------MESH------------------------------------------
//Implements UCreateMesh Functiongbvbvbv                                                                     
void UCreateMesh(GLMesh& mesh) {

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// 
//_______________________________BUFFER CREATION & SETUP________________________________________
	//Creates vertex attribute pointer
	const GLuint floatsPerVertex = 3; //Number of cooridinates per float
	const GLuint floatsPerColor = 4; //(R, G, B, a)
	const GLuint floatsPerUV = 2;
	const GLuint floatsPerNormal = 3;

	//Stride between vertex cooridinates and color is 6 (x, y, z, R, G, B, a)
	GLuint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV + floatsPerNormal); //space between each vertexs elements

	//creates and binds array objects
	glGenVertexArrays(5, mesh.vaos); //Generates one vertex array object, storing it in 'vao'
	glGenVertexArrays(1, mesh.ex_vaos);

	//Creates two buffers: 1.) eraser data : 2.) eraser indices : 3.) plane Data : 4.) Plane indices : 5.) Lamp data : 6.) Lamp indices : 7.) Pad data : 8.) Pad indices : 9.) book data : 10.) book indices
	glGenBuffers(10, mesh.vbos); //creates eight buffers
	glGenBuffers(2, mesh.ex_vbos);

//----------------------------------------------------------------------------------------------
// =============================================================================================
//----------------------------------------------------------------------------------------------
//_____________________________________ERASER VERTICES__________________________________________ 
	//Binds to first of two VAO buffers(will hold eraser data and indices)
	glBindVertexArray(mesh.vaos[0]);
	//Binds VBO buffer that will store eraser vertex data
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // activates buffer

	//Specifies normalized device coordinates and RGB for eraser vertices
	GLfloat eraserVerts[] = {
		//index 0 - V:0
		-1.0f, 0.15f, 0.5f,			//0.)Top-Left-Front vertex
		1.0f, 0.0f, 0.0f, 1.0f,		//red
		0.0f, 0.0f, 1.0f,			//Normal :: Front face
		0.0f, 1.0f,					//Texture Coord :: Front Face

	//index 1 - V:0
		-1.0f, 0.15f, 0.5f,			//0.)Top-Left-Front vertex
		0.0f, 1.0f, 0.0f, 1.0f,		//green
		0.0f, 1.0f, 0.0f,			//Normal :: Top Face
		0.0f, 0.0f,					//Texture Coord :: Top Face

	//index 2 - V:0
		-1.0f, 0.15f, 0.5f,			//0.)Top-Left-Front vertex
		0.0f, 0.0f, 1.0f, 1.0f,		//blue
		-1.0f, 0.0f, 0.0f,			//Normal :: Left Face
		1.0f, 1.0f,					//Texture Coord :: Left Face
//------------------------------------------------------------------------------------
	//index 3 - V:1
		0.75f, 0.15f, 0.5f,			//1.)Top-Right-Front vertex
		1.0f, 0.0f, 0.0f, 1.0f,		//red
		0.0f, 1.0f, 0.0f,			//Normal :: Front face
		1.0f, 1.0f,					//Texture Coord :: Front Face

	//index 4 - V:1
		0.75f, 0.15f, 0.5f,			//1.)Top-Right-Front vertex
		0.0f, 1.0f, 0.0f, 1.0f,		//green
		0.0f, 1.0f, 0.0f,			//Normal :: Top Face
		1.0f, 0.0f,					//Texture Coord :: Top Face

	//index 5 - V:1
		0.75f, 0.15f, 0.5f,			//1.)Top-Right-Front vertex
		0.0f, 0.0f, 1.0f, 1.0f,		//blue
		1.0f, 0.0f, 0.0f,			//Normal :: Right Face
		0.0f, 1.0f,					//Texture Coord :: Right Face
//------------------------------------------------------------------------------------
	 //index 6 - V:2
		-0.75f, -0.15f, 0.5f,		//2.)Front-Bottom-left vertex 
		1.0f, 0.0f, 0.0f, 1.0f,		//red
		0.0f, 0.0f, 1.0f,			//Normal :: Front face
		0.0f, 0.0f,					//Texture Coord :: Front Face

	//index 7 - V:2
		-0.75f, -0.15f, 0.5f,		//2.)Front-Bottom-left vertex 
		0.0f, 1.0f, 0.0f, 1.0f,		//green
		0.0f, -1.0f, 0.0f,			//Normal :: Bottom Face 
		0.0f, 1.0f,					//Texture Coord :: Bottom Face

	//index 8 - V:2
		-0.75f, -0.15f, 0.5f,		//2.)Front-Bottom-left vertex 
		0.0f, 0.0f, 1.0f, 1.0f,		//blue
		-1.0f, 0.0f, 0.0f,			//Normal :: Left Face
		1.0f, 0.0f,					//Texture Coord :: Left Face
//------------------------------------------------------------------------------------
	//index 9 - V:3
		1.0f, -0.15f, 0.5f,			//3.)Front-Bottom-right Vertex
		1.0f, 0.0f, 0.0f, 1.0f,		//red
		0.0f, 0.0f, 1.0f,			//Normal :: Front face
		1.0f, 0.0f,					//Texture Coord :: Front Face

	//index 10 - V:3
		1.0f, -0.15f, 0.5f,			//3.)Front-Bottom-right Vertex
		0.0f, 1.0f, 0.0f, 1.0f,		//green
		0.0f, -1.0f, 0.0f,			//Normal :: Bottom Face
		1.0f, 1.0f,					//Texture Coord :: Bottom Face

	//index 11 - V:3
		1.0f, -0.15f, 0.5f,			//3.)Front-Bottom-right Vertex
		0.0f, 0.0f, 1.0f, 1.0f,		//blue
		1.0f, 0.0f, 0.0f,			//Normal :: Right Face
		0.0f, 0.0f,					//Texture Coord :: Right Face
//------------------------------------------------------------------------------------
	//index 12 - V:4
		-1.0f, 0.15f, -0.5f,		 //4.)Back-Top-Left vertex
		1.0f, 0.0f, 0.0f, 1.0f,		//red
		0.0f, 0.0f, -1.0f,			//Normal :: back Face
		1.0f, 1.0f,					//Texture Coord :: Back Face

	//index 13 - V:4
		-1.0f, 0.15f, -0.5f,		//4.)Back-Top-Left vertex
		0.0f, 1.0f, 0.0f, 1.0f,		//green
		0.0f, 1.0f, 0.0f,			//Normal :: Top Face
		1.0f, 0.0f,					//Texture Coord :: Top face

	//index 14 - V:4
		-1.0f, 0.15f, -0.5f,		//4.)Back-Top-Left vertex
		0.0f, 0.0f, 1.0f, 1.0f,		//blue
		-1.0f, 0.0f, 0.0f,			//Normal :: Left Face
		0.0f, 1.0f,					//Texture Coord :: Left Face
//------------------------------------------------------------------------------------
	//index 15 - V:5
		0.75f, 0.15f, -0.5f,		 //5.)Back-Top-Right vertex
		1.0f, 0.0f, 0.0f, 1.0f,		//red
		0.0f, 0.0f, -1.0f,			//Normal :: back Face
		0.0f, 1.0f,					//Texture Coord :: Back Face

	//index 16 - V:5
		0.75f, 0.15f, -0.5f,		//5.)Back-Top-Right vertex
		0.0f, 1.0f, 0.0f, 1.0f,		//green
		0.0f, 1.0f, 0.0f,			//Normal :: Top Face
		1.0f, 1.0f,					//Texture Coord :: Top Face

	//index 17 - V:5
		0.75f, 0.15f, -0.5f,		 //5.)Back-Top-Right vertex
		0.0f, 0.0f, 1.0f, 1.0f,		 //blue
		1.0f, 0.0f, 0.0f,			//Normal :: Right Face
		1.0f, 1.0f,					//Texture Coord:: Right Face
//------------------------------------------------------------------------------------
	//index 18 - V:6
		-0.75f, -0.15, -0.5f,		//6.)Back-Bottom-Left vertex
		1.0f, 0.0f, 0.0f, 1.0f,		//red
		0.0f, 0.0f, -1.0f,			//Normal :: back Face
		1.0, 0.0f,					//Texture Coord :: Back Face

	//index 19 - V:6
		-0.75f, -0.15, -0.5f,		//6.)Back-Bottom-Left vertex
		0.0f, 1.0f, 0.0f, 1.0f,		//green
		0.0f, -1.0f, 0.0f,			//Normal :: Bottom Face
		0.0f, 0.0f,					//Texture Coord :: Bottom Face

	//index 20 - V:6 
		-0.75f, -0.15, -0.5f,		//6.)Back-Bottom-Left vertex
		0.0f, 0.0f, 1.0f, 1.0f,		//blue
		-1.0f, 0.0f, 0.0f,			//Normal :: left Face
		0.0f, 0.0f,					//Texture Coord :: Left Face
//------------------------------------------------------------------------------------
	//index 21 - V:7
		1.0f, -0.15f, -0.5f,		//7.)Back-Bottom-Right Vertex
		1.0f, 0.0f, 0.0f, 1.0f,		//red
		-1.0f, 0.0f, 0.0f,			//Normal :: Left Face
		0.0f, 0.0f,					//Texture Coord :: Back Face

	//index 22 - V:7
		1.0f, -0.15f, -0.5f,		//7.)Back-Bottom-Right Vertex
		0.0f, 1.0f, 0.0f, 1.0f,		//green
		0.0f, -1.0f, 0.0f,			//Normal :: Bottom Face
		1.0f, 0.0f,					//Texture Coord :: Bottom face

	//index 23 - V:7
		1.0f, -0.15f, -0.5f,		//7.)Back-Bottom-Right Vertex
		0.0f, 0.0f, 1.0f, 1.0f,		//blue
		1.0f, 0.0f, 0.0f,			//Normal :: Right Face
		1.0f, 0.0f				    //Texture Coord :: Right Face
	};


	//Populates buffer with eraser vertex data
	glBufferData(GL_ARRAY_BUFFER, sizeof(eraserVerts), eraserVerts, GL_STATIC_DRAW); // sends vertex data to GPU and informs how to draw

//_____________________________________ERASER INDICES___________________________________________
	//Binds to second buffer(Populated with index data)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);

	//Creates buffer object for eraser indices
	GLushort eraserIndices[] = {
						  0, 3, 6,			//Triangle :: (Front Face)
						  6, 3, 9,			//Triangle :: (Front Face)

						  11, 5, 23,		//Triangle :: (Right Face)
						  23, 17, 5,		//Triangle :: (Right Face)

						  4, 1, 16,			//Triangle :: (Top Face)
						  16, 13, 1,		//Triangle :: (Top Face)

						  2, 8, 20,			//Triangle :: (Left Face)
						  20, 2, 14,		//Triangle :: (Left Face)

						  12, 15, 21,		//Triangle :: (Back Face)
						  21, 12, 18,		//Triangle :: (Back Face)

						  19, 22, 10,		//Triangle :: (Bottom Face)
						  10, 7, 19,		//Triangle :: (Bottom Face)
	};
	mesh.eraser_N_indices = sizeof(eraserIndices) / sizeof(eraserIndices[0]); //Specifies number of indices in memory buffer

	//Populates buffer with eraser index data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(eraserIndices), eraserIndices, GL_STATIC_DRAW);

	//______________________________________ERASER ATTRIBUTE________________________________________
		//Instructs GPU how to handle 'vbo'
		//// Parameters: attribPointerPosition | coordinates per vertex is 2, i.e., x and y |   data type | deactivate normalization |  0 strides | 0 offset
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	// Parameters: attribPointerPosition 1 | floats per color is 4, i.e., rgba | data type | deactivate normalization | 6 strides until the next color, i.e., rgbaxy | 2 floats until the beginning of each color, i.e,. xy
	//glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
	//glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerNormal)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* (floatsPerVertex + floatsPerColor)));
	glEnableVertexAttribArray(3);
	//----------------------------------------------------------------------------------------------
	//==============================================================================================
	//----------------------------------------------------------------------------------------------	
	//______________________________________PLANE VERTICES__________________________________________
		//Binds to first of two VAO buffers(will hold eraser data and indices)
	glBindVertexArray(mesh.vaos[1]);
	//Binds VBO buffer that will store eraser vertex data
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[2]); // activates buffer

	GLfloat planeVerts[] = {
		//index 0 - PLANE
		5.0f, 0.0f, -5.0f,			//Back-Right Vertex 
		1.0f, 0.0f, 1.0f, 1.0f,		//White
		0.0f, 1.0f, 0.0f,			//Normal :: Top Face
		1.0f, 1.0f,

		//index 1 - PLANE 
		-5.0f, 0.0f, -5.0f,			//Back-left Vertex 
		1.0f, 0.0f, 1.0f, 1.0f,		//White
		0.0f, 1.0f, 0.0f,			//Normal :: Top Face
		0.0f, 1.0f,

		//index 2 - PLANE 
		-5.0f, 0.0f, 5.0f,			//Front-left Vertex 
		1.0f, 0.0f, 1.0f, 1.0f,		//White
		0.0f, 1.0f, 0.0f,			//Normal :: Top Face
		0.0f, 0.0f,

		//index 3 - PLANE 
		5.0f, 0.0f, 5.0f,			//Front-Right Vertex 
		1.0f, 0.0f, 1.0f, 1.0f,		//White
		0.0f, 1.0f, 0.0f,			//Normal :: Top Face
		1.0f, 0.0f
	};

	//Populates buffer with eraser vertex data
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVerts), planeVerts, GL_STATIC_DRAW); // sends vertex data to GPU and informs how to draw

//_____________________________________PLANE INDICES____________________________________________
	//Binds to second buffer(Populated with index data)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[3]);

	//Creates buffer object for 2D plane
	GLushort planeIndices[] = {

		0, 1, 3,  //Plane Triangle 
		3, 2, 1   //Plane Trianlge
	};
	mesh.plane_N_indices = sizeof(planeIndices) / sizeof(planeIndices[0]); //Specifies number of indices in memory buffer

	//Populates buffer with plane index data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), planeIndices, GL_STATIC_DRAW);

	//______________________________________PLANE ATTRIBUTE________________________________________
	//Instructs GPU how to handle 'vbo'
	//// Parameters: attribPointerPosition | coordinates per vertex is 2, i.e., x and y |   data type | deactivate normalization |  0 strides | 0 offset
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	// Parameters: attribPointerPosition 1 | floats per color is 4, i.e., rgba | data type | deactivate normalization | 6 strides until the next color, i.e., rgbaxy | 2 floats until the beginning of each color, i.e,. xy
	glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* (floatsPerVertex + floatsPerColor + floatsPerNormal)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* (floatsPerVertex + floatsPerColor)));
	glEnableVertexAttribArray(3);

	//-------------------------------------------------------------------------------------
	//=====================================================================================
	//-------------------------------------------------------------------------------------
	//________________________________LAMP VERTICES________________________________________
	
	glBindVertexArray(mesh.vaos[2]);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[4]);

	GLfloat lampVerts[] = {

		//Index 0
			-0.5f, -0.5f, 0.5f,				//Front-Bottom-Left vertex
			1.0f, 1.0f, 1.0f, 1.0f,			//White
			0.0f, 0.0f, 1.0f,				//Normal :: Front face
			0.0f, 0.0f,						//texture Coord

		//Index 1
			0.5f, -0.5f, 0.5f,				//Front-Bottom-right vertex
			1.0f, 1.0f, 1.0f, 1.0f,			//White
			1.0f, 0.0f, 0.0f,				//Normal :: Left face
			0.0f, 0.0f,						//texture Coord

		//Index 2
			-0.5f, 0.5f, 0.5f,				//Front-top-Left vertex
			1.0f, 1.0f, 1.0f, 1.0f,			//White
			0.0f, 0.0f, 1.0f,				//Normal :: Front face
			0.0f, 0.0f,						//texture Coord

		//Index 3
			0.5f, 0.5f, 0.5f,				//Front-Top-Right vertex
			1.0f, 1.0f, 1.0f, 1.0f,			//White
			0.0f, 0.0f, 1.0f,				//Normal :: Front face
			0.0f, 0.0f,						//texture Coord

		//Index 4
			-0.5f, -0.5f, -0.5f,			//Back-Bottom-Left vertex
			1.0f, 1.0f, 1.0f, 1.0f,			//White
			0.0f, 0.0f, 1.0f,				//Normal :: Front face
			0.0f, 0.0f,						//texture Coord

		//Index 5
			0.5f, -0.5f, -0.5f,				//Back-Bottom-Right vertex
			1.0f, 1.0f, 1.0f, 1.0f,			//White
			0.0f, 0.0f, 1.0f,				//Normal :: Front face
			0.0f, 0.0f,						//texture Coord

		//Index 6
			-0.5f, 0.5f, -0.5f,				//back-Top-Left vertex
			1.0f, 1.0f, 1.0f, 1.0f,			//White
			0.0f, 0.0f, 1.0f,				//Normal :: Front face
			0.0f, 0.0f,						//texture Coord

		//Index 7
			0.5f, 0.5f, -0.5f,				//Back-Top-Right vertex
			1.0f, 1.0f, 1.0f, 1.0f,			//White
			0.0f, 0.0f, 1.0f,				//Normal :: Front face
			0.0f, 0.0f,						//texture Coord

	};

	//Populates buffer with lamp vertex data
	glBufferData(GL_ARRAY_BUFFER, sizeof(lampVerts), lampVerts, GL_STATIC_DRAW);

	//______________________________________LAMP INDICES_______________________________________

	//Binds to fourth buffer(Populated with index data)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[5]);

	//Creates buffer object for lamp
	GLushort lampIndices[] = {

		1, 0, 2,		//Front face
		2, 3, 1,		//Front face

		1, 5, 3,		//Right Face
		3, 7, 5,		//Right face

		5, 4, 7,		//Back Face
		7, 6, 4,		//back Face

		4, 6, 2,		//left Face
		2, 4, 1,		//Left Face	

		2, 6, 7,		//Top Face
		7, 2, 3,		//Top Face

		0, 4, 5,		//Bottom Face
		5, 0, 1			//Bottom Face

	};

	mesh.lamp_N_indices = sizeof(lampIndices) / sizeof(lampIndices[0]); //Specifies number of indices in memory buffer

	//Populates buffer with plane index data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(lampIndices), lampIndices, GL_STATIC_DRAW);

	//______________________________________LAMP ATTRIBUTE________________________________________
	//Instructs GPU how to handle 'vbo'
	//// Parameters: attribPointerPosition | coordinates per vertex is 2, i.e., x and y |   data type | deactivate normalization |  0 strides | 0 offset
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	// Parameters: attribPointerPosition 1 | floats per color is 4, i.e., rgba | data type | deactivate normalization | 6 strides until the next color, i.e., rgbaxy | 2 floats until the beginning of each color, i.e,. xy
	glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* floatsPerVertex));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* (floatsPerVertex + floatsPerColor + floatsPerNormal)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* (floatsPerVertex + floatsPerColor)));
	glEnableVertexAttribArray(3);

	//-------------------------------------------------------------------------------------
	//=====================================================================================
	//-------------------------------------------------------------------------------------
	//________________________________PAD VERTICES________________________________________

	glBindVertexArray(mesh.vaos[3]);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[6]);

	GLfloat padVerts[] = {

		//Left-Top-Front Vertex
			//index 0
				-1.0f, 0.25f, 1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, 0.0f, 1.0f,           //Normal :: FRONT FACE
				0.0f, 1.0f,					//Texture :: FRONT FACE - TOP-LEFT

			//index 1
				-1.0f, 0.25f, 1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, 1.0f, 0.0f,			//Normal :: TOP FACE
				0.0f, 1.0f,					//Texture :: TOP FACE - BOTTOM-LEFT

			//index 2
				-1.0f, 0.25f, 1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				-1.0f, 0.0f, 0.0f,			//Normal :: LEFT FACE
				1.0f, 1.0f,					//Texture :: LEFT FACE - TOP-RIGHT
	//----------------------------------------------------------------------------------------
		//Left-bottom-Front Vertex 
			//index 3
				-1.0f, -0.25f, 1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, 0.0f, 1.0f,			//Normal :: FRONT FACE
				0.0f, 0.0f,					//Texture :: FRONT FACE - BOTTOM-LEFT

			//index 4
				-1.0f, -0.25f, 1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, -1.0f, 0.0f,			//Normal :: BOTTOM FACE
				0.0f, 1.0f,					//Texture :: BOTTOM FACE - TOP-LEFT

			//index 5
				-1.0f, -0.25f, 1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				-1.0f, 0.0f, 0.0f,			//Normal :: LEFT FACE
				1.0f, 0.0f,					//Texture :: LEFT FACE - BOTTOM-RIGHT
	//----------------------------------------------------------------------------------------
		//Right-bottom-Front Vertex 
			//index 6
				1.0f, -0.25f, 1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, 0.0f, 1.0f,			//Normal :: FRONT FACE
				1.0f, 0.0f,					//Texture :: FRONT FACE - BOTTOM-RIGHT

			//index 7
				1.0f, -0.25f, 1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, -1.0f, 0.0f,			//Normal :: BOTTOM FACE
				1.0f, 1.0f,					//Texture :: BOTTOM FACE - TOP-RIGHT

			//index 8
				1.0f, -0.25f, 1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				1.0f, 0.0f, 0.0f,			//Normal :: RIGHT FACE
				0.0f, 0.0f,					//Texture :: RIGHT FACE - BOTTOM-LEFT
	//----------------------------------------------------------------------------------------
		//Right-Top-Front Vertex 
			//index 9
				1.0f, 0.25f, 1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, 0.0f, 1.0f,			//Normal :: FRONT FACE
				1.0f, 1.0f,					//Texture :: FRONT FACE - TOP-RIGHT

			//index 10
				1.0f, 0.25f, 1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, 1.0f, 0.0f,			//Normal :: TOP FACE
				1.0f, 0.0f,					//Texture :: TOP FACE - BOTTOM-RIGHT

			//index 11
				1.0f, 0.25f, 1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				1.0f, 0.0f, 0.0f,			//Normal :: RIGHT FACE
				0.0f, 1.0f,					//Texture :: RIGHT FACE - TOP-LEFT
	//----------------------------------------------------------------------------------------
		//Left-Top-Back Vertex 
			//index 12
				-1.0f, 0.25f, -1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, 0.0f, -1.0f,			//Normal :: BACK FACE
				1.0f, 1.0f,					//Texture :: BACK FACE - TOP-RIGHT

			//index 13
				-1.0f, 0.25f, -1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, 1.0f, 0.0f,			//Normal :: TOP FACE
				0.0f, 1.0f,					//Texture :: TOP FACE - TOP-LEFT

			//index 14
				-1.0f, 0.25f, -1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				-1.0f, 0.0f, 0.0f,			//Normal :: LEFT FACE
				0.0f, 1.0f,					//Texture :: LEFT FACE - TOP-LEFT
	//----------------------------------------------------------------------------------------
		//Left-Bottom-Back Vertex 
			//index 15
				-1.0f, -0.25f, -1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, 0.0f, -1.0f,			//Normal :: BACK FACE
				1.0f, 0.0f,					//Texture :: BACK FACE - BOTTOM-RIGHT

			//index 16
				-1.0f, -0.25f, -1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, -1.0f, 0.0f,			//Normal :: BOTTOM FACE
				0.0f, 0.0f,					//Texture :: BOTTOM FACE - BOTTOM-LEFT

			//index 17
				-1.0f, -0.25f, -1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				-1.0f, 0.0f, 0.0f,			//Normal :: LEFT FACE
				0.0f, 0.0f,					//Texture :: LEFT FACE - BOTTOM-LEFT
	//----------------------------------------------------------------------------------------
		//Right-Bottom-Back Vertex 
			//index 18
				1.0f, -0.25f, -1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, 0.0f, -1.0f,			//Normal :: BACK FACE
				0.0f, 0.0f,					//Texture :: BACK FACE - BOTTOM-LEFT

			//index 19
				1.0f, -0.25f, -1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, -1.0f, 0.0f,			//Normal :: BOTTOM FACE
				1.0f, 0.0f,					//Texture :: BOTTOM FACE - BOTTOM-RIGHT

			//index 20
				1.0f, -0.25f, -1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				1.0f, 0.0f, 0.0f,			//Normal :: RIGHT FACE
				1.0f, 0.0f,					//Texture :: RIGHT FACE - BOTTOM-RIGHT
	//----------------------------------------------------------------------------------------
		//Right-Top-Back Vertex 
			//index 21
				1.0f, 0.25f, -1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, 0.0f, -1.0f,			//Normal :: BACK FACE
				0.0f, 1.0f,					//Texture :: BACK FACE - TOP-LEFT

			//index 22
				1.0f, 0.25f, -1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, 1.0f, 0.0f,			//Normal :: TOP FACE
				1.0f, 1.0f,					//Texture :: TOP FACE - TOP-RIGHT

			//index 23
				1.0f, 0.25f, -1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				1.0f, 0.0f, 0.0f,			//Normal :: RIGHT FACE
				1.0f, 1.0f					//Texture :: RIGHT FACE - TOP-RIGHT

	};

	//Populates buffer with pad vertex data
	glBufferData(GL_ARRAY_BUFFER, sizeof(padVerts), padVerts, GL_STATIC_DRAW);

	//______________________________________PAD INDICES_______________________________________

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[7]);

	GLushort padIndices[] = {

		0, 3, 6,		//Front 
		6, 9, 0,		//Front

		11, 8, 20,		//Right
		20, 23, 11,		//Right

		12, 15, 18,		//Back
		18, 21, 12,		//Back

		14, 17, 5,		//Left
		5, 2, 14,		//Left

		13, 1, 10,		//Top
		10, 22, 13,		//Top

		16, 4, 7,		//Bottom
		7, 19, 16		//Bottom
	};

	mesh.pad_N_indices = sizeof(padIndices) / sizeof(padIndices[0]); //Specifies number of indices in memory buffer

	//Populates buffer with pad index data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(padIndices), padIndices, GL_STATIC_DRAW);

	//______________________________________PAD ATTRIBUTE________________________________________

	//Instructs GPU how to handle 'vbo'
	//// Parameters: attribPointerPosition | coordinates per vertex is 2, i.e., x and y |   data type | deactivate normalization |  0 strides | 0 offset
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	// Parameters: attribPointerPosition 1 | floats per color is 4, i.e., rgba | data type | deactivate normalization | 6 strides until the next color, i.e., rgbaxy | 2 floats until the beginning of each color, i.e,. xy
	glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* floatsPerVertex));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* (floatsPerVertex + floatsPerColor + floatsPerNormal)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* (floatsPerVertex + floatsPerColor)));
	glEnableVertexAttribArray(3);

	//-------------------------------------------------------------------------------------
	//=====================================================================================
	//-------------------------------------------------------------------------------------
	//________________________________BOOK VERTICES________________________________________

	glBindVertexArray(mesh.vaos[4]);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[8]);

	GLfloat bookVerts[] = {

		//Left-Top-Front Vertex
			//index 0
				-1.0f, 0.25f, 1.5f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, 0.0f, 1.0f,           //Normal :: FRONT FACE
				0.0f, 1.0f,					//Texture :: FRONT FACE - TOP-LEFT

			//index 1
				-1.0f, 0.25f, 1.5f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, 1.0f, 0.0f,			//Normal :: TOP FACE
				0.0f, 1.0f,					//Texture :: TOP FACE - BOTTOM-LEFT

			//index 2
				-1.0f, 0.25f, 1.5f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				-1.0f, 0.0f, 0.0f,			//Normal :: LEFT FACE
				1.0f, 1.0f,					//Texture :: LEFT FACE - TOP-RIGHT
	//----------------------------------------------------------------------------------------
		//Left-bottom-Front Vertex 
			//index 3
				-1.0f, -0.25f, 1.5f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, 0.0f, 1.0f,			//Normal :: FRONT FACE
				0.0f, 0.0f,					//Texture :: FRONT FACE - BOTTOM-LEFT

			//index 4
				-1.0f, -0.25f, 1.5f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, -1.0f, 0.0f,			//Normal :: BOTTOM FACE
				0.0f, 1.0f,					//Texture :: BOTTOM FACE - TOP-LEFT

			//index 5
				-1.0f, -0.25f, 1.5f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				-1.0f, 0.0f, 0.0f,			//Normal :: LEFT FACE
				1.0f, 0.0f,					//Texture :: LEFT FACE - BOTTOM-RIGHT
	//----------------------------------------------------------------------------------------
		//Right-bottom-Front Vertex 
			//index 6
				1.0f, -0.25f, 1.5f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, 0.0f, 1.0f,			//Normal :: FRONT FACE
				1.0f, 0.0f,					//Texture :: FRONT FACE - BOTTOM-RIGHT

			//index 7
				1.0f, -0.25f, 1.5f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, -1.0f, 0.0f,			//Normal :: BOTTOM FACE
				1.0f, 1.0f,					//Texture :: BOTTOM FACE - TOP-RIGHT

			//index 8
				1.0f, -0.25f, 1.5f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				1.0f, 0.0f, 0.0f,			//Normal :: RIGHT FACE
				0.0f, 0.0f,					//Texture :: RIGHT FACE - BOTTOM-LEFT
	//----------------------------------------------------------------------------------------
		//Right-Top-Front Vertex 
			//index 9
				1.0f, 0.25f, 1.5f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, 0.0f, 1.0f,			//Normal :: FRONT FACE
				1.0f, 1.0f,					//Texture :: FRONT FACE - TOP-RIGHT

			//index 10
				1.0f, 0.25f, 1.5f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, 1.0f, 0.0f,			//Normal :: TOP FACE
				1.0f, 0.0f,					//Texture :: TOP FACE - BOTTOM-RIGHT

			//index 11
				1.0f, 0.25f, 1.5f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				1.0f, 0.0f, 0.0f,			//Normal :: RIGHT FACE
				0.0f, 1.0f,					//Texture :: RIGHT FACE - TOP-LEFT
	//----------------------------------------------------------------------------------------
		//Left-Top-Back Vertex 
			//index 12
				-1.0f, 0.25f, -1.5f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, 0.0f, -1.0f,			//Normal :: BACK FACE
				1.0f, 1.0f,					//Texture :: BACK FACE - TOP-RIGHT

			//index 13
				-1.0f, 0.25f, -1.5f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, 1.0f, 0.0f,			//Normal :: TOP FACE
				0.0f, 1.0f,					//Texture :: TOP FACE - TOP-LEFT

			//index 14
				-1.0f, 0.25f, -1.5f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				-1.0f, 0.0f, 0.0f,			//Normal :: LEFT FACE
				0.0f, 1.0f,					//Texture :: LEFT FACE - TOP-LEFT
	//----------------------------------------------------------------------------------------
		//Left-Bottom-Back Vertex 
			//index 15
				-1.0f, -0.25f, -1.5f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, 0.0f, -1.0f,			//Normal :: BACK FACE
				1.0f, 0.0f,					//Texture :: BACK FACE - BOTTOM-RIGHT

			//index 16
				-1.0f, -0.25f, -1.5f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, -1.0f, 0.0f,			//Normal :: BOTTOM FACE
				0.0f, 0.0f,					//Texture :: BOTTOM FACE - BOTTOM-LEFT

			//index 17
				-1.0f, -0.25f, -1.5f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				-1.0f, 0.0f, 0.0f,			//Normal :: LEFT FACE
				0.0f, 0.0f,					//Texture :: LEFT FACE - BOTTOM-LEFT
	//----------------------------------------------------------------------------------------
		//Right-Bottom-Back Vertex 
			//index 18
				1.0f, -0.25f, -1.5f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, 0.0f, -1.0f,			//Normal :: BACK FACE
				0.0f, 0.0f,					//Texture :: BACK FACE - BOTTOM-LEFT

			//index 19
				1.0f, -0.25f, -1.5f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, -1.0f, 0.0f,			//Normal :: BOTTOM FACE
				1.0f, 0.0f,					//Texture :: BOTTOM FACE - BOTTOM-RIGHT

			//index 20
				1.0f, -0.25f, -1.5f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				1.0f, 0.0f, 0.0f,			//Normal :: RIGHT FACE
				1.0f, 0.0f,					//Texture :: RIGHT FACE - BOTTOM-RIGHT
	//----------------------------------------------------------------------------------------
		//Right-Top-Back Vertex 
			//index 21
				1.0f, 0.25f, -1.5f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, 0.0f, -1.0f,			//Normal :: BACK FACE
				0.0f, 1.0f,					//Texture :: BACK FACE - TOP-LEFT

			//index 22
				1.0f, 0.25f, -1.5f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				0.0f, 1.0f, 0.0f,			//Normal :: TOP FACE
				1.0f, 1.0f,					//Texture :: TOP FACE - TOP-RIGHT

			//index 23
				1.0f, 0.25f, -1.5f,
				1.0f, 1.0f, 1.0f, 1.0f,		//Color :: White
				1.0f, 0.0f, 0.0f,			//Normal :: RIGHT FACE
				1.0f, 1.0f					//Texture :: RIGHT FACE - TOP-RIGHT//Left-Top-Front Vertex
			
	};

	//Populates buffer with book vertex data
	glBufferData(GL_ARRAY_BUFFER, sizeof(bookVerts), bookVerts, GL_STATIC_DRAW);

	//______________________________________BOOK INDICES_______________________________________

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[9]);

	GLushort bookIndices[] = {

		0, 3, 6,		//Front 
		6, 9, 0,		//Front

		11, 8, 20,		//Right
		20, 23, 11,		//Right

		12, 15, 18,		//Back
		18, 21, 12,		//Back

		14, 17, 5,		//Left
		5, 2, 14,		//Left

		13, 1, 10,		//Top
		10, 22, 13,		//Top

		16, 4, 7,		//Bottom
		7, 19, 16		//Bottom
	};

	mesh.book_N_indices = sizeof(bookIndices) / sizeof(bookIndices[0]); //Specifies number of indices in memory buffer

	//Populates buffer with plane index data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bookIndices), bookIndices, GL_STATIC_DRAW);

	//______________________________________BOOK ATTRIBUTE________________________________________

	//Instructs GPU how to handle 'vbo'
	//// Parameters: attribPointerPosition | coordinates per vertex is 2, i.e., x and y |   data type | deactivate normalization |  0 strides | 0 offset
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	// Parameters: attribPointerPosition 1 | floats per color is 4, i.e., rgba | data type | deactivate normalization | 6 strides until the next color, i.e., rgbaxy | 2 floats until the beginning of each color, i.e,. xy
	glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* floatsPerVertex));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* (floatsPerVertex + floatsPerColor + floatsPerNormal)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* (floatsPerVertex + floatsPerColor)));
	glEnableVertexAttribArray(3);

	
};	
	

//Deletes buffers and vertex arrays
void UDestroyMesh(GLMesh& mesh) {
	glDeleteVertexArrays(5, mesh.vaos);  //deletes two vertex arrays in specified memory location
	glDeleteBuffers(10, mesh.vbos);  //deletes four buffers in specified memory location
}

//-------------------------------------------------------------------------------------
//*************************************************************************************
//------------------------------------TEXTURE------------------------------------------
//Images are loaded with 'Y' axis going down, OpenGl's 'Y' axis goes up, This flips the imiage to match the axis
void flipImageVertically(unsigned char* image, int width, int height, int channels) {

	for (int j = 0; j < height / 2; ++j) {
		int index1 = j * width * channels;
		int index2 = (height - 1 - j) * width * channels;

		for (int i = width * channels; i > 0; --i) {
			unsigned char tmp = image[index1];
			image[index1] = image[index2];
			image[index2] = tmp;
			++index1;
			++index2;
		}
	}
}

/*Generate and load the texture*/
bool CreateTexture(const char* filename, GLuint& textureId)
{
	int width, height, channels;
	unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
	if (image)
	{
		flipImageVertically(image, width, height, channels);

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		else if (channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			cout << "Not implemented to handle image with " << channels << " channels" << endl;
			return false;
		}

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		return true;
	}

	// Error loading the image
	return false;
}

void DestroyTexture(GLuint textureID) {
	glGenTextures(1, &textureID);
}

//----------------------------------------------------------------------------------------------
//**********************************************************************************************
//------------------------------------SHADER PROGRAM--------------------------------------------
//Function called to render a frame
void URender() {

	// Lamp orbits around the origin
	const float angularVelocity = glm::radians(45.0f);
	if (gIsLampOrbiting)
	{
		glm::vec4 newPosition = glm::rotate(angularVelocity * deltaTime, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(keyLightPosition, 1.0f);
		keyLightPosition.x = newPosition.x;
		keyLightPosition.y = newPosition.y;
		keyLightPosition.z = newPosition.z;
	}

	//Enable z-depth
	glEnable(GL_DEPTH_TEST);

	//clear the background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //sets background as black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//View Matrix: Transforms the camera
	glm::mat4 view = camera.GetViewMatrix();

	//Creates a persepctive projection
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
	//glm::mat4 projection = glm::ortho(glm::radians(camera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 1.0f, 100.0f);

	//set the shader to use
	glUseProgram(programID);

	//Retrives and passes transformation to the shader program
	GLint eraserModelLocation = glGetUniformLocation(programID, "model");
	GLint eraserViewMatrixLocation = glGetUniformLocation(programID, "view");
	GLint eraserProjectionMatrixLocation = glGetUniformLocation(programID, "projection");
	GLint eraserUVScaleLocation = glGetUniformLocation(programID, "uvScale");

	//Retrives and passes transformation to the shader program
	GLint planeModelLocation = glGetUniformLocation(programID, "model");
	GLint planeViewMatrixLocation = glGetUniformLocation(programID, "view");
	GLint planeProjectionMatrixLocation = glGetUniformLocation(programID, "projection");
	GLint planeUVScaleLocation = glGetUniformLocation(programID, "uvScale");

	//Retrives and passes transformation to the shader program
	GLint padModelLocation = glGetUniformLocation(programID, "model");
	GLint padViewMatrixLocation = glGetUniformLocation(programID, "view");
	GLint padProjectionMatrixLocation = glGetUniformLocation(programID, "projection");
	GLint padUVScaleLocation = glGetUniformLocation(programID, "uvScale");

	//Retrives and passes transformation to the shader program
	GLint bookModelLocation = glGetUniformLocation(programID, "model");
	GLint bookViewMatrixLocation = glGetUniformLocation(programID, "view");
	GLint bookProjectionMatrixLocation = glGetUniformLocation(programID, "projection");
	GLint bookUVScaleLocation = glGetUniformLocation(programID, "uvScale");

	// Reference matrix uniforms from the Lamp Shader program
	GLint lampModelLocation = glGetUniformLocation(lampID, "model");
	GLuint lampViewMatrixLocation = glGetUniformLocation(lampID, "view");
	GLuint lampProjectionMatrixLocation = glGetUniformLocation(lampID, "projection");

	// Reference matrix uniforms from the Cube Shader program for the cub color, light color, light position, and camera position
	GLint objectColorLoc = glGetUniformLocation(programID, "objectColor");
	GLint lightColorLoc = glGetUniformLocation(programID, "lightColor");
	GLint lightPositionLoc = glGetUniformLocation(programID, "lightPos");
	GLint lightIntesnity = glGetUniformLocation(programID, "specIntensity");
	GLint viewPositionLoc = glGetUniformLocation(programID, "viewPosition");

//-------------------------------------------------------------------------------------
//******************************ERASER RENDER*******************************************
//-------------------------------------------------------------------------------------

		//Activate the VBO contained within the mesh VAO
	glBindVertexArray(mesh.vaos[0]);

	glm::vec2 eraserUVScale(1.0f, 1.0f);

	// 1. Scales the object by 'n'
	glm::mat4 E_scale = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));
	// 2. Rotates shape by 'n' degrees in the x axis
	glm::mat4 E_rotation = glm::rotate(0.0f, glm::vec3(1.0, 0.0f, 0.0f));
	// 3. Place object at the 'n'
	glm::vec3 E_location = glm::vec3(-1.0f, -0.39f, 2.0f);
	// 4. Translates object to location
	glm::mat4 E_translation = glm::translate(E_location);
	// Model matrix: transformations are applied right-to-left order
	glm::mat4 E_model = E_translation * E_rotation * E_scale;

	glUniformMatrix4fv(eraserModelLocation, 1, GL_FALSE, glm::value_ptr(E_model));
	glUniformMatrix4fv(eraserViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(eraserProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform2fv(eraserUVScaleLocation, 1, glm::value_ptr(eraserUVScale));

	// Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
	glUniform3f(objectColorLoc, keyObjectColor.r, keyObjectColor.g, keyObjectColor.b);
	glUniform3f(lightColorLoc, keyLightColor.r, keyLightColor.g, keyLightColor.b);
	glUniform3f(lightPositionLoc, keyLightPosition.x, keyLightPosition.y, keyLightPosition.z);
	glUniform1f(lightIntesnity, keyLightIntensity);

	//bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, mesh.eraser_N_indices, GL_UNSIGNED_SHORT, NULL);

	//-------------------------------------------------------------------------------------
	//******************************PLANE RENDER*******************************************
	//-------------------------------------------------------------------------------------

		//Activate the VBO containing the Plane Data
	glBindVertexArray(mesh.vaos[1]);

	glm::vec2 planeUVScale(1.0f, 1.0f);

	// 1. Scales the object by 'n'
	glm::mat4 P_scale = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
	// 2. Rotates shape by 'n' degrees in the x axis
	glm::mat4 P_rotation = glm::rotate(0.0f, glm::vec3(1.0, 0.0f, 0.0f));
	// 3. Place object at the 'n'
	glm::vec3 P_location = glm::vec3(0.0f, -1.0f, 0.0f);
	// 4. Translates object to location
	glm::mat4 P_translation = glm::translate(P_location);
	// Model matrix: transformations are applied right-to-left order
	glm::mat4 P_model = P_translation * P_rotation * P_scale;

	glUniformMatrix4fv(planeModelLocation, 1, GL_FALSE, glm::value_ptr(P_model));
	glUniformMatrix4fv(planeViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(planeProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform2fv(planeUVScaleLocation, 1, glm::value_ptr(planeUVScale));

	//bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture2);

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, mesh.plane_N_indices, GL_UNSIGNED_SHORT, NULL);

//-------------------------------------------------------------------------------------
//******************************PAD RENDER*********************************************
//-------------------------------------------------------------------------------------

	//Activate the VBO containing the Pad Data
	glBindVertexArray(mesh.vaos[3]);

	glm::vec2 padUVScale(1.0f, 1.0f);

	// 1. Scales the object by 'n'
	glm::mat4 Pad_scale = glm::scale(glm::vec3(0.75f, 0.75f, 0.75f));
	// 2. Rotates shape by 'n' degrees in the x axis
	glm::mat4 Pad_rotation = glm::rotate(0.0f, glm::vec3(1.0, -1.92f, 0.0f));
	// 3. Place object at the 'n'
	glm::vec3 Pad_location = glm::vec3(2.0f, -0.80f, -2.0f);
	// 4. Translates object to location
	glm::mat4 Pad_translation = glm::translate(Pad_location);
	// Model matrix: transformations are applied right-to-left order
	glm::mat4 Pad_model = Pad_translation * Pad_rotation * Pad_scale;

	glUniformMatrix4fv(padModelLocation, 1, GL_FALSE, glm::value_ptr(Pad_model));
	glUniformMatrix4fv(padViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(padProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform2fv(padUVScaleLocation, 1, glm::value_ptr(padUVScale));

	//bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture3);

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, mesh.pad_N_indices, GL_UNSIGNED_SHORT, NULL);

//-------------------------------------------------------------------------------------
//******************************BOOK RENDER*********************************************
//-------------------------------------------------------------------------------------

	//Activate the VBO containing the book Data
	glBindVertexArray(mesh.vaos[4]);

	glm::vec2 bookUVScale(1.0f, 1.0f);

	// 1. Scales the object by 'n'
	glm::mat4 book_scale = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
	// 2. Rotates shape by 'n' degrees in the x axis
	glm::mat4 book_rotation = glm::rotate(45.0f, glm::vec3(0.0, 1.0f, 0.0f));
	// 3. Place object at the 'n'
	glm::vec3 book_location = glm::vec3(-1.0f, -0.72f, 2.0f);
	// 4. Translates object to location
	glm::mat4 book_translation = glm::translate(book_location);
	// Model matrix: transformations are applied right-to-left order
	glm::mat4 book_model = book_translation * book_rotation * book_scale;

	glUniformMatrix4fv(bookModelLocation, 1, GL_FALSE, glm::value_ptr(book_model));
	glUniformMatrix4fv(bookViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(bookProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform2fv(bookUVScaleLocation, 1, glm::value_ptr(bookUVScale));

	//bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture4);

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, mesh.book_N_indices, GL_UNSIGNED_SHORT, NULL);

	//---------------------------------LAMP RENDER-----------------------------------------
	
	glUseProgram(lampID);

	glBindVertexArray(mesh.vaos[2]);

	//Transform the smaller cube used as a visual que for the light source
	glm::mat4 L_model = glm::translate(keyLightPosition) * glm::scale(keyLightScale);

	glUniformMatrix4fv(lampModelLocation, 1, GL_FALSE, glm::value_ptr(L_model));
	glUniformMatrix4fv(lampViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(lampProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projection));

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, mesh.lamp_N_indices, GL_UNSIGNED_SHORT, NULL);

	//Deactivate the VAO;
	glBindVertexArray(0);

	//glfw: swap buffers and poll IO
	glfwSwapBuffers(window);
};

//Implements UCreateShader
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programID) {

	//Compilation and linkage error reporting
	int success = 0;
	char infoLog[512];

	//create shader program object
	programID = glCreateProgram();

	//create the vertex and fragment shader objects
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	//retrieves the source for shaders
	glShaderSource(vertexShaderID, 1, &vtxShaderSource, NULL);
	glShaderSource(fragmentShaderID, 1, &fragShaderSource, NULL);

	//Compile vertex shader and print any errors
	glCompileShader(vertexShaderID);  //compiles the vertex shader
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);  //vertex shader error check logic
	if (!success) {
		glGetShaderInfoLog(vertexShaderID, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;
	}

	//compile fragment shader and prints any errors
	glCompileShader(fragmentShaderID);  //compile the fragment shader
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);  //fragment shader error check logic
	if (!success) {
		glGetShaderInfoLog(fragmentShaderID, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;
	}
	//Attach compiled shaders to the shader program
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);

	//link shaders together in program and checks for link errors
	glLinkProgram(programID); //  links hte shader programs together
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programID, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED" << infoLog << std::endl;

		return false;
	}

	glUseProgram(programID);  //Uses the shader program

	return true;

}

//releases shader program
void UDestroyShaderProgram(GLuint programID) {
	glDeleteProgram(programID);
}
//-----------------------------------------------------------------------------------

/*User-defined Function prototypes to:
* initialize the program, set the window size,
* redraw graphics on the window when resized,
* and render graphics on the screeen
*/
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void KeyBoardInput(GLFWwindow* window);

void MousePositionCallback(GLFWwindow*, double xpos, double ypos);
void MouseScrollCallback(GLFWwindow*, double xoffset, double yoffset);
void MouseButtonCallback(GLFWwindow*, int button, int action, int mods);

void UCreateMesh(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragmentShaderSource, GLuint& programID);
void UDestroyShaderProgram(GLuint programID);

//main function. Entry point to the OpenGL program
int main(int argc, char* argv[]) {

	if (!UInitialize(argc, argv, &window))
		return EXIT_FAILURE;

	//Create the mesh
	UCreateMesh(mesh);  //calls function to create vertex buffer object

	//create the shader program
	if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, programID))
		return EXIT_FAILURE;

	if (!UCreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource, lampID))
		return EXIT_FAILURE;
	

	//Load texture
	const char* brickTexFileName = "../../Final_3D_Scene/Eraser_Texture.jpg";

	if (!CreateTexture(brickTexFileName, texture1)) {

		cout << "Failed to load texture " << brickTexFileName << endl;
		return EXIT_FAILURE;
	}

	//Load texture
	const char* tableTexFileName = "../../Final_3D_Scene/Table_Texture.jpg";

	if (!CreateTexture(tableTexFileName, texture2)) {

		cout << "Failed to load texture " << tableTexFileName << endl;
		return EXIT_FAILURE;
	}

	//Load texture
	const char* padTexFileName = "../../Final_3D_Scene/Pad_Texture.jpg";

	if (!CreateTexture(padTexFileName, texture3)) {

		cout << "Failed to load texture " << padTexFileName << endl;
		return EXIT_FAILURE;
	}

	//Load texture
	const char* indexTexFileName = "../../Final_3D_Scene/Index_Texture.jpg";

	if (!CreateTexture(indexTexFileName, texture4)) {

		cout << "Failed to load texture " << indexTexFileName << endl;
		return EXIT_FAILURE;
	}

	//Tells OpenGL which sampler texture unit it belongs to(need only be done once)
	glUseProgram(programID);

	//Sets the exture as texture unit 0
	glUniform1i(glGetUniformLocation(programID, "Texture"), 0);

	//sets background color of window to black
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


	//render loop
	while (!glfwWindowShouldClose(window)) {

		//per-frame timing
		//-----------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Input
		KeyBoardInput(window);

		//render this frame
		URender();

		glfwPollEvents();
	}

	//Release mesh data
	UDestroyMesh(mesh);

	//Release Texture
	DestroyTexture(texture1);
	DestroyTexture(texture2);
	DestroyTexture(texture3);
	DestroyTexture(texture4);

	//release shader program
	UDestroyShaderProgram(programID);
	UDestroyShaderProgram(lampID);

	exit(EXIT_SUCCESS); //Terminates the program sucessfully 
}