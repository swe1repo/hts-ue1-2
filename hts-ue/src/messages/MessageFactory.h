/*
 * MessageFactory.h
 *
 *  Created on: Oct 1, 2011
 *      Author: patrick
 */

#ifndef MESSAGEFACTORY_H_
#define MESSAGEFACTORY_H_

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "Messages.h"
#include "../foreach.h"

class MessageFactory
{
public:
	MessageFactory();
	Message* newInstance(const std::vector< boost::shared_ptr<std::string> >& data);

	// utility methods
	Message::MessageType getMessageType(std::string line);
	boost::shared_ptr<std::string> dataToString(const std::vector< boost::shared_ptr<std::string> >& data);
};

#endif /* MESSAGEFACTORY_H_ */
