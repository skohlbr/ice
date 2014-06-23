/*
 * BaseInformationStream.cpp
 *
 *  Created on: May 15, 2014
 *      Author: sni
 */

#include "ice/information/BaseInformationStream.h"

#include "ice/coordination/EngineState.h"
#include "ice/information/InformationType.h"
#include "ice/processing/EventHandler.h"

namespace ice
{

int BaseInformationStream::IDENTIFIER_COUNTER = 0;

BaseInformationStream::BaseInformationStream(const std::string name, std::weak_ptr<InformationType> informationType,
                                             std::shared_ptr<EventHandler> eventHandler,
                                             std::shared_ptr<InformationSpecification> specification,
                                             std::string provider, std::string description, bool shared) :
    name(name), specification(specification), iid(IDENTIFIER_COUNTER++)
{
  this->eventHandler = eventHandler;
  this->informationType = informationType;
  this->provider = provider;
  this->description = description;
  this->shared = shared;
  this->_log = Logger::get("InformationStream");
}

BaseInformationStream::~BaseInformationStream()
{
  // currently nothing to do here
}

std::shared_ptr<InformationSpecification> BaseInformationStream::getSpecification() const
{
  return this->specification;
}

const int BaseInformationStream::getIID() const
{
  return this->iid;
}

const std::string BaseInformationStream::getName() const
{
  return this->name;
}

const std::string BaseInformationStream::getProvider() const
{
  return this->provider;
}

void BaseInformationStream::setProvider(std::string provider)
{
  this->provider = provider;
}

const std::string BaseInformationStream::getDescription() const
{
  return this->description;
}

void BaseInformationStream::setDescription(std::string description)
{
  this->description = description;
}

int ice::BaseInformationStream::registerTaskAsync(std::shared_ptr<AsynchronousTask> task)
{
  std::lock_guard<std::mutex> guard(this->_mtx);

  for (auto taskItr : this->taskAsynchronous)
  {
    if (taskItr == task)
      return 1;
  }

  this->taskAsynchronous.push_back(task);

  return 0;
}

int ice::BaseInformationStream::unregisterTaskAsync(std::shared_ptr<AsynchronousTask> task)
{
  std::lock_guard<std::mutex> guard(this->_mtx);

  for (int i = 0; i < this->taskAsynchronous.size(); ++i)
  {
    auto taskItr = this->taskAsynchronous[i];

    if (taskItr == task)
    {
      this->taskAsynchronous.erase(this->taskAsynchronous.begin() + i);
      return 0;
    }
  }

  return 1;
}

int ice::BaseInformationStream::registerTaskSync(std::shared_ptr<AsynchronousTask> task)
{
  std::lock_guard<std::mutex> guard(this->_mtx);

  for (auto taskItr : this->taskSynchronous)
  {
    if (taskItr == task)
      return 1;
  }

  this->taskSynchronous.push_back(task);

  return 0;
}

int ice::BaseInformationStream::unregisterTaskSync(std::shared_ptr<AsynchronousTask> task)
{
  std::lock_guard<std::mutex> guard(this->_mtx);

  for (int i = 0; i < this->taskSynchronous.size(); ++i)
  {
    auto taskItr = this->taskSynchronous[i];

    if (taskItr == task)
    {
      this->taskSynchronous.erase(this->taskSynchronous.begin() + i);
      return 0;
    }
  }

  return 1;
}

std::shared_ptr<StreamDescription> BaseInformationStream::getStreamDescription()
{
  if (this->streamDescription)
    return this->streamDescription;

  std::lock_guard<std::mutex> guard(this->_mtx);

  if (this->streamDescription)
    return this->streamDescription;

  this->streamDescription = std::make_shared<StreamDescription>(this->getSpecification()->getUUID(), this->shared);

  return this->streamDescription;
}

int BaseInformationStream::registerEngineState(std::shared_ptr<EngineState> engineState)
{
  std::lock_guard<std::mutex> guard(this->_mtx);

  for (auto itr : this->remoteListeners)
  {
    if (itr == engineState)
      return 1;
  }

  this->remoteListeners.push_back(engineState);

  return 0;
}

const std::weak_ptr<InformationStreamTemplate> BaseInformationStream::getStreamTemplate() const
{
  return streamTemplate;
}

void BaseInformationStream::setStreamTemplate(const std::weak_ptr<InformationStreamTemplate> streamTemplate)
{
  this->streamTemplate = streamTemplate;
}

int BaseInformationStream::unregisterEngineState(std::shared_ptr<EngineState> engineState)
{
  std::lock_guard<std::mutex> guard(this->_mtx);

  for (int i = 0; i < this->remoteListeners.size(); ++i)
  {
    auto itr = this->remoteListeners[i];

    if (itr == engineState)
    {
      this->remoteListeners.erase(this->remoteListeners.begin() + i);

      this->allEngineStatesUnregistered();

      return 0;
    }
  }

  return 1;
}

void BaseInformationStream::dropReceiver()
{
}

} /* namespace ice */
