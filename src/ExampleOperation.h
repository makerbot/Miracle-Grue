/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#ifndef EXAMPLEOPERATION_H_
#define EXAMPLEOPERATION_H_

#include "Operation.h"
#include "PathData.h"

#include <iostream>
#include <fstream>

#include <string>
#include <assert.h>

#include "GCodeEnvelope.h"

/**
 * Example Operation is a class that simply writes a summary of any passed envelope to cout
 * It is an example of an Operation that is used for Template and Example uses,
 * for developers of other Operations.
 */
class ExampleOperation : public Operation
{

/************** Start of Functions each <NAME_OF>Operation must contain***********************/
protected:
	/**
	 * This is a required function, that returns true of the passed config
	 * can be used to initalize an instance object with some set of valid settings
	 * @param config configuration to check
	 * @return true if the configuration can build a working Operation,  false otherwise
	 */
	bool isValidConfig(Configuration& config) const ;


public:
	/**
	 * Standard Constructor.  Note that an object can be built and exist, but
	 * not yet be configured or initalized. See details in implementation.
	 */
	ExampleOperation();


	/**
	 * Standard Destructor.  This should close streams (if any are open) and
	 * deinitalize the Operation (if it is still initalized). See details in implementation.
	 */
	~ExampleOperation();


	/**
	 * This function returns a global static pointer to a list of Configuration Requirements
	 * See details in implementation
	 * @return global static Json::Value pointer
	 */
	static Json::Value* getStaticConfigRequirements();

	/**
	 * This is the heart of data processing.  This is the core of the envelope accepting system.
	 * See details in implementation
	 * @param envelope reference to a DataEnvelope, or related subclass
	 */
	void processEnvelope(const DataEnvelope& envelope);

	/**
	 * This initalizes an operation with specific settings for processing this stream.
	 * See details in implementation
	 * @param config configuration for this stream
	 * @param outputs A vector of operations that receive output envelopes of data from this object
	 */
	void init(Configuration& config,const std::vector<Operation*> &outputs);

	/**
	 * This un-initalizes an operation.
	 * See details in implementation
	 */
	void deinit();

/************** End of Functions each <NAME_OF>Operation must contain***********************/


/************** Start of Functions custom to this <NAME_OF>Operation ***********************/
private:
	//// An example custom per-Operation function,  a file handle accessor
	std::ostream& stream() const;

	//// An example custom per-Operation member,  a file handle accessor
	std::ofstream *pStream;

/************** End of Functions custom to this <NAME_OF>Operation ***********************/

};




#endif /* EXAMPLEOPERATION_H_ */

