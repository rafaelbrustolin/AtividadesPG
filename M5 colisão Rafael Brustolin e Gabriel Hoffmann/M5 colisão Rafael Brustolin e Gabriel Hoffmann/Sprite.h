#pragma once

//Esta classe vai ir sofrendo alterações durante o curso
//Aqui ela está preparada apenas para sprites texturizados

#include "Shader.h"
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Sprite
{
public:
	Sprite(float vertices[]);
	~Sprite() {}
	//Para setar os atributos 
	void setTexture(int texID);
	int getTexture();
	void setShader(GLuint shader) { this->shader = shader; glUseProgram(shader); }
	
	void setPosition(glm::vec3 pos) { this->pos = pos; } 
	void setDimension(glm::vec3 scale) { this->scale = scale; }
	void setAngle(float angle) { this->angle = angle; } //para 3D precisa angulo por eixo ou quaternion
	
	glm::vec3 getPosition();
	//Alguns gets e sets para maior controle dos objetos durante a atualização da cena
	void setRotation(float angle, glm::vec3 axis, bool reset = true);
	void setTranslation(glm::vec3 displacements, bool reset = true);
	void setScale(glm::vec3 scaleFactors, bool reset = true);

	float getPosXInicial();
	float getPosYInicial();
	float getPosZInicial();
	
	float getPosX();
	void setPosX(float x);

	float getPosY();
	void setPosY(float y);

	void setPosXInicial(float x);
	void setPosYInicial(float y);
	void setPosZInicial(float z);

	//rates x y e offset x y
	void setOffsetX(float x);
	void setOffsetY(float y);
	void setRateX(float x);
	void setRateY(float y);

	float getOffsetX();
	float getOffsetY();
	float getRateX();
	float getRateY();

	//desenho e atualização da camada
	void draw();
	void update();

protected:
	//Atributos gerais

	GLuint VAO; //ID do buffer de geometria

	glm::mat4 transform; //matriz de transformação

	unsigned int texID; //identificador da textura

	GLuint shader;
	
	//Atributos de controle da posição, orientação e dimensões do sprite
	glm::vec3 pos, scale;
	float angle;


	//offset e rate
	float offsetx, offsety, ratex, ratey;

	float posXInicial=0, posYInicial=0, posZInicial=0;
};

