#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<iostream>
#include<vector>d

using namespace std;



class Shader {
public: 
	GLuint shaderID;
	//outputs contents of a text file as a string
	string get_file_contents(const char* filename);
	Shader(const char* vertexFile, const char* fragmentFile);


	void useShader();
	void Delete();
};



#endif
