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

#include <stdlib.h>
#include <boost/filesystem.hpp>

#include "mgl/abstractable.h"
#include "mgl/configuration.h"
#include "mgl/miracle.h"

#include "libthing/Vector2.h"
#include <clpp/parser.hpp>

#include "optionparser.h"


using namespace std;
using namespace mgl;


int intFromCharEqualsStr(const std::string& str)
{
	string nb = str.substr(2, str.length()-2);
	int val = atoi(nb.c_str());
	return val;
}

double doubleFromCharEqualsStr(const std::string& str)
{
	string nb = str.substr(2, str.length()-2);
	double val = atof(nb.c_str());
	return val;
}

class ConfigSetter {
	typedef enum {NONE, INT, STR, DBL, BOOL} configtype;
public:
	ConfigSetter(Configuration &c, const char *s, const char *n):
		config(c), section(s), name(n), set(NONE) {
	};
	void set_s(const string val) {
		sval = val;
		set = STR;
	};
	void set_d(const double val) {
		dval = val;
		set = DBL;
	};
	void set_i(const int val) {
		ival = val;
		set = INT;
	};
	void set_b() {
		set = BOOL;
	};

	~ConfigSetter() {
		if (set == NONE)
			return;

		if (set == INT) {
			config[section][name] = ival;
		}
		else if (set == DBL) {
			config[section][name] = dval;
		}
		else if (set == STR) {
			config[section][name] = sval;
		}
		else if (set == BOOL) {
			config[section][name] = true;
		}
	};
private:
	Configuration &config;
	const char *section;
	const char *name;
	configtype set;
	string sval;
	double dval;
	int ival;
};


struct Arg: public option::Arg
{
  static void printError(const char* msg1, const option::Option& opt, const char* msg2)
  {
    fprintf(stderr, "%s", msg1);
    fwrite(opt.name, opt.namelen, 1, stderr);
    fprintf(stderr, "%s", msg2);
  }

  static option::ArgStatus Unknown(const option::Option& option, bool msg)
  {
    if (msg) printError("Unknown option '", option, "'\n");
    return option::ARG_ILLEGAL;
  }

  static option::ArgStatus Required(const option::Option& option, bool msg)
  {
    if (option.arg != 0)
      return option::ARG_OK;

    if (msg) printError("Option '", option, "' requires an argument\n");
    return option::ARG_ILLEGAL;
  }

  static option::ArgStatus NonEmpty(const option::Option& option, bool msg)
  {
    if (option.arg != 0 && option.arg[0] != 0)
      return option::ARG_OK;

    if (msg) printError("Option '", option, "' requires a non-empty argument\n");
    return option::ARG_ILLEGAL;
  }

  static option::ArgStatus Numeric(const option::Option& option, bool msg)
  {
    char* endptr = 0;
    if (option.arg != 0 && strtol(option.arg, &endptr, 10)){};
    if (endptr != option.arg && *endptr == 0)
      return option::ARG_OK;

    if (msg) printError("Option '", option, "' requires a numeric argument\n");
    return option::ARG_ILLEGAL;
  }
};

enum optionIndex {UNKNOWN, HELP, CONFIG, FIRST_Z,LAYER_H,LAYER_W, FILL_ANGLE, FILL_DENSITY,
				 N_SHELLS, BOTTOM_SLICE_IDX, TOP_SLICE_IDX, DEBUG_ME, START_GCODE,
				END_GCODE, OUT_FILENAME};
const option::Descriptor usageDescriptor[] =
{
 {UNKNOWN, 0, "", "",Arg::None, "miracle-grue [OPTIONS] FILE.STL \n\n"
                                        "Options:" },
 {HELP, 0,"", "help",Arg::None, "  --help  \tPrint usage and exit." },
{CONFIG, 1,"c", "config", Arg::NonEmpty, "-c  \tconfig data in a config.json file."
		 "(default is local miracle.config)" },
{FIRST_Z, 2,"f", "firstLayerZ", Arg::Numeric,
		"-f \tfirst layer height (mm)" },
{LAYER_H, 3,"h", "layerH", Arg::Numeric,
		"  -h \tgeneral layer height(mm)" },
{LAYER_W, 4,"w", "layerW", Arg::Numeric,
		"  -w \tlayer width(mm)" },
{ FILL_ANGLE, 5, "a","angle", Arg::Numeric,
		"  -a \tinfill grid inter slice angle(radians)" },
{ FILL_DENSITY, 6, "d", "density", Arg::Numeric,
		"  -d \tapprox infill density(percent)" },
{ N_SHELLS, 	7, "n", "nShells", Arg::Numeric,
		"  -n \tnumber of shells per layer" },
{ BOTTOM_SLICE_IDX, 8, "b", "bottomIdx", Arg::Numeric,
		"  -b \tbottom slice index" },
{ TOP_SLICE_IDX, 	9, "t", "topIdx", Arg::Numeric,
		"  -t \ttop slice index" },
{ DEBUG_ME, 	10, "d", "debug", Arg::Numeric,
		"  -d \tdebug level" },
{ START_GCODE, 	11, "s", "startGcode", Arg::NonEmpty,
		"  -s \tstart gcode file" },
{ END_GCODE, 	12, "e", "endGcode", Arg::NonEmpty,
		"  -e \tend gcode file" },
{ OUT_FILENAME, 	13, "o", "outFilename", Arg::NonEmpty,
		"  -o \twrite gcode to specific filename (defaults to <model>.gcode" },
{0,0,0,0,0,0},
};




void usage() {
	cout << endl;
	cout << "It is pitch black. You are likely to be eaten by a grue." << endl;
	cout << endl;
	cout << "This program translates a 3d model file in STL format to GCODE toolpath for a " << endl;
	cout << "3D printer." << " Another fine MakerBot Industries product!"<< endl;
	cout << endl;
    option::printUsage(std::cout, usageDescriptor);
	cout << endl;
}

void exitUsage(int code = 0) {
	usage();
	exit(code);
}


int newParseArgs( Configuration &config,
		int argc, char *argv[],
		string &modelFile,
		string &configFilename,
		int &firstSliceIdx,
		int &lastSliceIdx) {

	argc-=(argc>0); argv+=(argc>0); // skip program name argv[0] if present
	option::Stats  stats(usageDescriptor, argc, argv);
	option::Option* options = new option::Option[stats.options_max];
	option::Option* buffer  = new option::Option[stats.buffer_max];
	option::Parser parse(usageDescriptor, argc, argv, options, buffer);

	if (parse.error())
		return -20;

	for (int i = 0; i < parse.optionsCount(); ++i)
	{
		option::Option& opt = buffer[i];
		fprintf(stdout, "Argument #%d name %s is #%s\n", i, opt.desc->longopt, opt.arg );
		switch (opt.index())
		{
			case CONFIG:
				configFilename = string(opt.arg);
				config.readFromFile(configFilename);
				break;
			case  LAYER_H:
			case  LAYER_W:
			case  FILL_ANGLE:
			case  FILL_DENSITY:
			case  N_SHELLS:
			case  BOTTOM_SLICE_IDX:
			case  TOP_SLICE_IDX:
			case  FIRST_Z:
				configFilename = string(opt.arg);
				config["slicer"][opt.desc->longopt] = doubleFromCharEqualsStr(opt.arg);;
				break;
			case  DEBUG_ME:
			case  START_GCODE:
			case  END_GCODE:
			case  OUT_FILENAME:
				configFilename = string(opt.arg);
				config["gcoder"][opt.desc->longopt] = opt.arg;
				break;
			case HELP:
			// not possible, because handled further above and exits the program
			default:
				break;
		}
	}

	/// handle parameters (not options!)
	if ( parse.nonOptionsCount() == 0) {
		usage();
	}
	else if ( parse.nonOptionsCount() != 1) {
		std::cout << "too many parameters" << endl;
		for (int i = 0; i < parse.nonOptionsCount(); ++i)
			std::cout << "Parameter #" << i << ": " << parse.nonOption(i) << "\n";
			exit(-10);
	}
	else {
		//handle the unnamed parameter separately
		modelFile = parse.nonOption(0);
		std::cout << "filename " << modelFile << endl;
		if (!boost::filesystem::is_regular_file(modelFile)) {
			usage();
			throw mgl::Exception(("Invalid model file [" + modelFile + "]").c_str());
			exit(-10);
		}
	}

	//exit(-10);
	return 0;
}

void parseArgs( Configuration &config,
				int argc,
				char *argv[],
				string &modelFile,
				string &, // configFileName,
				int &firstSliceIdx,
				int &lastSliceIdx)
{
	firstSliceIdx = -1;
	lastSliceIdx = -1;

	//first get the config parameter and parse the file so that other params can override the
	//config
	try {
		clpp::command_line_parameters_parser parser;

		parser.add_parameter("-h", "--help", &exitUsage);

		parser.add_parameter("-c", "--config", &config, &Configuration::readFromFile)
			.default_value("miracle.config");

		ConfigSetter f(config, "slicer", "firstLayerZ");
		parser.add_parameter("-f", "--firstLayerZ", &f, &ConfigSetter::set_d);

		ConfigSetter l(config, "slicer", "layerH");
		parser.add_parameter("-l", "--layerH", &l, &ConfigSetter::set_d);

		ConfigSetter w(config, "slicer", "layerW");
		parser.add_parameter("-w", "--layerW", &w, &ConfigSetter::set_d);

		ConfigSetter t(config, "slicer", "tubeSpacing");
		parser.add_parameter("-t", "--tubeSpacing", &t, &ConfigSetter::set_d);

		ConfigSetter a(config, "slicer", "angle");
		parser.add_parameter("-a", "--angle", &a, &ConfigSetter::set_d);

		ConfigSetter s(config, "slicer", "nbOfShells");
		parser.add_parameter("-s", "--nbOfShells", &s, &ConfigSetter::set_d);

		ConfigSetter d(config, "slicer", "writeDebugScadFiles");
		parser.add_parameter("-d", "--writeDebug", &d, &ConfigSetter::set_b);

		ConfigSetter n(config, "slicer", "firstSliceIdx");
		parser.add_parameter("-n", "--firstSliceIdx", &n, &ConfigSetter::set_i);

		ConfigSetter m(config, "slicer", "lastSliceIdx");
		parser.add_parameter("-m", "--lastSliceIdx", &m, &ConfigSetter::set_i)
			.default_value(-1);

		ConfigSetter b(config, "gcoder", "header");
		parser.add_parameter("-b", "--header", &b, &ConfigSetter::set_s);

		ConfigSetter e(config, "gcoder", "footer");
		parser.add_parameter("-e", "--footer", &b, &ConfigSetter::set_s);

		ConfigSetter o(config, "gcoder", "outputFilename");
		parser.add_parameter("-o", "--outputFilename",
							 &o, &ConfigSetter::set_s);

		parser.parse(argc - 1, argv);
	}
	catch (std::exception &exp) {
		usage();
		throw mgl::Exception(exp.what());
	}
	catch (mgl::Exception &exp) {
		usage();
		throw exp;
	}

	firstSliceIdx = config["slicer"]["firstSliceIdx"].asInt();
	lastSliceIdx = config["slicer"]["lastSliceIdx"].asInt();

	//handle the unnamed parameter separately
	modelFile = argv[argc  - 1];
	if (!boost::filesystem::is_regular_file(modelFile)) {
		usage();
		throw mgl::Exception(("Invalid model file [" + modelFile + "]").c_str());
	}
}


// @returns true of preconditions are met
bool preConditions(int argc, char *[]) //char * argv[]
{
	if (argc < 2)
		return false;
	return true;
}



int main(int argc, char *argv[], char *[]) // envp
{

	// design by contract ;-)
	int cmdsOk = preConditions(argc,argv);
	if( false == cmdsOk )
		exitUsage(-1);

	string modelFile;
	string configFileName = "miracle.config";

    Configuration config;
    try
    {
		int firstSliceIdx, lastSliceIdx;

		//parseArgs(config, argc, argv, modelFile, configFileName, firstSliceIdx, lastSliceIdx);
		newParseArgs(config, argc, argv, modelFile, configFileName, firstSliceIdx, lastSliceIdx);

		// cout << config.asJson() << endl;

		cout << "Tube spacing: " << config["slicer"]["tubeSpacing"] << endl;

		MyComputer computer;
		cout << endl;
		cout << endl;
		cout << "behold!" << endl;
		cout << "Materialization of \"" << modelFile << "\" has begun at " << computer.clock.now() << endl;

		std::string scadFile = "."; // outDir
		scadFile += computer.fileSystem.getPathSeparatorCharacter();
		scadFile = computer.fileSystem.ChangeExtension(computer.fileSystem.ExtractFilename(modelFile.c_str()).c_str(), ".scad" );

		
		std::string gcodeFile = config["gcoder"]["outputFilename"].asString();

		if (gcodeFile.empty()) {
			gcodeFile = ".";
			gcodeFile += computer.fileSystem.getPathSeparatorCharacter();
			gcodeFile = computer.fileSystem.ChangeExtension(computer.fileSystem.ExtractFilename(modelFile.c_str()).c_str(), ".gcode" );
		}

		cout << endl << endl;
		cout << modelFile << " to \"" << gcodeFile << "\" and \"" << scadFile << "\"" << endl;

		GCoderConfig gcoderCfg;
		loadGCoderConfigFromFile(config, gcoderCfg);

		SlicerConfig slicerCfg;
		loadSlicerConfigFromFile(config, slicerCfg);

		const char* scad = NULL;

		if (scadFile.size() > 0 )
			scad = scadFile.c_str();

		Tomograph tomograph;
		Regions regions;
		std::vector<mgl::SliceData> slices;

		ProgressLog log;

		miracleGrue(gcoderCfg, slicerCfg, modelFile.c_str(),
					scad,
					gcodeFile.c_str(),
					firstSliceIdx,
					lastSliceIdx,
					tomograph,
					regions,
					slices,
					&log);

    }
    catch(mgl::Exception &mixup)
    {
    	cout << "ERROR: "<< mixup.error << endl;
    	return -1;
    }
}
