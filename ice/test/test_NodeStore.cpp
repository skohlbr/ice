#include <gtest/gtest.h>

#include "ice/ICEngine.h"
#include "ice/processing/NodeStore.h"

#include "etc/EngineStuff.cpp"
#include "etc/TestTime.cpp"

TEST(NodeStoreTest, create)
{
  ice::Node::registerNodeCreator("SimpleSourceNode", &SimpleSourceNode::createNode);
  ice::Node::registerNodeCreator("smothing", &SmothingNode::createNode);


  auto streamFactory = std::make_shared<TestFactory>();
  auto timeFactory = std::make_shared<TestTimeFactory>();
  std::shared_ptr<ice::ICEngine> engine = std::make_shared<ice::ICEngine>(timeFactory, streamFactory, "http://vs.uni-kassel.de/IceTest", "tut");
  engine->init();

  std::map<std::string, std::string> config;
//  const NodeType type, const std::string className, const std::string name,
//                                       const ont::entity entity, const ont::entity entityRelated,
//                                       std::map<std::string, std::string> config,
  auto node = engine->getNodeStore()->registerNode(ice::NodeType::PROCESSING, "smothing", "SmothingProcessingNode", "testEntity1", "none", config);

  ASSERT_TRUE(node ? true : false);
}
