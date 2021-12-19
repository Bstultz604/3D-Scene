#ifndef FRAGSHADE_H
#define FRAGSHADE_H

const char* fragmentShaderSource = "#version 440 core\n"

"in vec3 vertexNormal;\n"				// For incoming normals
"in vec3 vertexFragmentPos;\n"			// For incoming fragment position
"in vec2 vertexTextureCoordinate;\n"

"out vec4 FragColor;\n"

"uniform vec3 objectColor;\n"
"uniform vec3 lightColor;\n"
"uniform vec3 lightPos;\n"
"uniform vec3 viewPosition;\n"
"uniform sampler2D Texture;\n"
"uniform vec2 uvScale;\n"
"uniform float specIntensity;\n"

"void main()\n"
"{\n"
/*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

//Calculate Ambient lighting*/
"float ambientStrength = 0.1f;\n"									// Set ambient or global lighting strength
"vec3 ambient = ambientStrength * lightColor;\n"					// Generate ambient light color

//Calculate Diffuse lighting*/
"vec3 norm = normalize(vertexNormal);\n"							// Normalize vectors to 1 unit
"vec3 lightDirection = normalize(lightPos - vertexFragmentPos);\n"	// Calculate distance (light direction) between light source and fragments/pixels on cube
"float impact = max(dot(norm, lightDirection), 0.0);\n"				// Calculate diffuse impact by generating dot product of normal and light
"vec3 diffuse = impact * lightColor;\n"								// Generate diffuse light color

//Calculate Specular lighting*/
"float specularIntensity = specIntensity;\n"									// Set specular light strength
"float highlightSize = 16.0f;\n"									// Set specular highlight size
"vec3 viewDir = normalize(viewPosition - vertexFragmentPos);\n"		// Calculate view direction
"vec3 reflectDir = reflect(-lightDirection, norm);\n"				// Calculate reflection vector

//Calculate specular component
"float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);\n"
"vec3 specular = specularIntensity * specularComponent * lightColor;\n"

// Texture holds the color to be used for all three components
"vec4 textureColor = texture(Texture, vertexTextureCoordinate * uvScale);\n"

// Calculate phong result
"vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;\n"

"FragColor = vec4(phong, 1.0);\n"								// Send lighting results to GPU
"}\n\0";

#endif