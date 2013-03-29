// Ŭnicofde please 
#include "stdafx.h"
#include "debug_draw.h"

#include <memory>

#include "sora/io/file.h"
#include "sora/io/filesystem.h"
#include "sora/math_helper.h"
#include "haruna/primitive_mesh.h"
#include "shader.h"
#include "gl_env.h"
#include "sys_font.h"

#include "render_state.h"

using glm::vec4;
using glm::vec3;
using glm::mat4;
using sora::io::Filesystem;
using sora::io::ReadonlyCFile;

namespace haruna {;
namespace gl {
	struct Vertex_1P1C {
		glm::vec3 p;
		vec4ub c;
		Vertex_1P1C(const glm::vec3 &p, const vec4ub &c) : p(p), c(c) {}
		Vertex_1P1C() {}
	};

	struct Vertex_1P1C1UV {
		glm::vec3 p;
		vec4ub c;
		glm::vec2 uv;
		Vertex_1P1C1UV(const glm::vec3 &p, const vec4ub &c, const glm::vec2 &uv) : p(p), c(c), uv(uv) {}
		Vertex_1P1C1UV() {}
	};

	glm::vec4 ConvertColor(const haruna::vec4ub &orig) 
	{
		vec4 color;
		for(int i = 0 ; i < 4 ; i++) {
			color[i] = (float)orig[i] / 255.0f;
		}
		return color;
	}

	class DebugShader {
	public:
		DebugShader() {}
		virtual ~DebugShader() {}
		bool CreateShaderProgram(const std::string &vs_filename, const std::string &fs_filename)
		{
			std::string vs_path = Filesystem::GetAppPath(vs_filename);
			std::string fs_path = Filesystem::GetAppPath(fs_filename);
			ReadonlyCFile vs_file(vs_path);
			ReadonlyCFile fs_file(fs_path);
			bool vs_open_result = vs_file.Open();
			bool fs_open_result = fs_file.Open();
			if(!vs_open_result) {
				return false;
			}
			if(!fs_open_result) {
				return false;
			}
	
			std::string vs_src(static_cast<const char*>(vs_file.GetBuffer()));
			std::string fs_src(static_cast<const char*>(fs_file.GetBuffer()));
	
			haruna::gl::VertexShader vs(vs_src);
			haruna::gl::FragmentShader fs(fs_src);

			prog_.reset(new haruna::gl::ShaderProgram(vs, fs));
			bool prog_result = prog_->Init();
			if(!prog_result) {
				return false;
			}
			return true;
		}

		bool Deinit()
		{
			if(prog_.get() != nullptr) {
				return prog_->Deinit();
			}
			return false;
		}

		const ShaderProgram* prog() const { return prog_.get(); }
		ShaderProgram* prog() { return prog_.get(); }

	private:
		std::unique_ptr<haruna::gl::ShaderProgram> prog_;
	};

	class TextShader : public DebugShader {
	public:
		TextShader() {}
		virtual ~TextShader() {}

		bool Init()
		{
			bool prog_init = CreateShaderProgram("shader/text.vs", "shader/text.fs");
			if(!prog_init) {
				return false;
			}

			pos_loc_ = prog()->GetAttribLocation("a_position");
			texcoord_loc_ = prog()->GetAttribLocation("a_texcoord");		
			mvp_loc_ = prog()->GetUniformLocation("u_mvp");
			color_loc_ = prog()->GetUniformLocation("u_color");
			font_tex_loc_ = prog()->GetUniformLocation("s_tex");

			return true;
		}
		void ApplyColor(const haruna::vec4ub &color)
		{
			vec4 colorf = ConvertColor(color);
			glUniform4fv(color_loc(), 1, glm::value_ptr(colorf));
		}
		const haruna::gl::ShaderLocation &mvp_loc() const { return mvp_loc_; }
		const haruna::gl::ShaderLocation &pos_loc() const { return pos_loc_; }
		const haruna::gl::ShaderLocation &color_loc() const { return color_loc_; }
		const haruna::gl::ShaderLocation &texcoord_loc() const { return texcoord_loc_; }
		const haruna::gl::ShaderLocation &font_tex_loc() const { return font_tex_loc_; }
	private:
		haruna::gl::ShaderLocation mvp_loc_;
		haruna::gl::ShaderLocation pos_loc_;
		haruna::gl::ShaderLocation texcoord_loc_;
		haruna::gl::ShaderLocation color_loc_;
		haruna::gl::ShaderLocation font_tex_loc_;
	};

	class ColorShader : public DebugShader {
	public:
		ColorShader() {}
		virtual ~ColorShader() {}

		bool Init()
		{
			bool prog_init = CreateShaderProgram("shader/color.vs", "shader/color.fs");
			if(!prog_init) {
				return false;
			}
			pos_loc_ = prog()->GetAttribLocation("a_position");
			color_loc_ = prog()->GetAttribLocation("a_color");
			mvp_loc_ = prog()->GetUniformLocation("u_mvp");
			
			return true;
		}
		const haruna::gl::ShaderLocation &mvp_loc() const { return mvp_loc_; }
		const haruna::gl::ShaderLocation &pos_loc() const { return pos_loc_; }
		const haruna::gl::ShaderLocation &color_loc() const { return color_loc_; }
	private:
		haruna::gl::ShaderLocation mvp_loc_;
		haruna::gl::ShaderLocation pos_loc_;
		haruna::gl::ShaderLocation color_loc_;
	};


	std::vector<DrawCmdData<Vertex_1P>> wire_sphere_mesh;
	std::unique_ptr<TextShader> text_shader;
	std::unique_ptr<ColorShader> color_shader;
	std::unique_ptr<SysFont> sys_font;

	////////////////

	bool GLDebugDraw_Init()
	{
		WireSphereFactory factory(1, 16, 16);
		wire_sphere_mesh = factory.CreateSimpleMesh();

		sys_font.reset(new SysFont());
		if(!sys_font->Init()) {
			return false;
		}

		text_shader.reset(new TextShader());
		if(!text_shader->Init()) {
			return false;
		}

		color_shader.reset(new ColorShader());
		if(!color_shader->Init()) {
			return false;
		}
		return true;
	}
	bool GLDebugDraw_Deinit()
	{
		if(sys_font.get() != nullptr) {
			bool retval = sys_font->Deinit();
			sys_font.reset(nullptr);
			if(!retval) {
				return false;
			}
		}

		if(text_shader.get() != nullptr) {
			bool retval = text_shader->Deinit();
			text_shader.reset(nullptr);
			if(!retval) {
				return false;
			}
		}
		if(color_shader.get() != nullptr) {
			bool retval = color_shader->Deinit();
			color_shader.reset(nullptr);
			if(!retval) {
				return false;
			}
		}
		return true;
	}

	
	struct LineWidthReplacer {
		LineWidthReplacer(float width) : width_(width) 
		{
			if(width_ != 1.0f) {
				glLineWidth(width);
			}
		}
		~LineWidthReplacer()
		{
			if(width_ != 1.0f) {
				glLineWidth(1.0f);
			}
		}
	private:
		float width_;
	};

	struct PointSizeReplacer {
		PointSizeReplacer(float size) : size_(size) 
		{
			if(size != 1.0f) {
				glPointSize(size_);
			}
		}
		~PointSizeReplacer()
		{
			if(size_ != 1.0f) {
				glPointSize(1.0f);
			}
		}
	private:
		float size_;
	};

		

	DebugDrawer3D::DebugDrawer3D()
	{
	}

	DebugDrawer3D::~DebugDrawer3D()
	{
	}

	void DebugDrawer3D::Draw(const DebugDrawManager &mgr) 
	{
		RenderState::Get()->Set3D();		

		// 깊이씹고 렌더링하는것과 깊이고려 렌더링을 분리해서 그리자
		// 굳이 순서대로 그릴 이유가 없겟더라
		std::vector<DebugDraw3D*> with_depth_text_list;
		std::vector<DebugDraw3D*> without_depth_text_list;
		std::vector<DebugDraw3D*> with_depth_color_list;
		std::vector<DebugDraw3D*> without_depth_color_list;

		auto it = mgr.begin_3d();
		auto endit = mgr.end_3d();
		for( ; it != endit ; ++it) {
			DebugDraw3D *cmd = it->get();
			std::vector<DebugDraw3D*> *text_list = nullptr;
			std::vector<DebugDraw3D*> *color_list = nullptr;

			if(cmd->depth_enable == true) {
				text_list = &with_depth_text_list;
				color_list = &with_depth_color_list;
			} else {
				text_list = &without_depth_text_list;
				color_list = &without_depth_color_list;
			}
			
			switch(cmd->type) {
			case kDebugDraw3DString:
				text_list->push_back(cmd);
				break;
			case kDebugDraw3DAxis:
			default:
				color_list->push_back(cmd);
				break;
			}
		}

		// pos+color속성으로 그릴것
		color_shader->prog()->Use();
		glEnableVertexAttribArray(color_shader->pos_loc());
		glEnableVertexAttribArray(color_shader->color_loc());

		glEnable(GL_DEPTH_TEST);
		for(DebugDraw3D *cmd : with_depth_color_list) {
			AbstractDebugDrawer3D::DrawElem(cmd);
		}		

		glDisable(GL_DEPTH_TEST);
		for(DebugDraw3D *cmd : without_depth_color_list) {
			AbstractDebugDrawer3D::DrawElem(cmd);
		}

		// 텍스트는 별도 처리
		text_shader->prog()->Use();
		glEnableVertexAttribArray(text_shader->pos_loc());
		glEnableVertexAttribArray(text_shader->texcoord_loc());

		// 폰트 텍스쳐는 1번만 쓰면 된다
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sys_font->tex_id());
		glUniform1i(text_shader->font_tex_loc(), 0);

		// 폰트는 알파가 있어서 블렌드 필요
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		for(DebugDraw3D *cmd : with_depth_text_list) {
			AbstractDebugDrawer3D::DrawElem(cmd);
		}		

		glDisable(GL_DEPTH_TEST);
		for(DebugDraw3D *cmd : without_depth_text_list) {
			AbstractDebugDrawer3D::DrawElem(cmd);
		}
		glDisable(GL_BLEND);
	}

	void DebugDrawer3D::DrawElem(DebugDraw3D_Line *cmd) 
	{
		mat4 mvp = cmd->GetMVPMatrix();
		glUniformMatrix4fv(color_shader->mvp_loc(), 1, GL_FALSE, glm::value_ptr(mvp));

		std::array<Vertex_1P1C, 2> vert_list;
		vert_list[0] = Vertex_1P1C(cmd->p1, cmd->color);
		vert_list[1] = Vertex_1P1C(cmd->p2, cmd->color);

		int stride = sizeof(vert_list[0]);
		glVertexAttribPointer(color_shader->pos_loc(), 3, GL_FLOAT, GL_FALSE, stride,  &vert_list[0].p);
		glVertexAttribPointer(color_shader->color_loc(), 4, GL_UNSIGNED_BYTE, GL_FALSE, stride, &vert_list[0].c);

		LineWidthReplacer replacer(cmd->line_width);
		glDrawArrays(GL_LINES, 0, 2);
	}
	void DebugDrawer3D::DrawElem(DebugDraw3D_Cross *cmd) 
	{
		mat4 mvp = cmd->GetMVPMatrix();
		glUniformMatrix4fv(color_shader->mvp_loc(), 1, GL_FALSE, glm::value_ptr(mvp));

		Vertex_1P1C vert(cmd->pos, cmd->color);
		int stride = sizeof(vert);
		glVertexAttribPointer(color_shader->pos_loc(), 3, GL_FLOAT, GL_FALSE, stride, &vert.p);
		glVertexAttribPointer(color_shader->color_loc(), 4, GL_UNSIGNED_BYTE, GL_FALSE, stride, &vert.c);

		PointSizeReplacer replacer(cmd->size);
		glDrawArrays(GL_POINTS, 0, 1);
	}

	void DebugDrawer3D::DrawElem(DebugDraw3D_Sphere *cmd) 
	{
		mat4 model_mat = cmd->model_mat;
		model_mat = glm::scale(model_mat, vec3(cmd->radius, cmd->radius, cmd->radius));
		mat4 mvp = cmd->proj_mat * cmd->view_mat * model_mat;
		glUniformMatrix4fv(color_shader->mvp_loc(), 1, GL_FALSE, glm::value_ptr(mvp));

		SR_ASSERT(wire_sphere_mesh.size() == 1);
		const DrawCmdData<Vertex_1P> &cmd_elem = wire_sphere_mesh[0];
		auto index_list = cmd_elem.index_list;
		auto pos_vert_list = cmd_elem.vertex_list;
		SR_ASSERT(index_list.empty() == false);

		std::vector<Vertex_1P1C> vert_list(pos_vert_list.size());
		for(size_t i = 0 ; i < pos_vert_list.size() ; ++i) {
			Vertex_1P1C &vert = vert_list[i];
			vert.p = pos_vert_list[i].p;
			vert.c = cmd->color;
		}

		int stride = sizeof(vert_list[0]);
		glVertexAttribPointer(color_shader->pos_loc(), 3, GL_FLOAT, GL_FALSE, stride, &vert_list[0].p);
		glVertexAttribPointer(color_shader->color_loc(), 4, GL_UNSIGNED_BYTE, GL_FALSE, stride, &vert_list[0].c);
		glDrawElements(GL_LINES, index_list.size(), GL_UNSIGNED_SHORT, &index_list[0]);
	}

	void DebugDrawer3D::DrawElem(DebugDraw3D_String *cmd) 
	{
		RenderState *render_state = RenderState::Get();
		mat4 mvp = cmd->GetMVPMatrix();
		
		//billboard 같은 느낌으로 글자 쓰기
		//기울어지는거 없이 항상 글자가 뜨도록 적절히 만들기
		vec4 cliping_pos = mvp * vec4(cmd->pos, 1);   // -1~1에 적절히 위치한 좌표(로 아마도 보정되엇을거다)
		cliping_pos /= cliping_pos.w;
		cliping_pos.z = -cliping_pos.z; //보정된 좌표계는 z방향 다르다

		float win_w = (float)render_state->win_width();
		float win_h = (float)render_state->win_height();
		// -1~+1로 보정된 좌표를 윈도우좌표로 변환
		vec3 win_coord(
			(cliping_pos.x + 1) * win_w/2.0f,
			(cliping_pos.y + 1.0f) * win_h/2.0f,
			cliping_pos.z
			);

		text_shader->ApplyColor(cmd->color);

		mat4 mvp_mat = glm::ortho(0.0f, win_w, 0.0f, win_h);
		mvp_mat = glm::translate(mvp_mat, win_coord);
		mvp_mat = glm::scale(mvp_mat, vec3(cmd->scale));
		glUniformMatrix4fv(text_shader->mvp_loc(), 1, GL_FALSE, glm::value_ptr(mvp_mat));

		Label label(sys_font.get(), cmd->msg);
		auto vert_list = label.vertex_list();
		auto index_list = label.index_list();

		int stride = sizeof(vert_list[0]);
		glVertexAttribPointer(text_shader->pos_loc(), 3, GL_FLOAT, GL_FALSE, stride, &vert_list[0].p);
		glVertexAttribPointer(text_shader->texcoord_loc(), 2, GL_FLOAT, GL_FALSE, stride, &vert_list[0].uv);
		glDrawElements(GL_TRIANGLES, index_list.size(), GL_UNSIGNED_SHORT, &index_list[0]);
	}

	void DebugDrawer3D::DrawElem(DebugDraw3D_Axis *cmd) 
	{
		std::array<Vertex_1P1C, 6> vertex_list;
		vec4ub red(255, 0, 0, 255);
		vec4ub green(0, 255, 0, 255);
		vec4ub blue(0, 0, 255, 255);

		// x = red
		vertex_list[0] = Vertex_1P1C(vec3(0, 0, 0), red);
		vertex_list[1] = Vertex_1P1C(vec3(cmd->size, 0, 0), red);

		// y = green
		vertex_list[2] = Vertex_1P1C(vec3(0, 0, 0), green);
		vertex_list[3] = Vertex_1P1C(vec3(0, cmd->size, 0), green);

		// z = blue
		vertex_list[4] = Vertex_1P1C(vec3(0, 0, 0), blue);
		vertex_list[5] = Vertex_1P1C(vec3(0, 0, cmd->size), blue);

		mat4 mvp = cmd->GetMVPMatrix() * cmd->xf;
		glUniformMatrix4fv(color_shader->mvp_loc(), 1, GL_FALSE, glm::value_ptr(mvp));

		int stride = sizeof(vertex_list[0]);
		glVertexAttribPointer(color_shader->pos_loc(), 3, GL_FLOAT, GL_FALSE, stride, &vertex_list[0].p);
		glVertexAttribPointer(color_shader->color_loc(), 4, GL_UNSIGNED_BYTE, GL_FALSE, stride, &vertex_list[0].c);
		glDrawArrays(GL_LINES, 0, vertex_list.size());
	}

	void DebugDrawer2D::DrawElem(DebugDraw2D_Line *cmd) 
	{
		RenderState *render_state = RenderState::Get();
		float width = static_cast<float>(render_state->win_width());
		float height = static_cast<float>(render_state->win_height());

		glm::mat4 model_mat;
		glm::mat4 view_mat;
		glm::mat4 proj_mat = glm::ortho(0.0f, width, 0.0f, height);

		glm::mat4 mvp_mat = proj_mat * view_mat * model_mat;
		glUniformMatrix4fv(color_shader->mvp_loc(), 1, GL_FALSE, glm::value_ptr(mvp_mat));


		std::array<Vertex_1P1C, 2> vert_list;
		vert_list[0] = Vertex_1P1C(vec3(cmd->p1.x, cmd->p1.y, 0), cmd->color);
		vert_list[1] = Vertex_1P1C(vec3(cmd->p2.x, cmd->p2.y, 0), cmd->color);
		int stride = sizeof(vert_list[0]);
		glVertexAttribPointer(color_shader->pos_loc(), 3, GL_FLOAT, GL_FALSE, stride, &vert_list[0].p);
		glVertexAttribPointer(color_shader->color_loc(), 4, GL_UNSIGNED_BYTE, GL_FALSE, stride, &vert_list[0].c);

		LineWidthReplacer replacer(cmd->line_width);
		glDrawArrays(GL_LINES, 0, 2);
		
	}
	void DebugDrawer2D::DrawElem(DebugDraw2D_Cross *cmd) 
	{
		RenderState *render_state = RenderState::Get();
		float width = static_cast<float>(render_state->win_width());
		float height = static_cast<float>(render_state->win_height());

		glm::mat4 model_mat;
		glm::mat4 view_mat;
		glm::mat4 proj_mat = glm::ortho(0.0f, width, 0.0f, height);

		glm::mat4 mvp_mat = proj_mat * view_mat * model_mat;
		glUniformMatrix4fv(color_shader->mvp_loc(), 1, GL_FALSE, glm::value_ptr(mvp_mat));

		Vertex_1P1C vert(vec3(cmd->pos.x, cmd->pos.y, 0), cmd->color);
		glVertexAttribPointer(color_shader->pos_loc(), 3, GL_FLOAT, GL_FALSE, 0, &vert.p);
		glVertexAttribPointer(color_shader->color_loc(), 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, &vert.c);

		PointSizeReplacer replacer(cmd->size);
		glDrawArrays(GL_POINTS, 0, 1);
	}

	void DebugDrawer2D::DrawElem(DebugDraw2D_Circle *cmd) 
	{
		RenderState *render_state = RenderState::Get();
		float width = static_cast<float>(render_state->win_width());
		float height = static_cast<float>(render_state->win_height());

		glm::mat4 model_mat;
		model_mat = glm::translate(model_mat, vec3(cmd->pos.x, cmd->pos.y, 0));
		model_mat = glm::scale(model_mat, glm::vec3(cmd->radius, cmd->radius, 1));
		glm::mat4 view_mat;
		glm::mat4 proj_mat = glm::ortho(0.0f, width, 0.0f, height);

		glm::mat4 mvp_mat = proj_mat * view_mat * model_mat;
		glUniformMatrix4fv(color_shader->mvp_loc(), 1, GL_FALSE, glm::value_ptr(mvp_mat));
		
		int split = 32;

		std::vector<Vertex_1P1C> point_list;
		point_list.reserve(split + 1);
		point_list.push_back(Vertex_1P1C(glm::vec3(0, 0, 0), cmd->color));
		for(int i = 0 ; i < split + 1 ; i++) {
			float rad_seg = sora::kPi / split  * 2;
			float rad = rad_seg * i;
			float x = cos(rad);
			float y = sin(rad);

			point_list.push_back(Vertex_1P1C(glm::vec3(x, y, 0), cmd->color));
		}
		
		int stride = sizeof(point_list[0]);
		glVertexAttribPointer(color_shader->pos_loc(), 3, GL_FLOAT, GL_FALSE, stride, &point_list[0].p);
		glVertexAttribPointer(color_shader->color_loc(), 4, GL_UNSIGNED_BYTE, GL_FALSE, stride, &point_list[0].c);
		glDrawArrays(GL_LINE_STRIP, 0, point_list.size());
	}

	void DebugDrawer2D::DrawElem(DebugDraw2D_String *cmd) 
	{
		RenderState *render_state = RenderState::Get();
		float width = static_cast<float>(render_state->win_width());
		float height = static_cast<float>(render_state->win_height());

		//폰트의 왼쪽위를 원점으로 사용하기 위해서 약간 올린다
		float base_line = SysFont::kFontSize * cmd->scale;

		glm::mat4 model_mat;
		model_mat = glm::translate(model_mat, vec3(cmd->pos.x, cmd->pos.y + base_line, 0));
		model_mat = glm::scale(model_mat, glm::vec3(cmd->scale, cmd->scale, 1));
		glm::mat4 view_mat;
		glm::mat4 proj_mat = glm::ortho(0.0f, width, 0.0f, height);
		glm::mat4 mvp_mat = proj_mat * view_mat * model_mat;
		glUniformMatrix4fv(text_shader->mvp_loc(), 1, GL_FALSE, glm::value_ptr(mvp_mat));

		text_shader->ApplyColor(cmd->color);

		Label label(sys_font.get(), cmd->msg);
		auto vert_list = label.vertex_list();
		auto index_list = label.index_list();

		int stride = sizeof(vert_list[0]);
		glVertexAttribPointer(text_shader->pos_loc(), 3, GL_FLOAT, GL_FALSE, stride, &vert_list[0].p);
		glVertexAttribPointer(text_shader->texcoord_loc(), 2, GL_FLOAT, GL_FALSE, stride, &vert_list[0].uv);
		glDrawElements(GL_TRIANGLES, index_list.size(), GL_UNSIGNED_SHORT, &index_list[0]);
	}

	
	
	void DebugDrawer2D::Draw(const DebugDrawManager &mgr) 
	{
		//string은 쉐이더가 다르니 따로 그린다
		//종류를 나눠서 그리다보니까 높이 정보는 씹는다
		//어차피 2차원 디버깅은 통계 관련에서만 쓸데니까 별 문제 없을거다
		std::vector<DebugDraw2D*> color_list;
		std::vector<DebugDraw2D*> text_list;

		auto it = mgr.begin_2d();
		auto endit = mgr.end_2d();
		for( ; it != endit ; ++it) {
			DebugDraw2D *cmd = it->get();
			if(cmd->type == kDebugDraw2DString) {
				text_list.push_back(cmd);
			} else {
				color_list.push_back(cmd);
			}
		}

		//2차원 환경에서의 렌더링 기본 설정
		RenderState::Get()->Set2D();

		color_shader->prog()->Use();
		glEnableVertexAttribArray(color_shader->pos_loc());
		for(DebugDraw2D *cmd : color_list) {
			AbstractDebugDrawer2D::DrawElem(cmd);
		}
		
		text_shader->prog()->Use();
		glEnableVertexAttribArray(text_shader->pos_loc());
		glEnableVertexAttribArray(text_shader->texcoord_loc());

		// 폰트 텍스쳐는 1번만 쓰면 된다
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sys_font->tex_id());
		glUniform1i(text_shader->font_tex_loc(), 0);

		for(DebugDraw2D *cmd : text_list) {
			AbstractDebugDrawer2D::DrawElem(cmd);
		}
	}

	DebugDrawer2D::DebugDrawer2D()
	{
	}
	DebugDrawer2D::~DebugDrawer2D()
	{
	}
}	// namespace gl
}	// namespace haruna