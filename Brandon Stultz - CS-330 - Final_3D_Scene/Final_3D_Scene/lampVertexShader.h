#ifndef LAMPVERTSHADE_H
#define LAMPVERTSHADE_H


const char* lampVertexShaderSource = "#version 440 core\n"

"layout (location = 0) in vec3 aPos;\n"		//Lamp position data

//Uniform for Transformation matrices
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"

"void main()\n"
"{\n"

"	gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"

"}\0";

#endif