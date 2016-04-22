
#include "gtest/gtest.h"

#include <geometry_msgs/Vector3.h>
#include <ros/package.h>

#include "IceServalBridge.h"
#include "RosGContainerPublisher.h"

void onMsg(const geometry_msgs::Vector3::ConstPtr& msg)
{
  std::cout << msg << std::endl;
}

void createConfig(ice::InitParams const * const params)
{
  // create folder
  mkdir(params->servalInstancePath.c_str(), 0700);

  std::ofstream myfile;
  myfile.open(params->servalInstancePath + "/serval.conf");
  myfile << "interfaces.0.match=*\n";
  myfile << "interfaces.0.socket_type=dgram\n";
  myfile << "interfaces.0.type=ethernet\n";
  myfile << "interfaces.0.port=4110\n";
  myfile << "rhizome.http.port=" << params->servalPort << "\n";
  myfile << "api.restful.users." << params->servalUser << ".password=" << params->servalPassword << "\n";
  myfile.close();
}


TEST(GMessagePublisher, simpleTest)
{
  ice::InitParams *params1 = new ice::InitParams();

  std::string path = ros::package::getPath("ice_serval_ros_bridge");

  params1->ontologyIri = "http://vs.uni-kassel.de/IceServalBridgeTest";
  params1->ontologyIriSelf = "http://vs.uni-kassel.de/IceServalBridgeTest#Mops";
  params1->ontologyPath = path + "/tests/data/";
  params1->servalInstancePath = "/tmp/mops";
  params1->servalHost = "localhost";
  params1->servalPort = 4111;
  params1->servalUser = "peter";
  params1->servalPassword = "venkman";
  params1->xmlInfoPath = path + "/tests/data/info_bridge_off.xml";

  createConfig(params1);

  ros::NodeHandle nh_("");
  ros::NodeHandle pnh_("~");

  auto subscriber = nh_.subscribe("testTopic", 100, &onMsg);

  ice::IceServalBridge bridge(nh_, pnh_, params1);

  bridge.init();

  std::string repStr = bridge.ontologyInterface->toShortIri("http://www.semanticweb.org/sni/ontologies/2013/7/Ice#CoordinatePositionRep");
  auto rep = bridge.gcontainerFactory->getRepresentation(repStr);

  ASSERT_NE(nullptr, rep);

  auto x = rep->accessPath({bridge.ontologyInterface->toShortIri("http://www.semanticweb.org/sni/ontologies/2013/7/Ice#XCoordinate")});
  auto y = rep->accessPath({bridge.ontologyInterface->toShortIri("http://www.semanticweb.org/sni/ontologies/2013/7/Ice#YCoordinate")});
  auto z = rep->accessPath({bridge.ontologyInterface->toShortIri("http://www.semanticweb.org/sni/ontologies/2013/7/Ice#ZCoordinate")});

}
