#include "SceneManager.h"

//static controllers for mouse and keyboard
static bool keys[1024];
static bool resized;
static GLuint width, height;
unsigned int texnave[3];

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::initialize(GLuint w, GLuint h)
{
	width = w;
	height = h;

	// GLFW - GLEW - OPENGL general setup -- TODO: config file
	initializeGraphics();

}

void SceneManager::initializeGraphics()
{
	setX = 400;
	setY = 200;

	objsetX[0] = float(rand() % int(width - 10.0f));
	objsetX[1] = float(rand() % int(width - 10.0f));
	objsetX[2] = float(rand() % int(width - 10.0f));

	objsetY[0] = 700;
	objsetY[1] = 700;
	objsetY[2] = 700;

	velocity[0] = 0.4;
	velocity[1] = 0.3;
	velocity[2] = 0.5;

	// Init GLFW
	glfwInit();

	// Create a GLFWwindow object that we can use for GLFW's functions
	window = glfwCreateWindow(width, height, "Hello Sprites", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	//Setando a callback de redimensionamento da janela
	glfwSetWindowSizeCallback(window, resize);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Build and compile our shader program
	addShader("../shaders/transformations.vs", "../shaders/transformations.frag");

	//setup the scene -- LEMBRANDO QUE A DESCRIÇÃO DE UMA CENA PODE VIR DE ARQUIVO(S) DE 
	// CONFIGURAÇÃO
	setupScene();

	resized = true; //para entrar no setup da câmera na 1a vez

}

void SceneManager::addShader(string vFilename, string fFilename)
{
	shader = new Shader(vFilename.c_str(), fFilename.c_str());
}


void SceneManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void SceneManager::resize(GLFWwindow* window, int w, int h)
{
	width = w;
	height = h;
	resized = true;

	// Define the viewport dimensions
	glViewport(0, 0, width, height);
}

void SceneManager::update()
{
	if (keys[GLFW_KEY_ESCAPE])
		glfwSetWindowShouldClose(window, GL_TRUE);


	//AQUI aplicaremos as transformações nos sprites
	if (keys[GLFW_KEY_LEFT])
	{
		if ((setX + 0.01) > 0.0)
		{
			setX -= 0.5f;
			setY += 0.0f;
			objects[1]->setTexture(texnave[2]);
			objects[1]->setPosition(glm::vec3(setX, setY, 0.0));

			if (TestCollision()) {
				finish();
				keys[GLFW_KEY_LEFT] = false;
				std::cout << "Ops! Parece que voce bateu sua nave" << std::endl;
			}
		}
	}

	if (keys[GLFW_KEY_RIGHT])
	{
		if ((setX + 0.01) < width)
		{
			setX += 0.5f;
			setY += 0.0f;
			objects[1]->setTexture(texnave[1]);
			objects[1]->setPosition(glm::vec3(setX, setY, 0.0));

			if (TestCollision()) {
				finish();
				keys[GLFW_KEY_RIGHT] = false;
				std::cout << "Ops! Parece que voce bateu sua nave" << std::endl;
			}
		}
	}

	//gera obstáculo
	for (int i = 0; i <= 2; i++)
	{
		objsetY[i] -= velocity[i];

		if (objsetY[i] <= -10.0f)
		{
			objsetX[i] = float(rand() % int(width - 10.0f));
			objsetY[i] = 800;
			velocity[i] += 0.05;
		}
		objects[i + 2]->setPosition(glm::vec3(objsetX[i], objsetY[i], 0.0));
	}

	//altera o angulo do segundo objeto
	objects[2]->setAngle((float)glfwGetTime());
	objects[3]->setAngle((float)glfwGetTime());
	objects[4]->setAngle((float)glfwGetTime());
}

void SceneManager::render()
{
	// Clear the colorbuffer
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (resized) //se houve redimensionamento na janela, redefine a projection matrix
	{
		setupCamera2D();
		resized = false;
	}

	//atualiza e desenha os Sprites

	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->update();
		objects[i]->draw();
	}

}

void SceneManager::run()
{
	//GAME LOOP
	while (!glfwWindowShouldClose(window))
	{

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		//Update method(s)
		update();

		//Render scene
		render();

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
}

void SceneManager::finish()
{
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
}


void SceneManager::setupScene()
{
	//Criação dos Sprites iniciais -- pode-se fazer métodos de criação posteriormen

	//Mínimo: posicao e escala e ponteiro para o shader
	Sprite* obj = new Sprite;
	obj->setPosition(glm::vec3(200.0f, 300.0f, 0.0));
	obj->setDimension(glm::vec3(1280.0f, 720.0f, 1.0f)); //note que depois podemos reescalar conforme tamanho da sprite
	obj->setShader(shader);
	objects.push_back(obj); //adiciona o primeiro obj BACKGROUND

	obj = new Sprite;
	obj->setPosition(glm::vec3(setX, setY, 0.0));
	obj->setDimension(glm::vec3(64.0f, 64.0f, 1.0f));
	obj->setShader(shader);
	objects.push_back(obj); //adiciona o segundo obj NAVE

	obj = new Sprite;
	obj->setPosition(glm::vec3(objsetX[0], objsetY[0], 0.0));
	obj->setDimension(glm::vec3(79.5f, 79.5f, 1.0f));
	obj->setShader(shader);
	objects.push_back(obj); //adiciona o terceiro obj ASTEROID

	obj = new Sprite;
	obj->setPosition(glm::vec3(objsetX[1], objsetY[1], 0.0));
	obj->setDimension(glm::vec3(79.5f, 79.5f, 1.0f));
	obj->setShader(shader);
	objects.push_back(obj); //adiciona o quarto obj ASTEROID

	obj = new Sprite;
	obj->setPosition(glm::vec3(objsetX[2], objsetY[2], 0.0));
	obj->setDimension(glm::vec3(79.5f, 79.5f, 1.0f));
	obj->setShader(shader);
	objects.push_back(obj); //adiciona o quinto obj ASTEROID

	obj = new Sprite;
	obj->setPosition(glm::vec3(600.0f, 400.0f, 0.0));
	obj->setDimension(glm::vec3(79.5f, 79.5f, 1.0f));
	obj->setShader(shader);
	objects.push_back(obj); //adiciona o sexto obj PLANETA

	obj = new Sprite;
	obj->setPosition(glm::vec3(720.0f, 520.0f, 0.0));
	obj->setDimension(glm::vec3(155.0f, 155.0f, 1.0f));
	obj->setShader(shader);
	objects.push_back(obj); //adiciona o sétimo obj PLANETA

	obj = new Sprite;
	obj->setPosition(glm::vec3(0.0f, 0.0f, 0.0));
	obj->setDimension(glm::vec3(159.0f, 159.0f, 1.0f));
	obj->setShader(shader);
	objects.push_back(obj); //adiciona o oitavo obj PLANETA

	//Carregamento das texturas (pode ser feito intercalado na criação)
	//Futuramente, utilizar classe de materiais e armazenar dimensoes, etc
	unsigned int texID = loadTexture("../textures/background.png");
	objects[0]->setTexture(texID);

	texnave[0] = loadTexture("../textures/nave1.png");
	texnave[1] = loadTexture("../textures/naved.png");
	texnave[2] = loadTexture("../textures/navee.png");

	objects[1]->setTexture(texnave[0]);

	texID = loadTexture("../textures/asteroid.png");
	objects[2]->setTexture(texID);
	objects[3]->setTexture(texID);
	objects[4]->setTexture(texID);

	texID = loadTexture("../textures/planeta.png");
	objects[5]->setTexture(texID);
	objects[6]->setTexture(texID);
	objects[7]->setTexture(texID);

	//Definindo a janela do mundo (ortho2D)
	ortho2D[0] = 0.0f; //xMin
	ortho2D[1] = 800.0f; //xMax
	ortho2D[2] = 0.0f; //yMin
	ortho2D[3] = 600.0f; //yMax

	//Habilita transparência
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void SceneManager::setupCamera2D() //TO DO: parametrizar aqui
{
	float zNear = -1.0, zFar = 1.0; //estão fixos porque não precisamos mudar

	projection = glm::ortho(ortho2D[0], ortho2D[1], ortho2D[2], ortho2D[3], zNear, zFar);


	//Obtendo o identificador da matriz de projeção para enviar para o shader
	GLint projLoc = glGetUniformLocation(shader->ID, "projection");
	//Enviando a matriz de projeção para o shader
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

unsigned int SceneManager::loadTexture(string filename)
{
	unsigned int texture;

	// load and create a texture 
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
										   // set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load image, create texture and generate mipmaps
	int width, height, nrChannels;

	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3) //jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else //png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	glActiveTexture(GL_TEXTURE0);

	return texture;
}

bool SceneManager::TestCollision() {
	if (setX <= objsetX[0] + 30 && setX >= objsetX[0] - 30)
	{
		if (setY <= objsetY[0] + 30 && setY >= objsetY[0] - 30)
		{
			return true;
		}
	}
	if (setX <= objsetX[1] + 30 && setX >= objsetX[1] - 30)
	{
		if (setY <= objsetY[1] + 30 && setY >= objsetY[1] - 30)
		{
			return true;
		}
	}
	if (setX <= objsetX[2] + 30 && setX >= objsetX[2] - 30)
	{
		if (setY <= objsetY[2] + 30 && setY >= objsetY[2] - 30)
		{
			return true;
		}
	}
	return false;
}