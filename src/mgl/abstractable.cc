/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

**/
#include "abstractable.h"


using namespace mgl;
using namespace std;




ProgressBar::ProgressBar(unsigned int count)
:total(0), delta(0), progress(0), ticks(0)
{
	reset(count);
	cout << ":";
}


void ProgressBar::reset(unsigned int count)
{
	ticks=0;
	total = count;
	progress = 0;
	delta = count /10;
}


void ProgressBar::tick()
{
	total --;
	ticks ++;
	if (ticks >= delta)
	{
		ticks = 0;
		progress ++;
		cout << " [" << progress * 10<< "%] ";
		cout.flush();

	}
	if (total ==0)
	{
		// ::std::cout << "" << ::std::endl;
		std::cout << myPc.clock.now() << std::endl;
	}
}






