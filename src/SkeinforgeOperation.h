

#include "Operation.h"


class SkeinforgeFromFile : public Operation {

private:
	DataEnvelope& runSkeinforge()
	{
		return new DataEnvelope();
	}

public :
	SkeinforgeFromFile(): Operation();

	void init(Configuration& config)
	{
		//
	}

	DataEnvelope* processEnvelope(const DataEnvelope& envelope)
	{
		if( this->isFirstEnvelope(envelope)) {
			//run skeinforge at the commandline
			// return the skeinforge data
			//DataEnvelope newEnvelope = runSkeinforge()
			//return DataEnvelope(newEnvelope)
		}

		return 0x00;
	}

	void cleanup()
	{

	}

};
