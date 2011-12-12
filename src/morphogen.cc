/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#include <iostream>
#include <string>

using namespace std;
//using namespace mgl;


int main(int argc, char *argv[], char *envp[])
{
	if (argc < 2)
	{
		cout << endl;
		cout << "*-------------------------------------*" << endl;
		cout << argv[0]<< endl;
		cout << "It is pitch black. You are likely to be eaten by a grue." << endl;
		cout << "Find the light: try " << argv[0] << " [FILE]" << endl;
		return (-1);
	}
	string modelFile = argv[1];
	cout << "behold!" << endl;
	cout << "Materialization of \"" << modelFile << "\" has begun." << endl;

	cout << "" << endl;
}
