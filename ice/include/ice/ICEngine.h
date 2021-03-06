/*
 * icengine.h
 *
 *  Created on: May 13, 2014
 *      Author: sni
 */

#ifndef ICENGINE_H_
#define ICENGINE_H_

#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <typeinfo>
#include <vector>

#include "ice/Identifier.h"
#include "ice/Time.h"
#include "ice/TimeFactory.h"
#include "ice/XMLReader.h"
#include "ice/communication/Communication.h"
#include "ice/coordination/Coordinator.h"
#include "ice/coordination/InformationModel.h"
#include "ice/coordination/ModelComperator.h"
#include "ice/information/BaseInformationStream.h"
#include "ice/information/InformationStream.h"
#include "ice/information/InformationStreamTemplate.h"
#include "ice/information/StreamStore.h"
#include "ice/information/InformationType.h"
#include "ice/information/StreamFactory.h"
#include "ice/model/ProcessingModelGenerator.h"
#include "ice/ontology/OntologyInterface.h"
#include "ice/processing/EventHandler.h"
#include "ice/processing/Node.h"
#include "ice/processing/NodeStore.h"
#include "ice/representation/GContainerFactory.h"
#include "ice/representation/ASPTransformationGeneration.h"
#include "ice/ros/RosCommunication.h"

// Forward declarations
namespace ice
{
class UpdateStrategie;
} /* namespace ice */

namespace ice
{

//* ICEngine
/**
 * The Engine of the information coordination system. It provides a
 * simple knowledge processing system and collects and shares information.
 *
 */
class ICEngine : public std::enable_shared_from_this<ICEngine>
{
public:
  /*!
   * \brief This constructor creates an engine object with a specialized configuration.
   *
   * This constructor creates an engine object with a specialized configuration.
   *
   * \param timeFactory The time factory to create time stamps.
   * \param streamFActory Factory to create information stream objects.
   * \param ontologyIri The iri of the main ontology.
   * \param iri The iri of this engine.
   * \param config The configuration object.
   */
  ICEngine(std::shared_ptr<TimeFactory> timeFactory, std::shared_ptr<StreamFactory> streamFactory, std::string ontologyIri, std::string iri,
           std::shared_ptr<Configuration> config = std::make_shared<Configuration>());

  /*!
   * \brief Default destructor
   *
   * Default destructor
   */
  virtual ~ICEngine();

  /*!
   * \brief Initialize the engine.
   *
   * Initialize the engine.
   */
  void init();

  /*!
   * \brief Starts the engine.
   *
   * Starts the engine.
   */
  void start();

  /*!
   * \brief Returns the time factory used by this engine.
   *
   * Returns the time factory used by this engine.
   */
  std::shared_ptr<TimeFactory> getTimeFactory();

  /*!
   * \brief Returns the configuration object.
   *
   * Returns the configuration object.
   */
  std::shared_ptr<Configuration> getConfig();

  /*!
   * \brief Returns the event handler.
   *
   * Returns the event handler.
   */
  std::shared_ptr<EventHandler> getEventHandler();

  /*!
   * \brief Returns the stream store.
   *
   * Returns the stream store.
   */
  std::shared_ptr<StreamStore> getStreamStore();

  /*!
   * \brief Returns the node store.
   *
   * Returns the node store.
   */
  std::shared_ptr<NodeStore> getNodeStore();

  /*!
   * \brief Returns the communication interface.
   *
   * Returns the communication interface.
   */
  std::shared_ptr<Communication> getCommunication();

  /*!
   * \brief Returns the coordinator.
   *
   * Returns the coordinator.
   */
  std::shared_ptr<Coordinator> getCoordinator();

  /*!
   * \brief Returns the stream factory.
   *
   * Returns the stream factory.
   */
  std::shared_ptr<StreamFactory> getStreamFactory();

  /*!
   * \brief Returns the ontology interface.
   *
   * Returns the ontology interface.
   */
  std::shared_ptr<OntologyInterface> getOntologyInterface();

  /*!
   * \brief Returns the processing model generator.
   *
   * Returns the processing model generator.
   */
  std::shared_ptr<ProcessingModelGenerator> getProcessingModelGenerator();

  /*!
   * \brief Reads a xml file and adds specified elements to the engine. Returns
   * true if successful, false otherwise.
   *
   * Extracts the information type description, the stream specifications, the
   * stream requests, and the node descriptions from the file. The components
   * are added to this engine. Returns true if the process was successful and
   * false if an error is occurred.
   *
   * \param fileName The file to be read.
   */
  bool readFromFile(const std::string fileName);

  /*!
   * \brief Reads a list of xml file and adds specified elements to the engine.
   * Returns true if successful, false otherwise.
   *
   * Extracts the information type description, the stream specifications, the
   * stream requests, and the node descriptions from the files. The components
   * are added to this engine. Returns true if the process was successful and
   * false if an error is occurred.
   *
   * \param fileName The file to be read.
   */
  bool readFromFiles(std::initializer_list<std::string> fileNameList);

  /*!
   * \brief Returns a model description of the information processing within this engine.
   *
   * Returns a model description of the information processing within this engine.
   */
  std::shared_ptr<InformationModel> getInformationModel();

  /*!
   * \brief Returns the identifier of this engine.
   *
   * Returns the identifier of this engine.
   */
  identifier getId() const;

  std::string getIri() const;

  std::shared_ptr<UpdateStrategie> getUpdateStrategie();

  bool isRunning();

  std::shared_ptr<GContainerFactory> getGContainerFactory();

  std::shared_ptr<ASPTransformationGeneration> getASPTransformationGeneration();

private:
  int readXMLInformation(XMLInformation* information, const std::string namePrefix);

private:
  identifier id; /**< The identifier of this engine */
  const std::string ontologyIri; /**< The iri of the main ontology */
  const std::string iri; /**< The iri of this engine */
  bool initialized; /**< True if the engine is initialized, else false */
  bool running; /**< True if the engine is running, alse false */
  std::shared_ptr<TimeFactory> timeFactory; /**< time factory to create time stamps */
  std::shared_ptr<Configuration> config; /**< The configuration object */
  std::shared_ptr<EventHandler> eventHandler; /**< Handler to execute asynchronous tasks */
  std::shared_ptr<StreamStore> streamStore; /**< The Stream store */
  std::shared_ptr<NodeStore> nodeStore; /**< The node store */
  std::shared_ptr<Communication> communication; /**< The communication interface */
  std::shared_ptr<Coordinator> coordinator; /**< Coordinator object which coordinates the cooperation */
  std::shared_ptr<StreamFactory> streamFactory; /**< Factory to create InformationStream objects */
  std::shared_ptr<ModelComperator> modelComperator; /**< Comparator to find similarities in different information model */
  std::shared_ptr<OntologyInterface> ontologyInterface; /*< Interface to access the ontology */
  std::shared_ptr<ProcessingModelGenerator> modelGenerator; /*< Processing model generator */
  std::shared_ptr<UpdateStrategie> updateStrategie; /**< Update strategie to modify the information processing */
  std::shared_ptr<GContainerFactory> gcontainerFactory; /**< Factory to create generic containers */
  std::shared_ptr<ASPTransformationGeneration> aspTransformationGenerator; /**< Component to create transformations based on an asp programm */
  std::mutex mtx_; /**< Mutex */
};

} /* namespace ice */

#endif /* ICENGINE_H_ */
