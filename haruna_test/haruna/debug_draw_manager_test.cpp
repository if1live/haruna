// Ŭnicode please
#include "test_stdafx.h"
#include "haruna/debug_draw_manager.h"
#include "haruna/gl/render_state.h"
#include "sora/io/filesystem.h"

using namespace glm;
using namespace std;
using namespace haruna;

using haruna::DebugDrawManager_Init;
using haruna::gl::RenderState_Init;
using sora::io::FS_Init;

using haruna::DebugDrawManager_Deinit;
using haruna::gl::RenderState_Deinit;
using sora::io::FS_Deinit;


class DebugDrawManagerTest : public ::testing::Test {
protected:
	virtual void SetUp()
	{
		FS_Init();
		DebugDrawManager_Init();
		RenderState_Init(640, 480);
	}
	virtual void TearDown()
	{
		RenderState_Deinit();
		DebugDrawManager_Deinit();
		FS_Deinit();
	}
};

TEST_F(DebugDrawManagerTest, Update) 
{
	DebugDrawManager mgr;
	EXPECT_EQ(0, mgr.CmdCount());

	mgr.AddAxis(mat4(1.0f), 1);			// without duration
	mgr.AddAxis(mat4(1.0f), 1, 1.0f);	// with duration
	EXPECT_EQ(2, mgr.CmdCount());

	mgr.Update(0.5f);
	EXPECT_EQ(1, mgr.CmdCount());

	mgr.Update(1.0f);
	EXPECT_EQ(0, mgr.CmdCount());

	mgr.Update(1.0f);
	EXPECT_EQ(0, mgr.CmdCount());
}

