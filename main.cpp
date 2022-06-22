#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "gl_utils.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <string.h>
#include <time.h>
#define GL_LOG_FILE "gl.log"
#include <iostream>
#include "TileMap.h"
#include "DiamondView.h"
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <thread>
#include "src/LavaLogics.h"
#include "src/GameUtils.h"

using namespace std;

int g_gl_width = 800;
int g_gl_height = 800;
float xi = -1.0f;
float xf = 1.0f;
float yi = -1.0f;
float yf = 1.0f;
float w = xf - xi;
float h = yf - yi;
float tw, th, tw2, th2;
int tileSetCols = 2, tileSetRows = 1;
float tileW, tileW2;
float tileH, tileH2;
int playerX = 15, playerY = 15;

TilemapView *tview = new DiamondView();
TileMap *tmap = NULL;

GLFWwindow *g_window = NULL;

TileMap *readMap(char *filename)
{
	ifstream arq(filename);
	int w, h;
	arq >> w >> h;
	TileMap *tmap = new TileMap(w, h, 0);
	for (int r = 0; r < h; r++)
	{
		for (int c = 0; c < w; c++)
		{
			int tid;
			arq >> tid;
			tmap->setTile(c, h - r - 1, tid);
		}
	}
	arq.close();
	return tmap;
}

int loadTexture(unsigned int &texture, char *filename)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	GLfloat max_aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
	// set the maximum!
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);

	int width, height, nrChannels;

	unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (nrChannels == 4)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cout << "Failed to load texture" << endl;
	}
	stbi_image_free(data);
}

void onKeyPress(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		tview->computeTileWalking(playerX, playerY, DIRECTION_EAST);
	}
	else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		tview->computeTileWalking(playerX, playerY, DIRECTION_WEST);
	}
	else if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		tview->computeTileWalking(playerX, playerY, DIRECTION_NORTH);
	}
	else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		tview->computeTileWalking(playerX, playerY, DIRECTION_SOUTH);
	}
}

int main()
{
  	srand((unsigned)time( NULL ));
	startGame();
	restart_gl_log();
	// all the GLFW and GLEW start-up code is moved to here in gl_utils.cpp
	start_gl();
	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS);

	tmap = readMap("terrain1.tmap");
	tw = w / (float)tmap->getWidth();
	th = tw / 2.0f;
	tw2 = th;
	th2 = th / 2.0f;
	tileW = 1.0f / (float)tileSetCols;
	tileW2 = tileW / 2.0f;
	tileH = 1.0f / (float)tileSetRows;
	tileH2 = tileH / 2.0f;

	GLuint tid;
	loadTexture(tid, "terrain.png");

	tmap->setTid(tid);

	// LOAD TEXTURES

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
			// positions   // texture coords
			xi, yi + th2, 0.0f, tileH2,				// left
			xi + tw2, yi, tileW2, 0.0f,				// bottom
			xi + tw, yi + th2, tileW, tileH2, // right
			xi + tw2, yi + th, tileW2, tileH, // top
	};
	unsigned int indices[] = {
			0, 1, 3, // first triangle
			3, 1, 2	 // second triangle
	};

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	char vertex_shader[1024 * 256];
	char fragment_shader[1024 * 256];
	parse_file_into_str("_geral_vs.glsl", vertex_shader, 1024 * 256);
	parse_file_into_str("_geral_fs.glsl", fragment_shader, 1024 * 256);

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	const GLchar *p = (const GLchar *)vertex_shader;
	glShaderSource(vs, 1, &p, NULL);
	glCompileShader(vs);

	// check for compile errors
	int params = -1;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &params);
	if (GL_TRUE != params)
	{
		fprintf(stderr, "ERROR: GL shader index %i did not compile\n", vs);
		print_shader_info_log(vs);
		return 1; // or exit or something
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	p = (const GLchar *)fragment_shader;
	glShaderSource(fs, 1, &p, NULL);
	glCompileShader(fs);

	// check for compile errors
	glGetShaderiv(fs, GL_COMPILE_STATUS, &params);
	if (GL_TRUE != params)
	{
		fprintf(stderr, "ERROR: GL shader index %i did not compile\n", fs);
		print_shader_info_log(fs);
		return 1; // or exit or something
	}

	GLuint shader_programme = glCreateProgram();
	glAttachShader(shader_programme, fs);
	glAttachShader(shader_programme, vs);
	glLinkProgram(shader_programme);

	glGetProgramiv(shader_programme, GL_LINK_STATUS, &params);
	if (GL_TRUE != params)
	{
		fprintf(stderr, "ERROR: could not link shader programme GL index %i\n",
						shader_programme);
		// 		print_programme_info_log( shader_programme );
		return false;
	}

	float previous = glfwGetTime();

	for (int r = 0; r < tmap->getHeight(); r++)
	{
		for (int c = 0; c < tmap->getWidth(); c++)
		{
			unsigned char t_id = tmap->getTile(c, r);
		}
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	/*
	 * Start a new thread to spread the lava while the
	 * main thread is rendering the terrain.
	 **/
	thread([=]()
							{ startSpreadingLava(tmap); })
			.detach();

	// glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(g_window))
	{
		if (getGameStatus(tmap, playerX, playerY) == FINISHED)
		{
			cout << "\nJogo finalizado.\nVocê sobreviveu por " << getElapsedTimeInSeconds() << " segundos!\n"
					 << endl;
			glfwSetWindowShouldClose(g_window, 1);
		}

		_update_fps_counter(g_window);
		double current_seconds = glfwGetTime();

		// wipe the drawing surface clear
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// glClear(GL_COLOR_BUFFER_BIT);

		glViewport(0, 0, g_gl_width, g_gl_height);

		glUseProgram(shader_programme);

		glBindVertexArray(VAO);
		float x, y;
		int r = 0, c = 0;
		for (int r = 0; r < tmap->getHeight(); r++)
		{
			for (int c = 0; c < tmap->getWidth(); c++)
			{
				int t_id = (int)tmap->getTile(c, r);
				int u = t_id % tileSetCols;
				int v = t_id / tileSetCols;

				tview->computeDrawPosition(c, r, tw, th, x, y);

				glUniform1f(glGetUniformLocation(shader_programme, "offsetx"), u * tileW);
				glUniform1f(glGetUniformLocation(shader_programme, "offsety"), v * tileH);
				glUniform1f(glGetUniformLocation(shader_programme, "tx"), x);
				glUniform1f(glGetUniformLocation(shader_programme, "ty"), y + 1.0);
				glUniform1f(glGetUniformLocation(shader_programme, "layer_z"), tmap->getZ());
				glUniform1f(glGetUniformLocation(shader_programme, "weight"), (c == playerX) && (r == playerY) ? 0.5 : 0.0);

				// bind Texture
				// glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, tmap->getTileSet());
				glUniform1i(glGetUniformLocation(shader_programme, "sprite"), 0);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
		}

		glfwPollEvents();
		if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(g_window, 1);
		}

		glfwSetKeyCallback(g_window, onKeyPress);
		// put the stuff we've been drawing onto the display
		glfwSwapBuffers(g_window);
	}

	// close GL context and any other GLFW resources
	glfwTerminate();
	delete tmap;
	return 0;
}
