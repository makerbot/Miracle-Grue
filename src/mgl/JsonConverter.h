/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.
=======
*/

#ifndef JSON_CONVERTER_H_
#define JSON_CONVERTER_H_ (1)

#include "/home/farmckon/dev_makerbot/Miracle-Grue/src/json-cpp/include/json/value.h"

#include "mgl.h"

namespace mgl {

/// Tool to convert mgl objects into and out of their  JSON flat-file using the
/// json-cpp system. This is build as a separate class to avoid deep dependency on
/// json, and to make XML/RabbitMQ or other converters easier to make
class JsonConverter
{
public:

	bool loadJsonFromScalar(Json::Value& val, Scalar& s);
	bool loadJsonFromVector2(Json::Value& val, Vector2& input);
	bool loadJsonFromVector3(Json::Value& val, Vector3& input);
};

}
#endif /*JSON_CONVERTER_H_ */
