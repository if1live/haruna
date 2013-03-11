// Ŭnicode please
#include "test_stdafx.h"
#include "haruna/debug_draw_manager.h"

using namespace glm;
using namespace std;
using namespace haruna;

TEST(DebugDrawManager, Update) 
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
