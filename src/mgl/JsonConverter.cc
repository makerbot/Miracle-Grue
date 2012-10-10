#include "log.h"
#include "JsonConverter.h"

using namespace mgl;
using namespace Json;
using namespace std;
using namespace libthing;



bool JsonConverter::loadJsonFromScalar(Value& val,Scalar& s) {
	val = Value(s);
	return true;
}

bool JsonConverter::loadScalarFromJson(Scalar& s, Value& input)
{
	s = input.asDouble();
	return true;
}


bool JsonConverter::loadJsonFromVector2(Value& val, Vector2& input)
{
	val.append(Value(input.x));
	val.append(Value(input.y));
	return false;
}

bool JsonConverter::loadVector2FromJson(Vector2& s, Value& input)
{
	if(input.size() != 2)
		return false;
		s.x = input[0].asDouble();
		s.y = input[1].asDouble();
	return true;
}


bool JsonConverter::loadJsonFromVector3(Value& ,//val,
                                        Point3Type&)// input)
{
	return false;
}


bool JsonConverter::loadJsonFromPolygon(Value& val, Polygon& input)
{
	for(Polygon::iterator i = input.begin(); i < input.end(); i++)
	{
		//const std::pair<Scalar, unsigned int>& seg = *it;
		const Vector2& vec = *i;
		Value t;
		t.append(Value(vec.x));
		t.append(Value(vec.y));
		val.append(t);
	}
	return true;
}

bool JsonConverter::loadPolygonFromJson(Polygon& poly,Value& input)
{
	if(input.size() < 1)
		return false;

        for ( size_t index = 0; index < input.size(); ++index ){
                        Value& tmp = input[(int)index];
			if( tmp.size() == 2) {
				Vector2 vec(tmp[0].asDouble(), tmp[1].asDouble());
				poly.push_back(vec);
			}
			else {
				cerr << "fail in loadPolygonFromJson" << endl;
				throw Exception("polygon miscount");
			}
		}
	return true;

}
bool JsonConverter::loadPolygonsFromJson(Polygons& polys,Value& input)
{
	if(input.size() < 1)
		return false;
        for ( size_t index = 0; index < input.size(); ++index ) {
		Polygon poly;
                Value tmp = input[(int)index];
		bool ok = loadPolygonFromJson(poly,tmp);
		if (ok == false){
			cerr << "Vector2 miscount" << endl;
			throw Exception( "Vector2 miscount");
		}
		polys.push_back( poly );
	}
	return true;
}


bool JsonConverter::loadJsonFromPolygons(Value& val, Polygons& input)
{
	for(Polygons::iterator i = input.begin(); i < input.end(); i++)
	{
		Value polys;
		Polygon& polyList = *i;
		for(Polygon::iterator j = polyList.begin(); j < polyList.end(); j++)
		{
			//const std::pair<Scalar, unsigned int>& seg = *it;
			const Vector2& vec = *j;
			Value t;
			t.append(Value(vec.x));
			t.append(Value(vec.y));
			polys.append(t);
		}
		val.append(polys);
	}
	return true;
}

bool JsonConverter::loadJsonFromPolygonsGroup(Value& val, PolygonsGroup& input)
{
	for(PolygonsGroup::iterator i = input.begin(); i < input.end(); i++)
	{
		Value tmp;
		Polygons& polygons= *i;
		loadJsonFromPolygons(tmp, polygons);
		val.append(tmp);
	}
	return true;
}

bool JsonConverter::loadPolygonsGroupFromJson(PolygonsGroup& pg, Value& input)
{
	if(input.size() < 1)
		return false;
        for ( size_t index = 0; index < input.size(); ++index ) {
                Value group = input[(int)index];
		Polygons polys;
		bool ok = loadPolygonsFromJson(polys,group);
		if (ok == false)
			throw Exception( __FUNCTION__ );
		pg.push_back(polys);
	}
	return true;
}



bool JsonConverter::loadExtruderSliceFromJson(ExtruderSlice& input,Value& val)
{
	Value boundaryValue, infillsValue, insetLoopsListValue;

	try {
		boundaryValue = val["boundary"];
		infillsValue = val["infills"];
		insetLoopsListValue= val["insetLoopsList"];
	}
	catch (...){
			cerr << "loadExtruderSliceFromJson fail:" << endl;
			return false;
	}
	bool loaded =false;
	PolygonsGroup insetLoopsList;
	Polygons infills, boundary;

	loaded = loadPolygonsGroupFromJson(insetLoopsList,insetLoopsListValue );
	if(loaded)
		input.insetLoopsList = insetLoopsList;
	else
        Log::fine() << "loadExtruderSliceFromJson fail a" <<endl;

	loaded = loadPolygonsFromJson(infills,infillsValue );
	if(loaded)
		input.infills= infills;
	else
        Log::fine() << "loadExtruderSliceFromJson fail b" <<endl;


	loaded = loadPolygonsFromJson(boundary,boundaryValue );
	if(loaded)
		input.boundary = boundary;
	else
        Log::fine() << "loadExtruderSliceFromJson fail c" <<endl;

	return false;
}


bool JsonConverter::loadJsonFromExtruderSlice(Value& val, ExtruderSlice& input)
{

	Value boundaryValue, infillsValue, insetLoopsListValue;
	loadJsonFromPolygons(boundaryValue, input.boundary);
	loadJsonFromPolygons(infillsValue, input.infills);
	loadJsonFromPolygonsGroup(insetLoopsListValue, input.insetLoopsList);

	//TODO: verify matching cardanlity of insertLoopsList and boundary
	val["boundary"] = boundaryValue;
	val["infills"] = infillsValue;
	val["insetLoopsList"] = insetLoopsListValue;
	return true;
}


bool JsonConverter::loadJsonFromSliceData(Value& val, SliceData& input)
{
	//val["z"] = input.z;
	//val["index"] = input.sliceIndex;
	Value sliceList;
	for(ExtruderSlices::iterator i = input.extruderSlices.begin(); i < input.extruderSlices.end(); i++)
	{
		ExtruderSlice& slice = *i;
		Value tmp;
		loadJsonFromExtruderSlice(tmp, slice);
		sliceList.append(tmp);
	}
	val["slices"] = sliceList;
	return true;
}

//======================
