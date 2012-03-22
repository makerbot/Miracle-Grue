
#include "JsonConverter.h"

using namespace mgl;

bool JsonConverter::loadJsonFromScalar(Json::Value& val,Scalar& s)
{
	val = Json::Value(s);
	return true;
}


bool JsonConverter::loadJsonFromVector2(Json::Value& val, Vector2& input)
{
	val.append(Json::Value(input.x));
	val.append(Json::Value(input.y));
	return false;
}


bool JsonConverter::loadJsonFromVector3(Json::Value& val, Vector3& input)
{
	return false;
}
