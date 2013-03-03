// Ŭnicode please 
#include "renderer.h"

namespace haruna {;
class ParametricSurface;
}

class NormalMapping : public AbstractLogic {
public:
	NormalMapping(float width, float height);
	~NormalMapping();

	virtual bool Init();
	virtual bool Update(float dt);
	virtual void Draw();

private:
	std::unique_ptr<haruna::gl::ShaderProgram> prog_;

	std::unique_ptr<haruna::gl::Texture> diffuse_map_;
	std::unique_ptr<haruna::gl::Texture> specular_map_;
	std::unique_ptr<haruna::gl::Texture> normal_map_;

	float y_rot_;

	std::unique_ptr<haruna::ParametricSurface> mesh_;
};