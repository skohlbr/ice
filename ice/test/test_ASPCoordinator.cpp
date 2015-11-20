#include <gtest/gtest.h>

#include "ice/ICEngine.h"
#include "ice/information/InformationSpecification.h"
#include "ice/information/InformationStore.h"
#include "ice/model/aspModel/ASPModelGenerator.h"
#include "ice/model/updateStrategie/UpdateStrategie.h"

#include "etc/EngineStuff.cpp"
#include "etc/TestTime.cpp"

TEST(ASPModelGenerator, simpleTest)
{
  ice::Node::registerNodeCreator("TestSourceNodeGrounding", &SimpleSourceNode::createNode);
  ice::Node::registerNodeCreator("TestComputationalNodeGrounding", &SmothingNode::createNode);

  auto streamFactory = std::make_shared<TestFactory>();
  auto timeFactory = std::make_shared<TestTimeFactory>();
  std::shared_ptr<ice::ICEngine> engine = std::make_shared<ice::ICEngine>(timeFactory, streamFactory, "http://vs.uni-kassel.de/IceTest#TestSystem");
  engine->init();

  bool result = engine->getOntologyInterface()->addOntologyIRI("http://vs.uni-kassel.de/IceTest");

  engine->start();

  auto spec1 = ice::InformationSpecification("testEntity1", "testEntity", "testScope1", "testRepresentation1");
  auto spec2 = ice::InformationSpecification("testEntity1", "testEntity", "testScope1", "testRepresentation2");

  auto stream1 = engine->getInformationStore()->getStream<ice::Position>(&spec1, "testSourceNodeInd",
                                                                         "testSystem");
  auto stream2 = engine->getInformationStore()->getStream<ice::Position>(&spec2, "testComputationalNodeInd",
                                                                         "testSystem");

  ASSERT_TRUE((stream1 ? true : false));
  ASSERT_TRUE((stream2 ? true : false));

  std::unique_ptr<ice::Position> position1(new ice::Position());
  position1->x = 3;
  position1->y = 2;
  position1->z = 1;

  stream1->add(std::move(position1));

  std::this_thread::sleep_for(std::chrono::milliseconds {10});

  auto position2 = stream2->getLast();

  ASSERT_TRUE((position2 ? true : false));
  EXPECT_EQ(2, position2->getInformation().x);
  EXPECT_EQ(1, position2->getInformation().y);
  EXPECT_EQ(0, position2->getInformation().z);
}

TEST(ASPModelGenerator, twoSystemsSimple)
{
  ice::Node::registerNodeCreator("TestSourceNodeGrounding", &SimpleSourceNode::createNode);
  ice::Node::registerNodeCreator("TestComputationalNodeGrounding", &SmothingNode::createNode);

  // create engine 1
  auto streamFactory = std::make_shared<TestFactory>();
  auto timeFactory = std::make_shared<TestTimeFactory>();
  std::shared_ptr<ice::ICEngine> engine = std::make_shared<ice::ICEngine>(timeFactory, streamFactory, "http://vs.uni-kassel.de/IceTest#TestCoordination1_SystemInd1");
  engine->init();
  bool result = engine->getOntologyInterface()->addOntologyIRI("http://vs.uni-kassel.de/IceTest");
  engine->start();

  // create engine 2
  streamFactory = std::make_shared<TestFactory>();
  timeFactory = std::make_shared<TestTimeFactory>();
  std::shared_ptr<ice::ICEngine> engine2 = std::make_shared<ice::ICEngine>(timeFactory, streamFactory, "http://vs.uni-kassel.de/IceTest#TestCoordination1_SystemInd2");
  engine2->init();
  result = engine2->getOntologyInterface()->addOntologyIRI("http://vs.uni-kassel.de/IceTest");
  engine2->start();

  // wait some time to enable the engines to find each other
  std::this_thread::sleep_for(std::chrono::milliseconds {2000});

  // test processing system 1
  auto spec1 = ice::InformationSpecification("testEntity1", "testEntity", "testScope1", "testRepresentation1");
  auto stream1 = engine->getInformationStore()->getStream<ice::Position>(&spec1, "testSourceNodeInd", "testCoordination1_SystemInd1");
  ASSERT_TRUE((stream1 ? true : false));

  // test processing system 2
  auto spec2 = ice::InformationSpecification("testEntity1", "testEntity", "testScope1", "testRepresentation1");
  auto stream2 = engine2->getInformationStore()->getStream<ice::Position>(&spec2, "testSourceNodeInd", "testCoordination1_SystemInd1");
  ASSERT_TRUE((stream2 ? true : false));

  // insert element in stream of system 2
  int x = rand();
  int y = rand();
  int z = rand();

  std::unique_ptr<ice::Position> position1(new ice::Position());
  position1->x = x;
  position1->y = y;
  position1->z = z;

  stream1->add(std::move(position1));

  // wait some time
  std::this_thread::sleep_for(std::chrono::milliseconds {500});

  // check if element was send to system 2 and placed in stream2
  auto position2 = stream2->getLast();

  ASSERT_TRUE((position2 ? true : false));
  EXPECT_EQ(x, position2->getInformation().x);
  EXPECT_EQ(y, position2->getInformation().y);
  EXPECT_EQ(z, position2->getInformation().z);
}

TEST(ASPModelGenerator, threeSystemsSimple)
{
  ice::Node::registerNodeCreator("TestSourceNodeGrounding", &SimpleSourceNode::createNode);
  ice::Node::registerNodeCreator("TestComputationalNodeGrounding", &SmothingNode::createNode);

  // create engine 1
  auto streamFactory = std::make_shared<TestFactory>();
  auto timeFactory = std::make_shared<TestTimeFactory>();
  std::shared_ptr<ice::ICEngine> engine = std::make_shared<ice::ICEngine>(timeFactory, streamFactory, "http://vs.uni-kassel.de/IceTest#TestCoordination2_SystemInd1");
  engine->init();
  bool result = engine->getOntologyInterface()->addOntologyIRI("http://vs.uni-kassel.de/IceTest");
  engine->start();

  // create engine 2
  streamFactory = std::make_shared<TestFactory>();
  timeFactory = std::make_shared<TestTimeFactory>();
  std::shared_ptr<ice::ICEngine> engine2 = std::make_shared<ice::ICEngine>(timeFactory, streamFactory, "http://vs.uni-kassel.de/IceTest#TestCoordination2_SystemInd2");
  engine2->init();
  result = engine2->getOntologyInterface()->addOntologyIRI("http://vs.uni-kassel.de/IceTest");
  engine2->start();

  // wait some time to enable the engines to find each other
  std::this_thread::sleep_for(std::chrono::milliseconds {2000});

  auto spec1 = ice::InformationSpecification("testEntity1", "testEntity", "testScope1", "testRepresentation1");
  auto spec2 = ice::InformationSpecification("testEntity1", "testEntity", "testScope1", "testRepresentation2");

  // test processing system 1
  auto stream11 = engine->getInformationStore()->getStream<ice::Position>(&spec1, "testSourceNodeInd", "testCoordination2_SystemInd1");
  ASSERT_TRUE((stream11 ? true : false));

  auto stream12 = engine2->getInformationStore()->getStream<ice::Position>(&spec2, "testComputationalNodeInd", "testCoordination2_SystemInd2");
  ASSERT_TRUE((stream12 ? true : false));

  // test processing system 2
  auto stream21 = engine2->getInformationStore()->getStream<ice::Position>(&spec1, "testSourceNodeInd", "testCoordination2_SystemInd1");
  ASSERT_TRUE((stream21 ? true : false));

  auto stream22 = engine2->getInformationStore()->getStream<ice::Position>(&spec2, "testComputationalNodeInd", "testCoordination2_SystemInd2");
  ASSERT_TRUE((stream22 ? true : false));

  // insert element in stream of system 2
  int x = rand();
  int y = rand();
  int z = rand();

  std::unique_ptr<ice::Position> position1(new ice::Position());
  position1->x = x;
  position1->y = y;
  position1->z = z;

  stream11->add(std::move(position1));

  // wait some time
  std::this_thread::sleep_for(std::chrono::milliseconds {500});

  // check if element was send to system 2 and placed in stream2
  auto position2 = stream12->getLast();

  ASSERT_TRUE((position2 ? true : false));
  EXPECT_EQ(x - 1, position2->getInformation().x);
  EXPECT_EQ(y - 1, position2->getInformation().y);
  EXPECT_EQ(z - 1, position2->getInformation().z);

  // TODO
}
