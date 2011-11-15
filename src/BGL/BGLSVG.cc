#include "BGLSVG.h"

namespace BGL {


ostream &SVG::header(ostream &os) const
{
    os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    os << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
    os << "<svg xmlns=\"http://www.w3.org/2000/svg\"";
    os << " xml:space=\"preserve\"";
    os << " style=\"shape-rendering:geometricPrecision; text-rendering:geometricPrecision; image-rendering:optimizeQuality; fill-rule:evenodd; clip-rule:evenodd\"";
    os << " xmlns:xlink=\"http://www.w3.org/1999/xlink\"";
    os << " width=\"" << width << "mm\"";
    os << " height=\"" << height << "mm\"";
    //os << " viewport=\"0 0 " << width << " " << height << "\"";
    os << " stroke=\"black\"";
    os << " stroke-width=\"0.254\"";
    os << " fill=\"none\"";
    os << ">" << endl;
    os << "<style type=\"text/css\">" << endl;
    os << "  path.inset_shell { stroke: black; }" << endl;
    os << "  path.infill_path { stroke: blue; }" << endl;
    os << "  path.model_outline { stroke: red; }" << endl;
    os << "</style>" << endl;
    os << "<g transform=\"scale(3.937)\">" << endl;

    return os;
} 



ostream &SVG::footer(ostream& os) const
{
    os << "</g>" << endl;
    os << "</svg>" << endl;
    return os;
}



}


