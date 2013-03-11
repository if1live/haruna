// Ŭnicofde please 
#include "stdafx.h"
#include "debug_draw.h"

namespace haruna {;
namespace gl {
	

	void DebugDrawer3D::Draw(const DebugDrawManager &mgr) 
	{
		/*
		mgr_ = const_cast<DebugDrawManager*>(&mgr);

		RenderState *dev = &Device::GetInstance()->render_state();
		dev->Set3D();

		DrawCmdList(mgr);

		mgr_ = NULL;
		*/
	}


	

	/*
	void DebugDrawer3D::ApplyDepthTest(DebugDraw3D *cmd) 
	{
	if(cmd->depth_enable == false) {
	glDisable(GL_DEPTH_TEST);
	}
	}
	void DebugDrawer3D::UnapplyDepthTest(DebugDraw3D *cmd) 
	{
	if(cmd->depth_enable == false) {
	glEnable(GL_DEPTH_TEST);
	}
	}
	*/
	///////////////////////////////////////////////

	void DebugDrawer3D::DrawElem(DebugDraw3D_Line *cmd) 
	{
		/*
		ShaderManager *shader_mgr = Device::GetInstance()->shader_mgr();
		Shader *shader = shader_mgr->Get(ShaderManager::kConstColor);
		RenderState *render_state = &Device::GetInstance()->render_state();
		render_state->UseShader(*shader);

		vec4 color = Draw2DPolicy::ConvertColor(cmd->color);
		bool const_color_result = shader->SetUniformVector(kConstColorHandleName, color);
		SR_ASSERT(const_color_result == true);

		vector<vec3> vertex_list;
		vertex_list.push_back(cmd->p1);
		vertex_list.push_back(cmd->p2);

		mat4 mvp = cmd->GetMVPMatrix();
		ShaderVariable mvp_var = shader->uniform_var(kMVPHandleName);
		SR_ASSERT(mvp_var.location != kInvalidShaderVarLocation);
		SetUniformMatrix(mvp_var, mvp);

		shader->SetVertexList(vertex_list);
		glLineWidth(cmd->line_width);
		ApplyDepthTest(cmd);
		shader->DrawArrays(kDrawLines, vertex_list.size());  
		UnapplyDepthTest(cmd);
		glLineWidth(1.0f);
		*/
	}
	void DebugDrawer3D::DrawElem(DebugDraw3D_Cross *cmd) 
	{
		/*
		ShaderManager *shader_mgr = Device::GetInstance()->shader_mgr();
		Shader &shader = *(shader_mgr->Get(ShaderManager::kConstColor));
		RenderState *dev = &Device::GetInstance()->render_state();
		dev->UseShader(shader);

		vec4 color = Draw2DPolicy::ConvertColor(cmd->color);
		shader.SetUniformVector(kConstColorHandleName, color);

		mat4 mvp = cmd->GetMVPMatrix();
		ShaderVariable mvp_var = shader.uniform_var(kMVPHandleName);
		SetUniformMatrix(mvp_var, mvp);

		vector<glm::vec3> vert_list;
		vert_list.push_back(cmd->pos);

		shader.SetVertexList(vert_list);

		ApplyDepthTest(cmd);
		glPointSize(cmd->size);
		shader.DrawArrays(kDrawPoints, vert_list.size());
		glPointSize(1.0f);
		UnapplyDepthTest(cmd);
		*/
	}

	void DebugDrawer3D::DrawElem(DebugDraw3D_Sphere *cmd) 
	{
		/*
		ShaderManager *shader_mgr = Device::GetInstance()->shader_mgr();
		Shader &shader = *(shader_mgr->Get(ShaderManager::kConstColor));
		RenderState *dev = &Device::GetInstance()->render_state();
		dev->UseShader(shader);

		vec4 color = Draw2DPolicy::ConvertColor(cmd->color);
		shader.SetUniformVector(kConstColorHandleName, color);

		static bool init_mesh = false;
		static GeometricObject<glm::vec3> mesh;
		if(init_mesh == false) {
		init_mesh = true;
		mesh.WireSphere(1.0f, 16, 16);
		}

		mat4 mvp = cmd->GetMVPMatrix();
		mvp = glm::translate(mvp, cmd->pos);
		mvp = glm::scale(mvp, vec3(cmd->radius, cmd->radius, cmd->radius));
		ShaderVariable mvp_var = shader.uniform_var(kMVPHandleName);
		SetUniformMatrix(mvp_var, mvp);

		ApplyDepthTest(cmd);
		auto it = mesh.Begin();
		auto endit = mesh.End();
		for( ; it != endit ; ++it) {
		//const DrawCmdData<Vertex> &cmd = *it;
		const DrawCmdData<vec3> &cmd = *it;
		shader.SetVertexList(cmd.vertex_list);
		if(cmd.index_list.empty()) {
		Shader::DrawArrays(cmd.draw_mode, cmd.vertex_list.size());
		} else {
		Shader::DrawElements(cmd.draw_mode, cmd.index_list);
		}
		}
		UnapplyDepthTest(cmd);
		*/
	}

	void DebugDrawer3D::DrawElem(DebugDraw3D_String *cmd) 
	{
		/*
		//billboard 같은 느낌으로 글자 쓰기
		//기울어지는거 없이 항상 글자가 뜨도록 적절히 만들기
		mat4 mvp = cmd->GetMVPMatrix();
		vec4 cliping_pos = mvp * vec4(cmd->pos, 1);   // -1~1에 적절히 위치한 좌표(로 아마도 보정되엇을거다)
		cliping_pos /= cliping_pos.w;
		cliping_pos.z = -cliping_pos.z; //보정된 좌표계는 z방향 다르다

		RenderState *dev = &Device::GetInstance()->render_state();
		float win_w = (float)dev->win_width();
		float win_h = (float)dev->win_height();
		// -1~+1로 보정된 좌표를 윈도우좌표로 변환
		vec3 win_coord(
		(cliping_pos.x + 1) * win_w/2.0f,
		(cliping_pos.y + 1.0f) * win_h/2.0f,
		cliping_pos.z);

		ShaderManager *shader_mgr = Device::GetInstance()->shader_mgr();
		Shader &shader = *(shader_mgr->Get(ShaderManager::kText));
		dev->UseShader(shader);

		vec4 color = Draw2DPolicy::ConvertColor(cmd->color);
		shader.SetUniformVector(kConstColorHandleName, color);

		sora::SysFont *font = Device::GetInstance()->sys_font();
		dev->UseTexture(font->font_texture(), 0);

		mat4 mvp_mat = glm::ortho(0.0f, win_w, 0.0f, win_h);
		mvp_mat = glm::translate(mvp_mat, win_coord);
		mvp_mat = glm::scale(mvp_mat, vec3(cmd->scale));
		ShaderVariable mvp_var = shader.uniform_var(kMVPHandleName);
		SetUniformMatrix(mvp_var, mvp_mat);

		//2d처럼 렌더링하기
		glEnable(GL_BLEND);

		ApplyDepthTest(cmd);
		sora::Label label(font, cmd->msg);
		shader.SetVertexList(label.vertex_list());
		shader.DrawElements(kDrawTriangles, label.index_list());
		UnapplyDepthTest(cmd);

		//3d용으로 복구
		glDisable(GL_BLEND);

		//점하나 적절히 찍기
		DebugDraw3D_Cross cross_cmd;
		cross_cmd.color = cmd->color;
		cross_cmd.depth_enable = cmd->depth_enable;
		cross_cmd.duration = cmd->duration;
		cross_cmd.model_mat = cmd->model_mat;
		cross_cmd.view_mat = cmd->view_mat;
		cross_cmd.projection_mat = cmd->projection_mat;
		cross_cmd.pos = cmd->pos;
		cross_cmd.size = 5;
		DrawElem(&cross_cmd);
		*/
	}

	void DebugDrawer3D::DrawElem(DebugDraw3D_Axis *cmd) 
	{
		/*
		ShaderManager *shader_mgr = Device::GetInstance()->shader_mgr();
		Shader *shader = shader_mgr->Get(ShaderManager::kVertexColor);
		RenderState *render_state = &Device::GetInstance()->render_state();
		render_state->UseShader(*shader);

		vector<ColorVertex3D> vertex_list;
		vertex_list.reserve(6);

		vec4ub red(255, 0, 0, 255);
		vec4ub green(0, 255, 0, 255);
		vec4ub blue(0, 0, 255, 255);

		// x = red
		vertex_list.push_back(CreateColorVertex3D(0, 0, 0, red));
		vertex_list.push_back(CreateColorVertex3D(cmd->size, 0, 0, red));

		// y = green
		vertex_list.push_back(CreateColorVertex3D(0, 0, 0, green));
		vertex_list.push_back(CreateColorVertex3D(0, cmd->size, 0, green));

		// z = blue
		vertex_list.push_back(CreateColorVertex3D(0, 0, 0, blue));
		vertex_list.push_back(CreateColorVertex3D(0, 0, cmd->size, blue));

		mat4 mvp = cmd->GetMVPMatrix() * cmd->xf;
		ShaderVariable mvp_var = shader->uniform_var(kMVPHandleName);
		SetUniformMatrix(mvp_var, mvp);

		ApplyDepthTest(cmd);
		shader->SetVertexList(vertex_list);
		shader->DrawArrays(kDrawLines, vertex_list.size());  
		UnapplyDepthTest(cmd);
		*/
	}

	/*
	void DebugDrawer2D::BeforeDraw() {
		RenderState *dev = &Device::GetInstance()->render_state();
		dev->Set2D();
	}
	*/
	void DebugDrawer2D::DrawElem(DebugDraw2D_Line *cmd) 
	{
		/*
		ShaderManager *shader_mgr = Device::GetInstance()->shader_mgr();
		Shader &shader = *(shader_mgr->Get(ShaderManager::kConstColor));
		RenderState *dev = &Device::GetInstance()->render_state();
		dev->UseShader(shader);

		vec4 color = ConvertColor(cmd->color);
		shader.SetUniformVector(kConstColorHandleName, color);

		const mat4 &projection = dev->projection_mat();
		ShaderVariable mvp_var = shader.uniform_var(kMVPHandleName);
		SetUniformMatrix(mvp_var, projection);

		vector<glm::vec2> vert_list;
		vert_list.push_back(cmd->p1);
		vert_list.push_back(cmd->p2);

		shader.SetVertexList(vert_list);

		glLineWidth(cmd->line_width);
		shader.DrawArrays(kDrawLines, 2);
		glLineWidth(1.0f);
		*/
	}
	void DebugDrawer2D::DrawElem(DebugDraw2D_Cross *cmd) 
	{
		/*
		ShaderManager *shader_mgr = Device::GetInstance()->shader_mgr();
		Shader &shader = *(shader_mgr->Get(ShaderManager::kConstColor));
		RenderState *dev = &Device::GetInstance()->render_state();
		dev->UseShader(shader);

		vec4 color = ConvertColor(cmd->color);
		shader.SetUniformVector(kConstColorHandleName, color);

		const mat4 &projection = dev->projection_mat();
		ShaderVariable mvp_var = shader.uniform_var(kMVPHandleName);
		SetUniformMatrix(mvp_var, projection);

		vector<glm::vec2> vert_list;
		vert_list.push_back(cmd->pos);

		shader.SetVertexList(vert_list);

		glPointSize(cmd->size);
		shader.DrawArrays(kDrawPoints, 2);
		glPointSize(1.0f);
		*/
	}

	void DebugDrawer2D::DrawElem(DebugDraw2D_Sphere *cmd) 
	{
		/*
		ShaderManager *shader_mgr = Device::GetInstance()->shader_mgr();
		Shader &shader = *(shader_mgr->Get(ShaderManager::kConstColor));
		RenderState *dev = &Device::GetInstance()->render_state();
		dev->UseShader(shader);

		vec4 color = ConvertColor(cmd->color);
		shader.SetUniformVector(kConstColorHandleName, color);

		const mat4 &projection = dev->projection_mat();
		glm::mat4 mvp = glm::translate(projection, vec3(cmd->pos.x, cmd->pos.y, 0));
		ShaderVariable mvp_var = shader.uniform_var(kMVPHandleName);
		SetUniformMatrix(mvp_var, mvp);

		GeometricObject<glm::vec3> mesh;
		mesh.WireSphere(cmd->radius, 16, 16);

		auto it = mesh.Begin();
		auto endit = mesh.End();
		for( ; it != endit ; ++it) {
			//const DrawCmdData<Vertex> &cmd = *it;
			const DrawCmdData<vec3> &cmd = *it;
			shader.SetVertexList(cmd.vertex_list);
			if(cmd.index_list.empty()) {
				Shader::DrawArrays(cmd.draw_mode, cmd.vertex_list.size());
			} else {
				Shader::DrawElements(cmd.draw_mode, cmd.index_list);
			}
		}
		*/
	}

	void DebugDrawer2D::DrawElem(DebugDraw2D_String *cmd) 
	{
		/*
		ShaderManager *shader_mgr = Device::GetInstance()->shader_mgr();
		Shader &shader = *(shader_mgr->Get(ShaderManager::kText));
		RenderState *dev = &Device::GetInstance()->render_state();
		dev->UseShader(shader);

		vec4 color = ConvertColor(cmd->color);
		shader.SetUniformVector(kConstColorHandleName, color);

		sora::SysFont *font = Device::GetInstance()->sys_font();
		dev->UseTexture(font->font_texture(), 0);

		//해상도에 맞춰서 적절히 설정
		const mat4 &projection = dev->projection_mat();
		ShaderVariable mvp_var = shader.uniform_var(kMVPHandleName);

		mat4 world_mat(1.0f);
		world_mat = glm::translate(world_mat, vec3(cmd->pos.x, cmd->pos.y, 0));
		world_mat = glm::scale(world_mat, vec3(cmd->scale));
		mat4 mvp = projection * world_mat;
		SetUniformMatrix(mvp_var, mvp);
		sora::Label label(font, cmd->msg);
		shader.SetVertexList(label.vertex_list());

		shader.DrawElements(kDrawTriangles, label.index_list());
		*/
	}

	/*
	glm::vec4 DebugDrawer2D::ConvertColor(const sora::vec4ub &orig) {
		vec4 color;
		for(int i = 0 ; i < 4 ; i++) {
			color[i] = (float)orig[i] / 255.0f;
		}
		return color;
	}
	*/

	
	void DebugDrawer2D::Draw(const DebugDrawManager &mgr) 
	{
		/*
		mgr_ = const_cast<Draw2DManager*>(&mgr);

		BeforeDraw();
		DrawCmdList(mgr);

		mgr_ = NULL;
		*/
	}
}	// namespace gl
}	// namespace haruna