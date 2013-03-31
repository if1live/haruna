// Ŭnicode please
#pragma once

namespace haruna {;
typedef enum {
	kMaterialTypeNone,
	kMaterialTypeUber,
} MaterialType;

//material/uber shader에서 사용되는 플래그용
enum {
	kMaterialAmbient = 1 << 0,
	kMaterialDiffuse = 1 << 1,
	kMaterialSpecular = 1 << 2,
	kMaterialDiffuseMap = 1 << 3,
	kMaterialSpecularMap = 1 << 4,
	kMaterialNormalMap = 1 << 5,
};

bool Material_Init();
bool Material_Deinit();

struct Material {
	Material();
	static const Material &NullMaterial();

	MaterialType type;

	std::string name;
	unsigned int props;   //flag저장 용도
	bool use_light; //빛이 없으면(false인 경우) diffuse_map를 albedo로써 사용한다

	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	float shininess;  //specular

	std::string diffuse_map;
	std::string specular_map;
	std::string normal_map;

	bool operator==(const Material &o) const;
	bool operator!=(const Material &o) const;
};

class MaterialManager {
public:
	MaterialManager();
	~MaterialManager();

	static MaterialManager &GetInstance();
public:
	//이름중복이 발생한 경우 false
	bool Add(const std::vector<Material> &mtl_list);
	bool Add(const Material &mtl);
	bool IsExist(const std::string &name) const;
	const Material &Get(const std::string &name) const;
	void Clear();

private:
	//일단은 몇개 안될테니까 간단하게 구현
	//재질정보는 그렇게 크지도 않고 많지도 않을테니까 전부 떄려박아도 심각한 문제가
	//발생하지는 않을것이다.
	typedef std::unordered_map<std::string, Material> MaterialDict;
	MaterialDict material_list_;
};
} //namespace haruna
