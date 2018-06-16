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
string currentAnimP2 = "idle";
string nextAnimP2 = "idle";
float animationTransition = 0.0f;
float animationTransitionP2 = 0.0f;
float char_angle = radians(90.0f);
bool punching = false;
bool punchingP2 = false;
bool left_turn = false;
bool right_turn = false;
bool char_forward = false;
bool char_backward = false;
float char_angleP2 = radians(-90.0f);
bool left_turnP2 = false;
bool right_turnP2 = false;
bool char_forwardP2 = false;
bool char_backwardP2 = false;
bool char_left = false;
bool char_right = false;
bool char_leftP2 = false;
bool char_rightP2 = false;
vec3 char_pos = vec3(-3, 0, -5);
vec3 og_char_pos = vec3(-3, 0, -5);
vec3 char_posP2 = vec3(3, 0, -5);
vec3 og_char_posP2 = vec3(3, 0, -5);
vec3 char_direction = vec3(0, 0, 0);
vec3 char_directionP2 = vec3(0, 0, 0);
bool restart = false;
float frame = 0;
float frameP2 = 0;
float health = 100;
float healthP2 = 100;
vector<float> bone::cylinder;
vector<float> bone::cylinder_normals;
mat4 bone::headModel = mat4(1.0f);
ParticleGenerator* particles;
ParticleGenerator* particles2;

vec3 p1Head = vec3(0.05f, 0.4f, 0.0f);
vec3 p1Fireball = p1Head;
vec3 p2Head = vec3(-0.05f, 0.4f, 0.0f);
vec3 p2Fireball = p2Head;
vec3 p1FireballVel = vec3(0.0f);
vec3 p2FireballVel = vec3(0.0f);
bool fireball1 = false;
bool fireball2 = false;

double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime = glfwGetTime();
	double difference = actualtime - lasttime;
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
		pos = glm::vec3(0, -1, 0);
		rot = glm::vec3(3.14159 / 6, 0, 0);
	}
	glm::mat4 process(double ftime)
	{
		float speed = 0;
		if (w == 1)
		{
			speed = 10 * ftime;
		}
		else if (s == 1)
		{
			speed = -10 * ftime;
		}
		float yangle = 0;
		if (a == 1)
			yangle = -3 * ftime;
		else if (d == 1)
			yangle = 3 * ftime;
		rot.y += yangle;
		glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
		glm::vec4 dir = glm::vec4(0, 0, speed, 1);
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
	GLuint VertexArrayIDP2;
	// Data necessary to give our box to OpenGL
	GLuint VertexBufferID, VertexBufferIDimat, VertexNormDBox, VertexTexBox, IndexBufferIDBox, NormalBufferID;
	GLuint VertexBufferIDP2, VertexBufferIDimatP2, VertexNormDBoxP2, VertexTexBoxP2, IndexBufferIDBoxP2, NormalBufferIDP2;
	GLuint VAOFloor, VertexFloor, NormalFloor;

	//texture data
	GLuint Texture;
	GLuint Texture2;

	//animation matrices:
	mat4 animmat[200];
	int animmatsize = 0;

	mat4 animmatP2[200];
	int animmatsizeP2 = 0;
	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}


		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			//mycam.a = 1;
			char_forward = true;
			nextAnim = "walkback";
			currentAnim = "walkback";


		}
		if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
		{
			health = 100;
			healthP2 = 100;
			restart = true;

		}
		if (key == GLFW_KEY_ENTER && action == GLFW_RELEASE)
		{


		}
		if (key != GLFW_KEY_P)
		{
			punching = false;
		}
		if (key != GLFW_KEY_KP_3)
		{
			punchingP2 = false;
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
			currentAnim = "walk";
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			//mycam.d = 0;
			char_backward = false;
			nextAnim = "idle";
		}
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			//mycam.d = 0;
			char_left = true;
			nextAnim = "left"; // add strafe left
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			//mycam.d = 0;
			char_left = false;
			nextAnim = "idle"; // add strafe left
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			//mycam.d = 0;
			char_right = true;
			nextAnim = "right"; // add strafe left
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			//mycam.d = 0;
			char_right = false;
			nextAnim = "idle"; // add strafe right animation
		}
		if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		{
			char_rightP2 = true;
			nextAnim = "right";
		}
		if (key == GLFW_KEY_UP && action == GLFW_RELEASE)
		{
			char_rightP2 = false;
			nextAnim = "idle";
		}
		if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		{
			char_leftP2 = true;
			nextAnim = "left";
		}
		if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
		{
			char_leftP2 = false;
			nextAnim = "idle";
			//char_backward = false;
		}

		if (key == GLFW_KEY_O && action == GLFW_PRESS) //Walk
		{

			nextAnim = "magic";
			frame = 0;
			if (p1FireballVel.y < 0.0f)
			{

				p1Fireball = p1Head;
				p1FireballVel = vec3(0.0f);
			}

		}
		if (key == GLFW_KEY_O && action == GLFW_RELEASE) //Walk
		{

			nextAnim = "idle";

		}
		if (key == GLFW_KEY_P && action == GLFW_PRESS) //Run

		{
			mycam.rot.x += 0.1f;
			punching = true;
			currentAnim = "punch";
			nextAnim = "punch"; // add punch animation
			frame = 110;
		}

		if (key == GLFW_KEY_P && action == GLFW_RELEASE)
		{
			nextAnim = "idle";
		}

		if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		{
			//mycam.a = 1;
			char_forwardP2 = true;
			nextAnimP2 = "walk";

		}
		if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE) // need to switch directions
		{
			//mycam.a = 0;
			char_forwardP2 = false;
			nextAnimP2 = "idle";
		}
		if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		{
			//mycam.d = 1;
			char_backwardP2 = true;
			nextAnimP2 = "walkback";
		}
		if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE)
		{
			//mycam.d = 0;
			char_backwardP2 = false;
			nextAnimP2 = "idle";
		}


		if (key == GLFW_KEY_KP_2 && action == GLFW_PRESS) //Walk
		{
			nextAnimP2 = "magic";

			frameP2 = 0;
			if (p2FireballVel.y < 0.0f)
			{
				p2Fireball = p2Head;
				p2FireballVel = vec3(0.0f);
			}


		}
		if (key == GLFW_KEY_KP_3 && action == GLFW_PRESS) //Run
		{
			punchingP2 = true;
			currentAnimP2 = "punch";
			nextAnimP2 = "punch"; // add punch animation
			frameP2 = 110;
		}
		if (key == GLFW_KEY_KP_3 && action == GLFW_RELEASE) //Run
		{
			nextAnimP2 = "idle"; // add punch animation
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
	bone *rootP2 = NULL;
	int size_stick = 0;
	all_animations all_animation;
	all_animations all_animationP2;
	void initGeom(const std::string& resourceDirectory)
	{

		for (int ii = 0; ii < 200; ii++)
			animmat[ii] = mat4(1);

		//Load in animations
		readtobone("Idle.fbx", &all_animation, &root, "idle");
		readtobone("Walking.fbx", &all_animation, NULL, "walk");
		readtobone("Walking Backwards.fbx", &all_animation, NULL, "walkback"); // testing
		readtobone("Boxing.fbx", &all_animation, NULL, "punch");
		readtobone("Fireball.fbx", &all_animation, NULL, "magic");
		readtobone("Strafe Left.fbx", &all_animation, NULL, "left");
		readtobone("Strafe Right.fbx", &all_animation, NULL, "right");

		root->set_animations(&all_animation, animmat, animmatsize);

		readtobone("Idle.fbx", &all_animationP2, &rootP2, "idle");
		readtobone("Walking.fbx", &all_animationP2, NULL, "walk");
		readtobone("Walking Backwards.fbx", &all_animationP2, NULL, "walkback"); // testing
		readtobone("Boxing.fbx", &all_animationP2, NULL, "punch");
		readtobone("Fireball.fbx", &all_animationP2, NULL, "magic");
		readtobone("Strafe Left.fbx", &all_animationP2, NULL, "left");
		readtobone("Strafe Right.fbx", &all_animationP2, NULL, "right");

		rootP2->set_animations(&all_animation, animmatP2, animmatsizeP2);

		// Initialize mesh.
		shape = make_shared<Shape>();
		shape->loadMesh(resourceDirectory + "/skybox.obj");
		shape->resize();
		shape->init();

		plane = make_shared<Shape>();
		plane->loadMesh(resourceDirectory + "/floor.obj");
		plane->resize();
		plane->init();

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

		//Floor setup
		glGenVertexArrays(1, &VAOFloor);
		glBindVertexArray(VAOFloor);

		glGenBuffers(1, &VertexFloor);
		glBindBuffer(GL_ARRAY_BUFFER, VertexFloor); //Floor vertices
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*bone::cylinder.size(), bone::cylinder.data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &NormalFloor);
		glBindBuffer(GL_ARRAY_BUFFER, NormalFloor); //Floor normals
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*bone::cylinder_normals.size(), bone::cylinder_normals.data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);


		//generate the player 1 VAO
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



		/*

		for character 2?


		*/

		//generate the VAO
		glGenVertexArrays(1, &VertexArrayIDP2);

		//VAO block
		glBindVertexArray(VertexArrayIDP2);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferIDP2);
		//Vertex buffer setting
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferIDP2);
		vector<vec3> posP2, normP2;
		vector<unsigned int> imatP2;
		rootP2->write_to_VBOs(posP2, normP2, imatP2);
		size_stick = pos.size();
		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*posP2.size(), posP2.data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &VertexBufferIDimatP2);
		//ID buffer setting
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferIDimatP2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uint)*imatP2.size(), imatP2.data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 0, (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &NormalBufferIDP2);
		//Normal buffer setting
		glBindBuffer(GL_ARRAY_BUFFER, NormalBufferIDP2);
		glBufferData(GL_ARRAY_BUFFER, normP2.size() * sizeof(vec3), normP2.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(1);




		int width, height, channels;
		char filepath[1000];

		//texture 1
		string str = resourceDirectory + "/particle2.png";
		strcpy(filepath, str.c_str());
		unsigned char *data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//texture 2
		str = resourceDirectory + "/grid.jpg";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
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
		GLuint Tex1Location = glGetUniformLocation(pparticle->pid, "tex");
		GLuint Tex2Location = glGetUniformLocation(prog->pid, "tex2");
		// Then bind the uniform samplers to texture units:
		glUseProgram(pparticle->pid);
		glUniform1i(Tex1Location, 0);

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
		pfloor->setShaderNames(current + "/floor_vertex.glsl", resourceDirectory + "/floor_fragment.glsl");
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
		pparticle->addUniform("M");
		pparticle->addUniform("Z");
		pparticle->addUniform("offset");
		pparticle->addUniform("color");
		pparticle->addUniform("tex");
		pparticle->addAttribute("vertex");

		particles = new ParticleGenerator(pparticle);
		particles->color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
		particles2 = new ParticleGenerator(pparticle);
		particles2->color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
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
		static double totaltime_ms = 0;
		totaltime_ms += frametime*1000.0;
		static double totaltime_untilframe_ms = 0;
		totaltime_untilframe_ms += frametime*1000.0;

		//animation frame system
		int anim_step_width_ms = 8490 / 204;
		if (health < 0 || healthP2 < 0)
		{
			printf("Game over. Player ");
			if (health < 0)
			{
				printf("2 ");
			}
			else
			{
				printf("1 ");
			}
			printf("has won!");
			printf("\nWait a few seconds to play again\n");
			int i = 0;
			while (!restart)
			{
				if (i > 10)
				{
					restart = true;
					health = 100;
					healthP2 = 100;
					char_pos = vec3(-3, 0, -5);
					char_posP2 = vec3(3, 0, -5);
				}
				i++;
			}

		}
		if (punching && frame > 120)
		{
			if ((char_posP2.x - char_pos.x) < 0.6 && (abs(char_posP2.z - char_pos.z) < 0.3))
			{
				punching = false;
				healthP2 -= 5;
				printf("Health P1: %f.....................Health P2: %f\n", health, healthP2);
			}
		}
		if (punchingP2 && frameP2 > 120)
		{
			if ((char_posP2.x - char_pos.x) < 0.6 && (abs(char_posP2.z - char_pos.z) < 0.3))
			{
				punchingP2 = false;
				health -= 5;
				printf("Health P1: %f.....................Health P2: %f\n", health, healthP2);
			}
		}

		if ((char_posP2.x - p1Fireball.x) < -2.7f && (abs(char_posP2.z - char_pos.z) < 0.5))
		{

			p1Fireball = p1Head;
			p1FireballVel = vec3(0.0f);
			healthP2 -= 25;
			printf("Health P1: %f.....................Health P2: %f\n", health, healthP2);
		}

		if ((char_pos.x - p2Fireball.x) > 2.7f && (abs(char_posP2.z - char_pos.z) < 0.5))
		{

			p2Fireball = p2Head;
			p2FireballVel = vec3(0.0f);
			health -= 25;
			printf("Health P1: %f.....................Health P2: %f\n", health, healthP2);
		}
		if (nextAnim == "magic" && frame > 50)
		{
			p1FireballVel = vec3(.010f, -0.001f, 0.0f);
		}
		if (p1Fireball.x > 6.7)
		{
			p1Fireball = p1Head;
			p1FireballVel = vec3(0.0f);
		}
		if (p2Fireball.x < -6.7)
		{
			p2Fireball = p2Head;
			p2FireballVel = vec3(0.0f);
		}
		if (nextAnimP2 == "magic" && frameP2 > 50)
		{
			p2FireballVel = p2FireballVel = vec3(-.010f, -0.001f, 0.0f);
		}
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
		else if (char_left)
		{
			char_pos.z -= 0.035;

		}
		else if (char_right)
		{
			char_pos.z += 0.035;
		}


		//animation frame system for player 2
		static double totaltime_untilframe_msP2 = 0;
		totaltime_untilframe_msP2 += frametime * 1000.0;

		if (totaltime_untilframe_msP2 >= anim_step_width_ms) // maybe split this too lets see if it works
		{
			totaltime_untilframe_msP2 = 0;
			frameP2 += 1;
		}
		if (currentAnimP2 == nextAnimP2)
		{
			rootP2->play_animation(frameP2, currentAnimP2);	//name of current animation	
		}
		else
		{
			rootP2->play_animation_mix(frameP2, currentAnimP2, nextAnimP2);
			if (currentAnimP2 == nextAnimP2) frameP2 = 0;
		}

		/*if (left_turn)
		{
		char_angle += 0.1f;
		}
		else if (right_turn)
		{
		char_angle -= 0.1f;
		}*/ //potential dead code

		glm::mat4 char_rotateP2 = glm::rotate(glm::mat4(1.0f), char_angleP2, vec3(0, 1, 0));
		char_directionP2 = vec4(0, 0, 0.01f, 0) * char_rotateP2;
		if (char_forwardP2)
		{
			char_posP2.z -= char_directionP2.z;
			char_posP2.x -= char_directionP2.x;
		}
		else if (char_backwardP2)
		{
			char_posP2.z += char_directionP2.z;
			char_posP2.x += char_directionP2.x;
		}
		else if (char_leftP2)
		{
			char_posP2.z += 0.035;
		}
		else if (char_rightP2)
		{
			char_posP2.z -= 0.035;
		}

		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width / (float)height;
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
			P = glm::ortho(-1.0f, 1.0f, -1.0f / aspect, 1.0f / aspect, -2.0f, 100.0f);
		}
		// ...but we overwrite it (optional) with a perspective projection.
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width / (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones
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


		prog->bind();
		//send the matrices to the shaders
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("campos"), 1, &mycam.pos[0]);
		glBindVertexArray(VertexArrayIDP2);

		glm::mat4 TransZP2 = glm::translate(glm::mat4(1.0f), char_posP2);
		glm::mat4 SP2 = glm::scale(glm::mat4(1.0f), glm::vec3(0.005f, 0.005f, 0.005f));
		M = TransZP2 * char_rotateP2 * S;

		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniformMatrix4fv(prog->getUniform("Manim"), 200, GL_FALSE, &animmatP2[0][0][0]);
		glDrawArrays(GL_TRIANGLES, 0, size_stick);
		glBindVertexArray(1);
		prog->unbind();

		//Floor
		pfloor->bind();
		glUniformMatrix4fv(pfloor->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(pfloor->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniform3fv(pfloor->getUniform("campos"), 1, &mycam.pos[0]);
		//  glBindVertexArray(VAOFloor);

		glm::mat4 FloorTranslate = glm::translate(glm::mat4(1.0f), vec3(0.0f, -20.f, 0.0f));
		glm::mat4 FloorScale = glm::scale(glm::mat4(1.0f), glm::vec3(100.0f, 1.0f, 100.0f));
		glm::mat4 FloorRotate = glm::rotate(glm::mat4(1.0f), 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		M = FloorScale * FloorTranslate;

		glUniformMatrix4fv(pfloor->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		/* glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture2);
		glDisable(GL_DEPTH_TEST);*/
		plane->draw(pfloor, false);
		pfloor->unbind();

		//Particles

		particles->Update(vec2(og_char_pos.x, og_char_pos.y), 4);
		particles2->Update(vec2(og_char_posP2.x, og_char_posP2.y), 4);
		p1Fireball += p1FireballVel;
		p2Fireball += p2FireballVel;
		// Draw the particles using GLSL.
		pparticle->bind();
		glm::mat4 head = glm::translate(glm::mat4(1.0f), p1Fireball);
		M = V * TransZ * head * S;
		glUniformMatrix4fv(pparticle->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(pparticle->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform1f(pparticle->getUniform("Z"), char_pos.z);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glDisable(GL_DEPTH_TEST);
		particles->Draw();			//render!!!!!!!

		glm::mat4 head2 = glm::translate(glm::mat4(1.0f), p2Fireball);
		M = V * TransZP2 * head2 * S;
		glUniformMatrix4fv(pparticle->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform1f(pparticle->getUniform("Z"), char_posP2.z);
		particles2->Draw();			//render p2
		glEnable(GL_DEPTH_TEST);
		pparticle->unbind();
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
	while (!glfwWindowShouldClose(windowManager->getHandle()))
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