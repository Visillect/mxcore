#include "gtest/gtest.h"
#include <mxprops/mxprops.h>

using namespace mxprops;

TEST(MxPropsTest, Basic)
{
  PTree tree;
  int const a_value = 23;
  std::string const a_value_str = "23";

  {
    PTree::Ref root = tree.root("my_root");
    root.set("a_value", a_value);
    EXPECT_EQ(a_value, root.get<int>("a_value"));
    EXPECT_EQ(a_value_str, root.get<std::string>("a_value"));
  }

  {
    PTree::ConstRef root = tree.root("my_root");
    EXPECT_EQ(a_value, root.get<int>("a_value"));
    EXPECT_EQ(a_value_str, root.get<std::string>("a_value"));
  }
}

TEST(MxPropsTest, ConstRoot)
{
  PTree const tree;
  PTree::ConstRef root = tree.root("my_root");
  EXPECT_FALSE(root.getOptional<int>("a_value"));
}

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
