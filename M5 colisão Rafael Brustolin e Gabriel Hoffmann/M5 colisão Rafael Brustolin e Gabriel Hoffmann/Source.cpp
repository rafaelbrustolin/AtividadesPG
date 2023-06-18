#include "Layer.h"
#include "Shader.h"
#include "Sprite.h" 
#include "Layer.h"
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>


using namespace std;

//static controllers for mouse and keyboard
static bool keys[1024];
static bool resized;
static GLuint width, height;

const int layer0 = 4;
const int layer1 = 3;
const int layer2 = 2;
const int layer3 = 1;
const int layer4 = 0;

const int personagem = 5;
const int pedra = 6;


//Variável estática para controle da movimentação do personagem
enum Movimento { esquerda, parado, direita, quebra };
static Movimento mov = parado;

float PARALLAX_RATE = 0.00005f;

//Programa de shader 
Shader* shader;

int loadTexture(string fpath)
{
	unsigned int texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	int width, height, nrChannels;

	unsigned char* data = stbi_load(fpath.c_str(), &width, &height, &nrChannels, 0);

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

//Mudança da variável estática que controla o movimento do personagem de acordo com input de teclas
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
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
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		mov = direita;
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		mov = esquerda;
	}
	else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		mov = quebra;
	}
	else
	{
		mov = parado;
	}
}

void resize(GLFWwindow* window, int w, int h)
{
	width = w;
	height = h;
	resized = true;

	// Define the viewport dimensions
	glViewport(0, 0, width, height);
}

void addShader(string vFilename, string fFilename)
{
	shader = new Shader(vFilename.c_str(), fFilename.c_str());
}

int main()
{
	width = 800;
	height = 600;

	//Janela GLFW (por enquanto, assumimos apenas 1 instância)
	GLFWwindow* window;


	//Câmera 2D - Matriz de projeção (ortográfica) com os limites em x,y
	glm::vec4 ortho2D; //xmin, xmax, ymin, ymax
	glm::mat4 projection;
	Layer* layers = new Layer[7];
	//Nossos objetos (sprites) da cena
	vector <Sprite*> objects;

	// Init GLFW
	glfwInit();

	// Create a GLFWwindow object that we can use for GLFW's functions
	window = glfwCreateWindow(width, height, "M5 - Camadas + colisao", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	//Setando a callback de redimensionamento da janela
	glfwSetWindowSizeCallback(window, resize);

	//glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	// Build and compile our shader program
	addShader("./shaders/vertex.vs", "./shaders/fragment.frag");


	//setup the scene -- LEMBRANDO QUE A DESCRIÇÃO DE UMA CENA PODE VIR DE ARQUIVO(S) DE 
	// CONFIGURAÇÃO

	float verticesCena[] = {
		// positions          // colors           // texture coords
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0, // top right
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0  // top left 
	};
	float verticesPersonagem[] = {
		// positions          // colors           // texture coords
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   .125f, 1.0, // top right
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   .125f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.006f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.006f, 1.0  // top left 
	};

	//Definindo posições de cada Sprites.
	layers[layer4].adcionarObjeto(400.0f, 300.0f, -40.0, 800.0f, 600.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, shader->ID, verticesCena);
	layers[layer3].adcionarObjeto(400.0f, 300.0f, -35.0, 800.0f, 600.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, shader->ID, verticesCena);
	layers[layer2].adcionarObjeto(400.0f, 300.0f, -30.0, 800.0f, 600.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, shader->ID, verticesCena);
	layers[layer1].adcionarObjeto(400.0f, 300.0f, -25.0, 800.0f, 600.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, shader->ID, verticesCena);
	layers[layer0].adcionarObjeto(400.0f, 300.0f, -20.0, 800.0f, 600.0f, 0.0f, 0.0f, 0.0f, 3.0f, 0.0f, shader->ID, verticesCena);
	layers[personagem].adcionarObjeto(80.0f, 210.0f, -1.0f, 100.0f, 200.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, shader->ID, verticesPersonagem);
	layers[personagem].setDesloc(0.2f);
	layers[pedra].adcionarObjeto(800.0f, 150.0f, -1.0f, 100.0f, 100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, shader->ID, verticesCena);
	layers[pedra].setDesloc(0.12f);

	//Adicionando Textura para cada Sprites
	unsigned int texID = loadTexture("./textures/fundo.png");
	layers[layer4].objects[0]->setTexture(texID);
	
	texID = loadTexture("./textures/sol.png");
	layers[layer3].objects[0]->setTexture(texID);

	texID = loadTexture("./textures/cidade.png");
	layers[layer2].objects[0]->setTexture(texID);

	texID = loadTexture("./textures/arvores.png");
	layers[layer1].objects[0]->setTexture(texID);

	texID = loadTexture("./textures/chao.png");
	layers[layer0].objects[0]->setTexture(texID);

	texID = loadTexture("./textures/homem.png");
	layers[personagem].objects[0]->setTexture(texID);

	texID = loadTexture("./textures/pedra.png");
	layers[pedra].objects[0]->setTexture(texID);

	//Definindo a janela do mundo (ortho2D)
	ortho2D[0] = 0.0f; //xMin
	ortho2D[1] = 800.0f; //xMax
	ortho2D[2] = 0.0f; //yMin
	ortho2D[3] = 600.0f; //yMax

	resized = true; //para entrar no setup da câmera na 1a vez


	float fw = 0.125f;
	float offsetx = 0;
	int frameAtual = 0;
	float previous = glfwGetTime();
	float previous_pulo = glfwGetTime();

	float sprite_speed = 0.1;
	bool colidindo = false;

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	while (!glfwWindowShouldClose(window))
	{
				
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Clear the colorbuffer
		glClearColor(0.89f, 0.89f, 0.87f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		double current_seconds = glfwGetTime();

		if (keys[GLFW_KEY_ESCAPE])
			glfwSetWindowShouldClose(window, GL_TRUE);
		//AQUI APLICAREMOS TRANSFORMAÇÕES NOS SPRITES//

		//Controle de movimentação do personagem
		switch (mov)
		{
		case esquerda:
			layers[personagem].objects[0]->setPosX(layers[personagem].objects[0]->getPosX() - layers[personagem].getDesloc());
			sprite_speed = 0.1;
			break;
		case direita:
			if (colidindo == false) {
				layers[personagem].objects[0]->setPosX(layers[personagem].objects[0]->getPosX() + layers[personagem].getDesloc());
				sprite_speed = 0.06;
			}
			break;
		case quebra:
			if (colidindo == true) {
				layers[pedra].objects[0]->setPosX(900.0f);
				layers[pedra].setDesloc(0.12f);
				layers[layer0].objects[0]->setRateX(3.0);
				layers[layer1].objects[0]->setRateX(2.0);
				layers[layer2].objects[0]->setRateX(1.0);
				colidindo = false;
			}
			break;
		default:
			sprite_speed = 0.08;
			break;
		}

		if (colidindo == false) {
			layers[pedra].objects[0]->setPosX(layers[pedra].objects[0]->getPosX() - layers[pedra].getDesloc());
			layers[pedra].objects[0]->setPosition(glm::vec3(layers[pedra].objects[0]->getPosX(), layers[pedra].objects[0]->getPosY(), layers[pedra].objects[0]->getPosZInicial()));
		}
		else {
			layers[pedra].setDesloc(0.0f);
		}

		layers[personagem].objects[0]->setPosition(glm::vec3(layers[personagem].objects[0]->getPosX(), layers[personagem].objects[0]->getPosY(), layers[personagem].objects[0]->getPosZInicial()));

		//Colisão do personagem com a pedra
		if ((layers[personagem].objects[0]->getPosX() + 31 >= layers[pedra].objects[0]->getPosX() - 32.5f) && (layers[personagem].objects[0]->getPosX() < layers[pedra].objects[0]->getPosX()))
		{
			colidindo = true;
			layers[layer0].objects[0]->setRateX(0);
			layers[layer1].objects[0]->setRateX(0);
			layers[layer2].objects[0]->setRateX(0);
		}
		else {
			colidindo = false;
		}

		if (resized) //se houve redimensionamento na janela, redefine a projection matrix
		{
			float zNear = -100.0, zFar = 100.0; //estão fixos porque não precisamos mudar

			projection = glm::ortho(ortho2D[0], ortho2D[1], ortho2D[2], ortho2D[3], zNear, zFar);

			//Obtendo o identificador da matriz de projeção para enviar para o shader
			GLint projLoc = glGetUniformLocation(shader->ID, "projection");
			//Enviando a matriz de projeção para o shader
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
			resized = false;
		}

		//atualiza e desenha os Sprites

		for (int i = 0; i < 7; i++)
		{
			if (i == personagem) {
				if (((current_seconds - previous) > (sprite_speed)) && colidindo == false)
				{
					previous = current_seconds;

					if (frameAtual == 7) {
						frameAtual = 0;
					}
					else {
						frameAtual++;
					}

					offsetx = fw * (float)frameAtual;
					layers[personagem].objects[0]->setOffsetX(offsetx);
				}
			}
			else {
				layers[i].objects[0]->setOffsetX(layers[i].objects[0]->getOffsetX() + (PARALLAX_RATE * layers[i].objects[0]->getRateX()));
			}
			glUniform1f(glGetUniformLocation(shader->ID, "offsetx"), layers[i].objects[0]->getOffsetX());
			layers[i].updateCamada();
		}
		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
}
