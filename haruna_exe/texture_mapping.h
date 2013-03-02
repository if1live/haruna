// Ŭnicode please 
#include "renderer.h"

class TextureMapping : public AbstractLogic {
public:
	TextureMapping(float width, float height);
	~TextureMapping();

	virtual bool Init();
	virtual bool Update(float dt);
	virtual void Draw();

private:
	std::unique_ptr<haruna::gl::ShaderProgram> prog_;
	std::unique_ptr<haruna::gl::Texture> tex_;
	
	float y_rot_;
};

