

#include "Operation.h"


class SkeinforgeFromFile : public Operation {

private:
	DataEnvelope& runSkeinforge()
	{
		return new DataEnvelope();
	}

public :
	SkeinforgeFromFile(): Operation();



	void processEnvelope(const DataEnvelope& envelope)
	{
		if( this->isFirstEnvelope(envelope)) {
			//run skeinforge at the commandline
			// return the skeinforge data
			//DataEnvelope newEnvelope = runSkeinforge()
			//return DataEnvelope(newEnvelope)
		}

	}

	void cleanup()
	{

	}

};
