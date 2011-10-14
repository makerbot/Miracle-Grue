/*
 * gcoder_test_main.cc
 *
 *  Created on: Oct 14, 2011
 *      Author: farmckon
 */
#include "src/GCoderOperation.h"
#include "src/DataEnvelope.h"

int main()
{
	GCoderOperation op = GCoderOperation();
	Configuration cfg = Configuration();
	op.init(cfg);

	DataEnvelope d = DataEnvelope();
	d.setLast();
	op.collect(d);
//	op.init(gcodeOutStream);
	return -1;
}
