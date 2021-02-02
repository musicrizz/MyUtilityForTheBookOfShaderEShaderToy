//============================================================================
// Name        : ShadersBookExample.cpp
// Author      : Giovanni Grandinetti
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "../context_util/util.h"
#include <filesystem>
#include <set>

using namespace std;
namespace fs = std::filesystem;

void keyboard(GLFWwindow* window, int key, int scancose, int action, int mods);
void mouseCallback(GLFWwindow* window, double y, double x);

void updateViewPort();
void updateTime();

int viewport_w, viewport_h;
float viewport_aspect;

enum VAO {V, VAOS_NUM};
unsigned int vaos[VAOS_NUM];

enum BUFFERS {B_VERTEX, UNIFORM, BUFFERS_NUM};
unsigned int buffers[BUFFERS_NUM];

static const char *CONTEXT="CONTEX_1",
				  *FPS_TIME="FPS_TIME";

bool flag_update_viewport=true;

int programs_count = 0;
int current_program = 0;

int main(int argc, char **argv) {

	if (!glfwInit()) {
		fprintf(stderr, "GLEW INTI ERROR");
		exit(EXIT_FAILURE);
	}

	glfwSetErrorCallback([](int error, const char *description) {
		fprintf(stderr, "GLFW_ERROR: %u :  %s ", error, description);
	});

	GLFWmonitor *primary = glfwGetPrimaryMonitor();
	const GLFWvidmode *mode = glfwGetVideoMode(primary);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 0);

#if DEVELOPEMENT
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	OpenGLContext::createContex(CONTEXT, 600, 600, NULL, true, true, false);
	OpenGLContext::makecurrent(CONTEXT);

	OpenGLContext::setReshape(CONTEXT, [](GLFWwindow* window, int width, int height){
		viewport_w = width;
		viewport_h = height;
		viewport_aspect = float(height) / float(width);
		flag_update_viewport = true;
	});
	OpenGLContext::setMouseCursorPos(mouseCallback);
	OpenGLContext::setKeyboard(CONTEXT, keyboard);

	set<fs::path> sorted_by_name;
	for (const auto &f : fs::directory_iterator("shaders/fragments")) {
		if (f.is_regular_file()) {
			sorted_by_name.insert(f.path());
		}
	}
	string common_vertex_shader = "shaders/common_vertex.glsl";
	for(const auto &p : sorted_by_name)  {
		try{
			programs_count++;
			ShaderMap::createProgram(to_string(programs_count), common_vertex_shader.c_str(), p.c_str());
			cout<<OpenGLerror::check("creation program "+p.string());
		}catch (ShaderException &e) {
			cout<< "Errore creazione programma : "<< p.string() << ", Error : "<< e.what()<<endl;
			programs_count--;
		}
	}
	if (programs_count <= 0)  {
		cerr<<"ERRORE NESSUN PROGRAMMA CARICATO"<<endl;
		exit(-1);
	}
	current_program = programs_count;

	ShaderMap::useProgram(to_string(current_program));

	glGenVertexArrays(VAOS_NUM, vaos);
	glGenBuffers(BUFFERS_NUM, buffers);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[B_VERTEX]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4)*4, NULL, GL_STATIC_DRAW);
	glm::vec4* v = (glm::vec4*)glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
		*v++ = glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f);
		*v++ = glm::vec4( 1.0f, -1.0f, 0.0f, 1.0f);
		*v++ = glm::vec4( 1.0f,  1.0f, 0.0f, 1.0f);
		*v   = glm::vec4(-1.0f,  1.0f, 0.0f, 1.0f);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	cout<<OpenGLerror::check("creation buffer vertex");

	unsigned int vertex_loc = 1;//this location is defined in the shader
	cout<<"POSITION : "<<vertex_loc<<endl;
	glBindVertexArray(vaos[V]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[B_VERTEX]);
		glVertexAttribPointer(vertex_loc, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), reinterpret_cast<void*>(0));
		glEnableVertexAttribArray(vertex_loc);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	cout<<OpenGLerror::check("creation buffer VAO");

	unsigned int uniform_binding_point = 2;
	ShaderMap::bindingUniformBlocks("CommonUniform", uniform_binding_point);
	glBindBufferBase(GL_UNIFORM_BUFFER, uniform_binding_point, buffers[UNIFORM]);

	glBindBuffer(GL_UNIFORM_BUFFER, buffers[UNIFORM]);
		glBufferData(GL_UNIFORM_BUFFER, 20, NULL, GL_DYNAMIC_DRAW); // allocate 20 bytes of memory
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	cout<<OpenGLerror::check("CREAZIOEN GL_UNIFORM_BUFFER : ")<<endl;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0, 1.0);
	glDepthMask(true);
	glClearDepth(1.0f);
	glEnable(GL_LINE_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glColorMask(true, true, true, true);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwGetFramebufferSize(OpenGLContext::getCurrent(), &viewport_w, &viewport_h);
	viewport_aspect = float(viewport_h) / float(viewport_w);
	flag_update_viewport = true;

	glfwSwapInterval(1);

	cout<<OpenGLerror::check("Finish setUp Opengl");

	glBindVertexArray(vaos[V]);

	while (!glfwWindowShouldClose(OpenGLContext::getCurrent())) {

		if (TempoMap::getElapsedMill(FPS_TIME) >= 22) {

			ShaderMap::useProgram(to_string(current_program));

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			updateViewPort();

			updateTime();

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			OpenGLContext::swapBuffers();

			TempoMap::updateStart(FPS_TIME);

			ShaderMap::useProgram(0);
		}

		glfwPollEvents(); //  It MUST be in the main thread

	}

	OpenGLContext::destroyAll();

	glfwTerminate();

	fprintf(stdout, " MAIN FINISH .");
	exit(EXIT_SUCCESS);

}

void updateViewPort() {
	if (flag_update_viewport) {
		glViewport(0, 0, viewport_w, viewport_h);
		glBindBuffer(GL_UNIFORM_BUFFER, buffers[UNIFORM]);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, 8, glm::value_ptr(glm::ivec2(viewport_w, viewport_h)));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		flag_update_viewport = false;
	}
}

void updateTime()   {
	float time = float(glfwGetTime());
	glBindBuffer(GL_UNIFORM_BUFFER, buffers[UNIFORM]);
		glBufferSubData(GL_UNIFORM_BUFFER, 16, 4, reinterpret_cast<void*>(&time) );
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void mouseCallback(GLFWwindow* window, double x, double y)  {
	glBindBuffer(GL_UNIFORM_BUFFER, buffers[UNIFORM]);
		glBufferSubData(GL_UNIFORM_BUFFER, 8, 8, glm::value_ptr(glm::vec2(float(x)/float(viewport_w), float(viewport_h-y)/float(viewport_h))));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void keyboard(GLFWwindow* window, int key, int scancose, int action, int mods)  {
	switch (key) {
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);

		}
		break;
	case GLFW_KEY_KP_ADD:
		if (action == GLFW_PRESS) {
			if(current_program < programs_count)  {
				current_program++;
			}else{
				current_program = 1;
			}
		}
		break;
	case GLFW_KEY_KP_SUBTRACT:
		if (action == GLFW_PRESS) {
			if(current_program > 1)  {
				current_program--;
			}else{
				current_program = programs_count;
			}
		}
		break;
	}
}
