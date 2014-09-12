#include <gtest/gtest.h>
#include "ClingWrapper.h"
#include "External.h"
#include "BaseLiteral.h"
#include "BoolLiteral.h"
#include "clasp/solver.h"
#include <chrono>
#include <map>
#include <vector>

using namespace std;

TEST(ClingWrap, simpleTest)
{
  supplementary::ClingWrapper* cw = new supplementary::ClingWrapper();
  cw->addKnowledgeFile("data/asp/nodeComposition.lp");
  cw->init();

  // entities
  cw->ground("entity", {"entity1"});

  // systems
  auto system1 = cw->getExternal("system", {"system1", 100}, true);
  auto system2 = cw->getExternal("system", {"system2", 10}, true);

  // inputs
  auto input1 = cw->getExternal("inputStream",
                                {"system1", "system1", Gringo::Value("information", {"entity1", "scope1", "rep1"}), 0, 90, 1}, true);
  auto input2 = cw->getExternal("inputStream",
                                {"system1", "system1", Gringo::Value("information", {"entity1", "scope2", "rep1"}), 5, 90, 1}, true);
  auto input3 = cw->getExternal("inputStream",
                                {"system2", "system2", Gringo::Value("information", {"entity1", "scope2", "rep1"}), 0, 99, 1}, false);

  // requireds
  auto required = cw->getExternal("requiredStream", {"system1", Gringo::Value("information", {"entity1", "scope3", "rep1"}), -1,
                                                     -1}, true);

  // add transfer
  auto transfer = cw->getExternal("transfer", {"system2", "system1", 1, 2}, true);

  // add node1
  cw->add("node1", {}, "#external nodeTemplate(system1,node1).");
  auto node1 = cw->getExternal("nodeTemplate", {"system1", "node1"}, "node1", {}, true);
  cw->add("node1", {}, "input(system1,node1,scope1,rep1,1,1) :- nodeTemplate(system1,node1).");
  cw->add("node1", {}, "input(system1,node1,scope2,rep1,1,1) :- nodeTemplate(system1,node1).");
  cw->add("node1", {}, "output(system1,node1,scope3,rep1).");
  cw->add("node1", {}, "nodeDelay(system1,node1,1).");
  cw->add("node1", {}, "nodeCost(system1,node1,10).");
  cw->ground("node1", {});

  // add node2
  cw->add("node2", {}, "#external nodeTemplate(system1,node2).");
  auto node2 = cw->getExternal("nodeTemplate", {"system1", "node2"}, "node2", {}, true);
  cw->add("node2", {}, "input(system1,node2,scope1,rep1,1,1) :- nodeTemplate(system1,node2).");
  cw->add("node2", {}, "input(system1,node2,scope2,rep1,2,2) :- nodeTemplate(system1,node2).");
  cw->add("node2", {}, "output(system1,node2,scope3,rep1).");
  cw->add("node2", {}, "nodeDelay(system1,node2,1).");
  cw->add("node2", {}, "nodeCost(system1,node2,5).");
  cw->ground("node2", {});

  auto query1 = cw->getExternal("query", {1}, true);

  cw->solve();
  //cw->printLastModel();

  EXPECT_EQ(true, cw->query("node", {"system1", "node1", "entity1", 1}));
  EXPECT_EQ(false, cw->query("node", {"system1", "node2", "entity1", 1}));
  EXPECT_EQ(true, cw->query("connect", {"system1","node1","system1",Gringo::Value("information", {"entity1", "scope2", "rep1"}), 1}));
  EXPECT_EQ(true, cw->query("sumCost", {12, 1}));
  EXPECT_EQ(true, cw->query("streamDelay", {"system1","system1",Gringo::Value("information", {"entity1", "scope3", "rep1"}),1,6}));

  input3->assign(true);
  cw->solve();

  EXPECT_EQ(true, cw->query("node", {"system1", "node1", "entity1", 1}));
  EXPECT_EQ(false, cw->query("node", {"system1", "node2", "entity1", 1}));
  EXPECT_EQ(false, cw->query("connect", {"system1","node1","system1",Gringo::Value("information", {"entity1", "scope2", "rep1"}), 1}));
  EXPECT_EQ(true, cw->query("connect", {"system1","node1","system2",Gringo::Value("information", {"entity1", "scope2", "rep1"}), 1}));
  EXPECT_EQ(true, cw->query("sumCost", {12, 1}));
  EXPECT_EQ(false, cw->query("streamDelay", {"system1","system1",Gringo::Value("information", {"entity1", "scope3", "rep1"}),1,6}));
  EXPECT_EQ(true, cw->query("streamDelay", {"system1","system1",Gringo::Value("information", {"entity1", "scope3", "rep1"}),1,2}));

  node1->assign(false);
  cw->solve();

  EXPECT_EQ(false, cw->query("node", {"system1", "node1", "entity1", 1}));
  EXPECT_EQ(true, cw->query("node", {"system1", "node2", "entity1", 1}));
  EXPECT_EQ(true, cw->query("connect", {"system1","node2","system1",Gringo::Value("information", {"entity1", "scope1", "rep1"}), 1}));
  EXPECT_EQ(true, cw->query("connect", {"system1","node2","system1",Gringo::Value("information", {"entity1", "scope2", "rep1"}), 1}));
  EXPECT_EQ(true, cw->query("connect", {"system1","node2","system2",Gringo::Value("information", {"entity1", "scope2", "rep1"}), 1}));
  EXPECT_EQ(true, cw->query("sumCost", {8, 1}));
  EXPECT_EQ(true, cw->query("streamDelay", {"system1","system1",Gringo::Value("information", {"entity1", "scope3", "rep1"}),1,6}));
  EXPECT_EQ(false, cw->query("streamDelay", {"system1","system1",Gringo::Value("information", {"entity1", "scope3", "rep1"}),1,2}));
}

TEST(ClingWrap, threeSystems)
{
  supplementary::ClingWrapper* cw = new supplementary::ClingWrapper();
  cw->addKnowledgeFile("data/asp/nodeComposition.lp");
  cw->init();

  // entities
  cw->ground("entity", {"entity1"});

  // systems
  auto system1 = cw->getExternal("system", {"system1", 100}, true);
  auto system2 = cw->getExternal("system", {"system2", 10}, true);
  auto system3 = cw->getExternal("system", {"system3", 10}, true);

  // inputs
  auto input1 = cw->getExternal("inputStream",
                                {"system1", "system1", Gringo::Value("information", {"entity1", "scope1", "rep1"}), 0, 90, 1}, true);

  // requires
  auto required = cw->getExternal("requiredStream", {"system3", Gringo::Value("information", {"entity1", "scope1", "rep1"}), -1,
                                                     -1}, true);

  // add transfer
  auto transfer1_2 = cw->getExternal("transfer", {"system2", "system1", 1, 2}, true);
  auto transfer2_3 = cw->getExternal("transfer", {"system3", "system2", 1, 2}, true);

  auto query1 = cw->getExternal("query", {1}, true);

  cw->solve();
 // cw->printLastModel();
// streamTransfer(SYSTEM_SOURCE,SYSTEM_TARGET,SOURCE,INFO,k,TRANSFER_DELAY,TRANSFER_COST)
  EXPECT_EQ(true, cw->query("streamTransfer", {"system1","system2","system1",Gringo::Value("information", {"entity1", "scope1", "rep1"}), 1, 1, 2}));
  EXPECT_EQ(true, cw->query("streamTransfer", {"system2","system3","system1",Gringo::Value("information", {"entity1", "scope1", "rep1"}), 1, 1, 2}));
}

TEST(ClingWrap, informationTranslation)
{
  supplementary::ClingWrapper* cw = new supplementary::ClingWrapper();
  cw->addKnowledgeFile("data/asp/ontology.lp");
  cw->addKnowledgeFile("data/asp/nodeComposition.lp");
  cw->init();

  // entities
  cw->ground("entity", {"entity1"});

  // systems
  auto system1 = cw->getExternal("system", {"system1", 100}, true);
  auto system2 = cw->getExternal("system", {"system2", 100}, true);

  // inputs
  auto input1 = cw->getExternal("inputStream",
                                {"system1", "system1", Gringo::Value("information", {"nase", "position", "coords"}), 0, 90, 1}, true);

  // requires
  auto required = cw->getExternal("requiredStream", {"system2", Gringo::Value("information", {"nase", "position", "wgs84"}), -1,
                                                     -1}, true);

  // add transfer
  auto transfer1_2 = cw->getExternal("transfer", {"system2", "system1", 1, 2}, true);

  // add traslation
  auto translation1_2 = cw->getExternal("interRepresentationOperation", {"system1", "coords", "wgs84", 1, 1}, true);

  auto query1 = cw->getExternal("query", {1}, true);

  cw->solve();
 // cw->printLastModel();
// streamTransfer(SYSTEM_SOURCE,SYSTEM_TARGET,SOURCE,INFO,k,TRANSFER_DELAY,TRANSFER_COST)
//  EXPECT_EQ(true, cw->query("streamTransfer", {"system1","system2","system1",Gringo::Value("information", {"entity1", "scope1", "rep1"}), 1, 1, 2}));
  EXPECT_EQ(true, cw->query("stream", {"system2","system1",Gringo::Value("information", {"nase", "position", "wgs84"}),1}));
  EXPECT_EQ(true, cw->query("streamDelay", {"system2","system1",Gringo::Value("information", {"nase", "position", "wgs84"}),1,2}));
  EXPECT_EQ(true, cw->query("streamCost", {"system2","system1",Gringo::Value("information", {"nase", "position", "wgs84"}),1,0}));
}


TEST(ClingWrap, informationExtraction)
{
  supplementary::ClingWrapper* cw = new supplementary::ClingWrapper();
  cw->addKnowledgeFile("data/asp/ontology.lp");
  cw->addKnowledgeFile("data/asp/nodeComposition.lp");
  cw->init();

  // entities
  cw->ground("entity", {"entity1"});

  // systems
  auto system1 = cw->getExternal("system", {"system1", 100}, true);
  auto system2 = cw->getExternal("system", {"system2", 100}, true);

  // inputs
  auto input1 = cw->getExternal("inputStream",
                                {"system1", "system1", Gringo::Value("information", {"nase", "position", "coords"}), 0, 90, 1}, true);

  // requires
  auto required = cw->getExternal("requiredStream", {"system2", Gringo::Value("information", {"nase", "alt", "floatRep"}), -1,
                                                     -1}, true);

  // add transfer
  auto transfer1_2 = cw->getExternal("transfer", {"system2", "system1", 1, 2}, true);

  // add traslation
  auto translation1_2 = cw->getExternal("interRepresentationOperation", {"system1", "coords", "wgs84", 1, 1}, true);

  auto query1 = cw->getExternal("query", {1}, true);

  cw->solve();
 // cw->printLastModel();
// streamTransfer(SYSTEM_SOURCE,SYSTEM_TARGET,SOURCE,INFO,k,TRANSFER_DELAY,TRANSFER_COST)
//  EXPECT_EQ(true, cw->query("streamTransfer", {"system1","system2","system1",Gringo::Value("information", {"entity1", "scope1", "rep1"}), 1, 1, 2}));
  EXPECT_EQ(true, cw->query("stream", {"system2","system1",Gringo::Value("information", {"nase", "alt", "floatRep"}),1}));
  EXPECT_EQ(true, cw->query("streamDelay", {"system2","system1",Gringo::Value("information", {"nase", "position", "wgs84"}),1,2}));
  EXPECT_EQ(false, cw->query("streamDelay", {"system2","system1",Gringo::Value("information", {"nase", "position", "wgs84"}),1,4}));
  EXPECT_EQ(true, cw->query("streamCost", {"system2","system1",Gringo::Value("information", {"nase", "position", "wgs84"}),1,0}));
}
