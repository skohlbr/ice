/*
 * Configuration.h
 *
 *  Created on: May 26, 2014
 *      Author: sni
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <string>

namespace ice
{

class Configuration
{
public:
  Configuration();
  virtual ~Configuration();

  int getEventHandlerBufferSize() const;

  void setEventHandlerBufferSize(int eventHandlerBufferSize);

  int getEventHandlerThreadCount() const;

  void setEventHandlerThreadCount(int eventHandlerThreadCount);

  int getInformationStreamBufferSize() const;

  void setInformationStreamBufferSize(int informationStreamBufferSize);

  long getHeartbeatTimeout() const;

  void setHeartbeatTimeout(long heartbeatTimeout);

  long getCoordinationMessageTimeout() const;

  void setCoordinationMessageTimeout(long coordinationMessageTimeout);

  int getMaxRetryCount() const;

  void setMaxRetryCount(int maxRetryCount);

private:
  int informationStreamBufferSize;

  int eventHandlerThreadCount;
  int eventHandlerBufferSize;

  long heartbeatTimeout;
  long coordinationMessageTimeout;
  int maxRetryCount;

  //Static values
public:
  static std::string INFORMATION_TYPE_NAME_SEPERATOR;
};

} /* namespace ice */

#endif /* CONFIGURATION_H_ */
