#include "Layer.h"



void Layer::updateCamada()
{
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->update();
		objects[i]->draw();
	}
}

//Aqui preparamos os Sprites com todas suas coordenadas e já com a possibilidade de dimensionar as sprites
void Layer::adcionarObjeto(float posX, float posY, float posZ, float dimX, float dimY, float dimZ, float offsetx, float offsety, float ratex, float ratey, GLuint shader, float vertices[])
{
	obj = new Sprite(vertices);
	obj->setPosition(glm::vec3(posX, posY, posZ));
	obj->setDimension(glm::vec3(dimX, dimY,dimZ));
	obj->setShader(shader);
	objects.push_back(obj);

	obj->setPosXInicial(posX);
	obj->setPosYInicial(posY);
	obj->setPosZInicial(posZ);
	obj->setPosY(posY);
	obj->setPosX(posX);
	obj->setOffsetX(offsetx);
	obj->setOffsetY(offsety);
	obj->setRateX(ratex);
	obj->setRateY(ratey);
}

//valor de deslocamento para movimentações da camada
void Layer::setDesloc(float deslocamento)
{
	desloc = deslocamento;
}

float Layer::getDesloc()
{
	return desloc;
}
