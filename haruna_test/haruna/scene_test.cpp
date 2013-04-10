// Ŭnicode please
#include "test_stdafx.h"
#include "haruna/scene.h"

using namespace haruna;

class SceneNodeTest : public ::testing::Test {
protected:
	virtual void SetUp()
	{
		//root
		//-child_1
		// -node_1
		// -node_2
		//-child_2

		root = new ContainerNode();
		child_1 = new ContainerNode();
		child_2 = new ContainerNode();
		node_1 = new SceneNode();
		node_2 = new SceneNode();

		root->Add(child_1);
		root->Add(child_2);
		child_1->Add(node_1);
		child_1->Add(node_2);
	}
	virtual void TearDown()
	{
		delete(root);
	}
protected:
	ContainerNode *root;
	ContainerNode *child_1;
	ContainerNode *child_2;
	SceneNode *node_1;
	SceneNode *node_2;
};

TEST_F(SceneNodeTest, Size)
{
	EXPECT_EQ(2, root->Size());
	EXPECT_EQ(2, child_1->Size());
	EXPECT_EQ(0, child_2->Size());
	EXPECT_EQ(0, node_2->Size());
}

TEST_F(SceneNodeTest, Remove)
{
	

	EXPECT_EQ(true, child_1->Remove(node_1));

	EXPECT_EQ(2, root->Size());
	EXPECT_EQ(1, child_1->Size());
	EXPECT_EQ(0, child_2->Size());
	EXPECT_EQ(0, node_2->Size());

	//잘못된 삭제는 false
	EXPECT_EQ(false, child_1->Remove(child_1));

	//정상 삭제
	EXPECT_EQ(true, root->Remove(child_1));

	EXPECT_EQ(1, root->Size());
	EXPECT_EQ(0, child_2->Size());
}

TEST_F(SceneNodeTest, Add)
{
	EXPECT_EQ(false, child_1->Add(child_1));
	EXPECT_EQ(false, child_1->Add(node_1));
	EXPECT_EQ(false, child_1->Add(root));
	EXPECT_EQ(true, child_1->Add(new SceneNode()));
}

TEST_F(SceneNodeTest, Detach)
{
	EXPECT_EQ(2, root->Size());
	EXPECT_EQ(2, child_1->Size());
	EXPECT_EQ(0, child_2->Size());
	EXPECT_EQ(0, node_2->Size());

	EXPECT_EQ(true, root->Detach(child_1));

	EXPECT_EQ(1, root->Size());
	EXPECT_EQ(0, child_2->Size());

	child_2->Add(child_1);

	EXPECT_EQ(1, root->Size());
	EXPECT_EQ(1, child_2->Size());
	EXPECT_EQ(2, child_1->Size());
}