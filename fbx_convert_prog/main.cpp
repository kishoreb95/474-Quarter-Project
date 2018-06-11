/*
CPE/CSC 474 Lab base code Eckhardt/Dahl
based on CPE/CSC 471 Lab base code Wood/Dunn/Eckhardt
*/
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

#include "WindowManager.h"
#include "Shape.h"
#include "Line.h"
#include "bone.h"
#include "tiny_obj_loader.h"
#include "particle.h"
using namespace std;
using namespace glm;
shared_ptr<Shape> shape;
shared_ptr<Shape> plane;
string currentAnim = "idle";
string nextAnim = "idle";
float animationTransition = 0.0f;

float char_angle = radians(90.0f);
bool left_turn = false;
bool right_turn = false;
bool char_forward = false;
bool char_backward = false;

vec3 char_pos = vec3(0, 0, -8);
vec3 char_direction = vec3(0, 0, 0);

vector<float> bone::cylinder;
vector<float> bone::cylinder_normals;
shared_ptr<ParticleGenerator> particles;

double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime =glfwGetTime();
	double difference = actualtime- lasttime;
	lasttime = actualtime;
	return difference;
}
class camera
{
public:
	glm::vec3 pos, rot;
	int w, a, s, d;
	camera()
	{
		w = a = s = d = 0;
		pos = rot = glm::vec3(0, 0, 0);
	}
	glm::mat4 process(double ftime)
	{
		float speed = 0;
		if (w == 1)
		{
			speed = 10*ftime;
		}
		else if (s == 1)
		{
			speed = -10*ftime;
		}
		float yangle=0;
		if (a == 1)
			yangle = -3*ftime;
		else if(d==1)
			yangle = 3*ftime;
		rot.y += yangle;
		glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
		glm::vec4 dir = glm::vec4(0, 0, speed,1);
		dir = dir*R;
		pos += glm::vec3(dir.x, dir.y, dir.z);
		glm::mat4 T = glm::translate(glm::mat4(1), pos);
		return R*T;
	}
};

camera mycam;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog, psky, pparticle, pfloor;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our box to OpenGL
	GLuint VertexBufferID, VertexBufferIDimat, VertexNormDBox, VertexTexBox, IndexBufferIDBox, NormalBufferID;

	//texture data
	GLuint Texture;
	GLuint Texture2;
	
	//animation matrices:
	mat4 animmat[200];
	int animmatsize=0;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			mycam.w = 1;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			mycam.w = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			mycam.s = 1;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			mycam.s = 0;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			//mycam.a = 1;
         char_forward = true;
         nextAnim = "walkback";
         
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE) // need to switch directions
		{
			//mycam.a = 0;
         char_forward = false;
         nextAnim = "idle";
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			//mycam.d = 1;
         char_backward = true;
         nextAnim = "walk";
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			//mycam.d = 0;
         char_backward = false;
         nextAnim = "idle";
		}
		if (key == GLFW_KEY_UP && action == GLFW_PRESS) //move character forward
		{
			char_forward = true;
		}
		if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) //move character backward
		{
			char_backward = true;
		}
		if (key == GLFW_KEY_UP && action == GLFW_RELEASE) 
		{
			char_forward = false;
		}
		if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) 
		{
			char_backward = false;
		}

		if (key == GLFW_KEY_O && action == GLFW_PRESS) //Walk
		{
			nextAnim = "idle";
		}
		if (key == GLFW_KEY_P && action == GLFW_PRESS) //Run
		{
			nextAnim = "punch"; // add punch animation
		}


		if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) //Character rotates counterclockwise
		{
			left_turn = true;
		}
		if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE)
		{
			left_turn = false;
		}
		if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) //Character rotates clockwise
		{
			right_turn = true;
		}
		if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE)
		{
			right_turn = false;
		}
	}

	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		
	}

	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}

	/*Note that any gl calls must always happen after a GL state is initialized */
	bone *root = NULL;
	int size_stick = 0;
	all_animations all_animation;
	void initGeom(const std::string& resourceDirectory)
	{

		for (int ii = 0; ii < 200; ii++)
			animmat[ii] = mat4(1);
		
		//Load in animations
		readtobone("Idle.fbx",&all_animation,&root, "idle");
		readtobone("Walking.fbx", &all_animation, NULL, "walk");
      readtobone("Walking Backwards.fbx", &all_animation, NULL, "walkback"); // testing
      readtobone("Boxing.fbx", &all_animation, NULL, "punch");
		
      root->set_animations(&all_animation, animmat, animmatsize);
			
		// Initialize mesh.
		shape = make_shared<Shape>();
		shape->loadMesh(resourceDirectory + "/skybox.obj");
		shape->resize();
		shape->init();

		vector<tinyobj::shape_t> shapes;
		vector<tinyobj::material_t> objMaterials;
		string errStr;
		bool loaded = false;
		string cylinder_load = resourceDirectory + "/cylinder.obj";
		loaded = tinyobj::LoadObj(shapes, objMaterials, errStr, cylinder_load.data());
		if (shapes.size() <= 0)
		{
			cout << "couldn't load" << endl;
			exit(1);
		}
		bone::cylinder = shapes[0].mesh.positions;
		bone::cylinder_normals = shapes[0].mesh.normals;

		//generate the VAO
		glGenVertexArrays(1, &VertexArrayID);

		//VAO block
		glBindVertexArray(VertexArrayID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferID);
		//Vertex buffer setting
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
		vector<vec3> pos, norm;
		vector<unsigned int> imat;
		root->write_to_VBOs(pos, norm, imat);
		size_stick = pos.size();
		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*pos.size(), pos.data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &VertexBufferIDimat);
		//ID buffer setting
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferIDimat);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uint)*imat.size(), imat.data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 0, (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &NormalBufferID);
		//Normal buffer setting
		glBindBuffer(GL_ARRAY_BUFFER, NormalBufferID);
		glBufferData(GL_ARRAY_BUFFER, norm.size() * sizeof(vec3), norm.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

		int width, height, channels;
		char filepath[1000];

		//texture 2
		string str = resourceDirectory + "/grid.jpg";
		strcpy(filepath, str.c_str());
		unsigned char *data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture2);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//[TWOTEXTURES]
		//set the 2 textures to the correct samplers in the fragment shader:
		GLuint Tex2Location = glGetUniformLocation(prog->pid, "tex2");
		// Then bind the uniform samplers to texture units:
		glUseProgram(prog->pid);
		glUniform1i(Tex2Location, 1);

	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();
		const string current = ".";
		// Set background color.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
		//glDisable(GL_DEPTH_TEST);
		// Initialize the GLSL program.
		prog = std::make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
		if (!prog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("Manim");
		prog->addUniform("campos");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertimat");
		prog->addAttribute("vertNor");


		psky = std::make_shared<Program>();
		psky->setVerbose(false);
		psky->setShaderNames(resourceDirectory + "/skyvertex.glsl", resourceDirectory + "/skyfrag.glsl");
		if (!psky->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		psky->addUniform("P");
		psky->addUniform("V");
		psky->addUniform("M");
		psky->addAttribute("vertPos");
		psky->addAttribute("vertTex");
		psky->addAttribute("vertNor");


		pfloor = std::make_shared<Program>();
		pfloor->setVerbose(true);
		pfloor->setShaderNames(current + "/floor_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
		if (!pfloor->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		pfloor->addUniform("P");
		pfloor->addUniform("V");
		pfloor->addUniform("M");
		pfloor->addUniform("campos");
		pfloor->addAttribute("vertPos");
		pfloor->addAttribute("vertNor");

		pparticle = std::make_shared<Program>();
		pparticle->setVerbose(true);
		pparticle->setShaderNames(current + "/particle_vertex.glsl", current + "/particle_fragment.glsl");
		if (!pparticle->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		pparticle->addUniform("P");
		pparticle->addUniform("offset");
		pparticle->addUniform("color");
		pparticle->addUniform("sprite");
		pparticle->addAttribute("vertex");
	}


	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/

	
	void render()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		double frametime = get_last_elapsed_time();
		static double totaltime_ms=0;
		totaltime_ms += frametime*1000.0;
		static double totaltime_untilframe_ms = 0;
		totaltime_untilframe_ms += frametime*1000.0;

		//animation frame system
		int anim_step_width_ms = 8490 / 204;
		static float frame = 0;
		if (totaltime_untilframe_ms >= anim_step_width_ms)
		{
			totaltime_untilframe_ms = 0;
			frame += 1;
		}
      if (currentAnim == nextAnim)
      {
         root->play_animation(frame, currentAnim);	//name of current animation	
      }
		else
		{
			root->play_animation_mix(frame, currentAnim, nextAnim);
			if (currentAnim == nextAnim) frame = 0;
		}

		if (left_turn)
		{
			char_angle += 0.1f;
		}
		else if (right_turn)
		{
			char_angle -= 0.1f;
		}

		glm::mat4 char_rotate = glm::rotate(glm::mat4(1.0f), char_angle, vec3(0, 1, 0));
		char_direction = vec4(0, 0, 0.01f, 0) * char_rotate;
		if (char_forward)
		{
			char_pos.z += char_direction.z;
			char_pos.x += char_direction.x;
		}
		else if (char_backward)
		{
			char_pos.z -= char_direction.z;
			char_pos.x -= char_direction.x;
		}

		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now
		
		glm::mat4 V, M, P; //View, Model and Perspective matrix
		V = mycam.process(frametime);
		M = glm::mat4(1);
		// Apply orthographic projection....
		P = glm::ortho(-1 * aspect, 1 * aspect, -1.0f, 1.0f, -2.0f, 100.0f);		
		if (width < height)
			{
			P = glm::ortho(-1.0f, 1.0f, -1.0f / aspect,  1.0f / aspect, -2.0f, 100.0f);
			}
		// ...but we overwrite it (optional) with a perspective projection.
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width/ (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones
		float sangle = 3.1415926 / 2.;
		glm::vec3 camp = -mycam.pos;
		glm::mat4 TransSky = glm::translate(glm::mat4(1.0f), camp);
		glm::mat4 SSky = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 0.8f));

		M = TransSky * SSky;

		// Draw the sky using GLSL.
		psky->bind();		
		glUniformMatrix4fv(psky->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(psky->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(psky->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture2);
		glDisable(GL_DEPTH_TEST);
		shape->draw(psky, false);			//render!!!!!!!
		glEnable(GL_DEPTH_TEST);	
		psky->unbind();
		
		prog->bind();
		//send the matrices to the shaders
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("campos"), 1, &mycam.pos[0]);	
		glBindVertexArray(VertexArrayID);

		glm::mat4 TransZ = glm::translate(glm::mat4(1.0f), char_pos);
		glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.005f, 0.005f, 0.005f));
		M = TransZ * char_rotate * S;
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniformMatrix4fv(prog->getUniform("Manim"), 200, GL_FALSE, &animmat[0][0][0]);
		glDrawArrays(GL_TRIANGLES, 0, size_stick);
		glBindVertexArray(0);		
		prog->unbind();
	}

};
//******************************************************************************************
int main(int argc, char **argv)
{
	std::string resourceDir = "../resources"; // Where the resources are loaded from
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager * windowManager = new WindowManager();
	windowManager->init(1920, 1080);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
	// Initialize scene.
	application->init(resourceDir);
	application->initGeom(resourceDir);

	// Loop until the user closes the window.
	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
