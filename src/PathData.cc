/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#include "PathData.h"

using namespace std;

PathData::~PathData()
{
	for (Paths::iterator i=paths.begin(); i != paths.end(); i++)
	{
		cout << "deleting polygon" << endl;
		Polygon& poly = *i;
		delete *poly;
	}
}



};

#endif // __PATH_DATA_H_
