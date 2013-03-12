// Ŭnicode please 
#pragma once

#include "renderer.h"

class Demo : public AbstractLogic {
public:
	Demo(float width, float height);
	virtual ~Demo();

	virtual bool Init();
	virtual bool Update(float dt);
	virtual void Draw();

private:
};