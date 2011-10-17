/*
 * gcoder_test_main.cc
 *
 *  Created on: Oct 14, 2011
 *      Author: farmckon
 */
#include "src/GCoderOperation.h"
#include "src/DataEnvelope.h"

Configuration *pConfig  = new Configuration();

Configuration &readFakeConfigFile()
{
	// assert(pConfig == NULL);

	pConfig->machineName = "Special";
	pConfig->firmware = "v2.9";

	Extruder e;
	e.defaultSpeed =1.98;

	pConfig->extruders.push_back(e);
	pConfig->extruders.push_back(e);

	pConfig->extruders[0].extrusionTemperature = 225;
	pConfig->extruders[0].coordinateSystemOffsetX = -16.5;

	pConfig->extruders[1].extrusionTemperature = 210;
	pConfig->extruders[0].coordinateSystemOffsetX = 16.5;

	return *pConfig;
}

int main()
{

	Configuration &cfg = readFakeConfigFile();

	GCoderOperation tooler = GCoderOperation();

	tooler.init(cfg);
	tooler.start();

	DataEnvelope d = DataEnvelope();
	d.typeID =  TYPE_PATH_ASCII;
	d.setLast();
	tooler.collect(d);

	delete pConfig;

//	op.init(gcodeOutStream);
	return -1;
}
