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

#include <json/value.h>

#include "slicy.h"
#include "mgl.h"

namespace mgl {

/// Tool to convert mgl objects into and out of their  JSON flat-file using the
/// json-cpp system. This is build as a separate class to avoid deep dependency on
/// json, and to make XML/RabbitMQ or other converters easier to make
class JsonConverter
{
public:

	bool loadJsonFromScalar(Json::Value& val, Scalar& s);
	bool loadJsonFromVector2(Json::Value& val, libthing::Vector2& input);
	bool loadJsonFromVector3(Json::Value& val, libthing::Vector3& input);
	bool loadJsonFromPolygon(Json::Value& val, Polygon& input);
	bool loadJsonFromPolygons(Json::Value& val, Polygons& input);

	bool loadJsonFromPolygonsGroup(Json::Value& val, PolygonsGroup& input);
	bool loadJsonFromExtruderSlice(Json::Value& val, ExtruderSlice& input);
	bool loadJsonFromSliceData(Json::Value& val, SliceData& input);

	bool loadScalarFromJson(Scalar& s, Json::Value& input);
	bool loadVector2FromJson(libthing::Vector2& vector,Json::Value& input);
	bool loadPolygonFromJson(Polygon& poly,Json::Value& input);
	bool loadPolygonsFromJson(Polygons& polys,Json::Value& input);
	bool loadPolygonsGroupFromJson(PolygonsGroup& pg, Json::Value& input);
	bool loadExtruderSliceFromJson(ExtruderSlice& input,Json::Value& val);

};

}
#endif /*JSON_CONVERTER_H_ */
