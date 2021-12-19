#ifndef VERTSHADE_H
#define VERTSHADE_H

// vertex shader program source code
const char* vertexShaderSource = "#version 440 core\n"

"layout (location = 0) in vec3 aPos;\n"						//Vertex Position Data
"layout (location = 2) in vec2 textureCoordinate;\n"		//Texture Position Data
"layout (location = 3) in vec3 normal;\n"					//Normals Position Data

"out vec3 vertexNormal;\n"									//Outgoing normals to fragment shader
"out vec3 vertexFragmentPos;\n"								//Outgoing color pixels to fragment shader
"out vec2 vertexTextureCoordinate;\n"						//Outgoing texture pixel coordinate to fragment shader 

"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"

"void main()\n"
"{\n"

//transforms vertices into clip coordinates
"   gl_Position = projection*view*model*vec4(aPos, 1.0f);\n"

//Gets fragment pixel position in world space only (excludes view and projection)
"	vertexFragmentPos = vec3(model * vec4(aPos, 1.0f));\n"

//Get normal vectors in world space only and exclude normal translation properties
"	vertexNormal = mat3(transpose(inverse(model))) * normal;\n"

"   vertexTextureCoordinate = textureCoordinate;\n"
"}\0";

#endif