// Ŭnicode please 
#include "renderer.h"

class DiffuseSpecularMapping : public AbstractLogic {
public:
	DiffuseSpecularMapping(float width, float height);
	~DiffuseSpecularMapping();

	virtual bool Init();
	virtual bool Update(float dt);
	virtual void Draw();

private:
	std::unique_ptr<haruna::gl::ShaderProgram> prog_;
	std::unique_ptr<haruna::gl::Texture> diffuse_map_;
	std::unique_ptr<haruna::gl::Texture> specular_map_;

	float y_rot_;
};