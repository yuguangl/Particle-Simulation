#include "Shader.h"




Shader::Shader(const char* vertexFile, const char* fragmentFile) {
	string vertexShaderSource = get_file_contents(vertexFile);
	string fragmentShaderSource = get_file_contents(fragmentFile);

	// Convert the shader source strings into character arrays
	const char* vertexSource = vertexShaderSource.c_str();
	const char* fragmentSource = fragmentShaderSource.c_str();


	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); 
	glShaderSource(vertexShader, 1, &vertexSource, NULL); 
	glCompileShader(vertexShader); 

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); 
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL); 
	glCompileShader(fragmentShader); 

	shaderID = glCreateProgram(); 
	glAttachShader(shaderID, vertexShader);
	glAttachShader(shaderID, fragmentShader);

	glLinkProgram(shaderID);
	glDeleteShader(vertexShader); 
	glDeleteShader(fragmentShader);

}

string Shader::get_file_contents(const char* filename) {
	//Code: https://github.com/VictorGordan/opengl-tutorials/blob/main/YoutubeOpenGL%207%20-%20Going%203D/shaderClass.cpp
	//cuz I dont understand how reading this textfile works
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);


}

void Shader::useShader() {
	glUseProgram(shaderID);
}

void Shader::Delete() {
	glDeleteProgram(shaderID);
}