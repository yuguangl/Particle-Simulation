/*
#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
using namespace std;


const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";

void processInput(GLFWwindow* window, GLfloat(&vertices)[18]) {
	GLfloat incr = 0.01f;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		for(int i : vertices){
			//vertices[i] += incr;
		}
	}
}
void framebuffer_size_callback(GLFWwindow* window, int w, int h) {
	glViewport(0, 0, w, h);
}

int createSquare() {


	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLfloat vertices[18] = {
		0.5f, 0.5f, 0.0f, // top right
		0.5f, -0.5f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f, // bottom left
		-0.5f, 0.5f, 0.0f
	};

	GLuint  indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	GLFWwindow* window = glfwCreateWindow(1000, 1000, "Square", NULL, NULL);

	if (window == NULL) {
		cerr << "ERROR: FAILED LOADING WINDOW" << endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();

	glViewport(0, 0, 1000, 1000);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//shaders
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cout << "ERROR: SHADER VRTX COMPILE FAILURE\n" << infoLog << endl;
	}
	

GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
glCompileShader(fragmentShader);

//wrap shaders
GLuint shaderProgram = glCreateProgram();
glAttachShader(shaderProgram, vertexShader);
glAttachShader(shaderProgram, fragmentShader);

glLinkProgram(shaderProgram);


glDeleteShader(vertexShader);
glDeleteShader(fragmentShader);


GLuint VAO, VBO, EBO;
glGenVertexArrays(1, &VAO);
glBindVertexArray(VAO);

glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
/*vertex attrib pointer takes data from currently bound VBO(or multiple)*/
/*
glEnableVertexAttribArray(0);//0 is the attrib location

glGenBuffers(1, &EBO);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);






//glUseProgram(shaderProgram); //???

//glBindBuffer(GL_ARRAY_BUFFER, 0);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//glBindVertexArray(0);


glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
glClear(GL_COLOR_BUFFER_BIT);
glfwSwapBuffers(window);



glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
while (!glfwWindowShouldClose(window)) {
	processInput(window, vertices);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgram);
	//glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glfwSwapBuffers(window);
	glfwPollEvents();
}

glDeleteVertexArrays(1, &VAO);
glDeleteBuffers(1, &VBO);
glDeleteProgram(shaderProgram);

glfwDestroyWindow(window);
glfwTerminate();


return 0;
}

int main() {
	createSquare();
	return 0;

}
*/