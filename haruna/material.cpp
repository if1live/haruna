// Ŭnicode please
#include "stdafx.h"
#include "material.h"

namespace haruna {;

//싱글턴처럼 사용하기 위한 용도
MaterialManager g_mgr;

bool Material_Init()
{
	g_mgr.Clear();
	return true;
}
bool Material_Deinit()
{
	g_mgr.Clear();
	return true;
}

Material::Material() 
	: shininess(20.0f), 
	props(0),
	use_light(true),
	type(kMaterialTypeUber) 
{
	//mtl format default value
	for(int i = 0 ; i < 4 ; i++) {
		ambient[i] = 1.0f;
		diffuse[i] = 1.0f;
		specular[i] = 1.0f;
	}
};

const Material &Material::NullMaterial() 
{
	static Material mtl;
	mtl.type = kMaterialTypeNone;
	mtl.props = 0;  //0이면 플래그 검사 그냥 통과하니까 전부 disable의미
	return mtl;
}

bool Material::operator==(const Material &o) const 
{
	if(type != o.type) { return false; }
	if(use_light != o.use_light) { return false; }
	if(name != o.name) { return false; }
	if(props != o.props) { return false; }
	if(ambient != o.ambient) { return false; }
	if(diffuse != o.diffuse) { return false; }
	if(specular != o.specular) { return false; }
	if(shininess != o.shininess) { return false; }
	if(diffuse_map != o.diffuse_map) { return false; }
	if(specular_map != o.specular_map) { return false; }
	if(normal_map != o.normal_map) { return false; }
	return true;
}

bool Material::operator!=(const Material &o) const 
{
	return !(*this == o);
}

/////////////////////////////////////
MaterialManager::MaterialManager() {}
MaterialManager::~MaterialManager() {}

MaterialManager &MaterialManager::GetInstance()
{
	return g_mgr;
}

bool MaterialManager::IsExist(const std::string &name) const 
{
	auto found = material_list_.find(name);
	if(found == material_list_.end()) {
		return false;
	} else {
		return true;
	}
}

bool MaterialManager::Add(const std::vector<Material> &mtl_list) 
{
	auto it = mtl_list.begin();
	auto endit = mtl_list.end();
	bool collision_occur = false;
	for( ; it != endit ; ++it) {
		bool add_result = Add(*it);
		if(add_result == false) {
			collision_occur = true;
		}
	}
	return collision_occur;
}
bool MaterialManager::Add(const Material &mtl) 
{
	if(IsExist(mtl.name) == true) {
		//name collision
		return false;
	}
	material_list_[mtl.name] = mtl;
	return true;
}
const Material &MaterialManager::Get(const std::string &name) const 
{
	auto found = material_list_.find(name);
	if(found == material_list_.end()) {
		//else..not found, use basic material
		static Material basic_material;
		return basic_material;
	}
	return found->second;
}
void MaterialManager::Clear() 
{
	material_list_.clear();
}

} //namespace haruna

