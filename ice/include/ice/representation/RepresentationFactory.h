/*
 * Representation.h
 *
 *  Created on: 15.07.2015
 *      Author: paspartout
 */

#ifndef ICE_ICE_INCLUDE_ICE_REPRESENTATION_REPRESENTATIONFACTORY_H_
#define ICE_ICE_INCLUDE_ICE_REPRESENTATION_REPRESENTATIONFACTORY_H_

#include <string>
#include <vector>
#include <memory>
#include <map>

#include "Representation.h"
#include "RepresentationInstance.h"

namespace ice {

class RepresentationFactory {
public:
  RepresentationFactory();
  virtual ~RepresentationFactory();

  Representation* fromCSV(std::string reprStr, const char delim = ';');
  std::shared_ptr<std::vector<Representation*>> fromCSVStrings(
      std::vector<std::string> lines);

  std::shared_ptr<std::vector<Representation*>> getRepVec();
  std::shared_ptr<std::map<std::string, Representation*>> getRepMap();
  BasicRepresentationType getBasicRep(std::string rep);

  std::shared_ptr<std::map<std::string, RepresentationInstance*>> getInstanceMap();
  Representation* getRepresentation(std::string representation);

  RepresentationInstance *makeInstance(std::string name);
  RepresentationInstance *makeInstance(Representation* representation);

  void printReps();

private:
  Representation* addOrGet(std::string name);

  std::shared_ptr<std::vector<Representation*>> repVec;
  std::shared_ptr<std::map<std::string, Representation*>> repMap;
  std::shared_ptr<std::map<std::string, RepresentationInstance*>> repInstanceMap;
  std::map<std::string, BasicRepresentationType> typeMap;
};

}  // namespace ice

#endif /* ICE_ICE_INCLUDE_ICE_REPRESENTATION_REPRESENTATIONFACTORY_H_ */
