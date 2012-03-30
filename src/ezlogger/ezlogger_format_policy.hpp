#ifndef EZLOGGER_FORMAT_POLICY_HPP_HEADER_GRD_
#define EZLOGGER_FORMAT_POLICY_HPP_HEADER_GRD_

#include <string>
#include <stdio.h>
#include <time.h>

#include "ezlogger_misc.hpp"

namespace axter
{
	/*! @class ezlogger_format_policy
	@brief This struct defines the get_log_prefix_format function, which 
			is used to format the start of each log line.
	The default ezlogger_format_policy only uses one variable member from
	the ext_data struct (m_severity_level).   A custom format policy could
	take advantage of the additional variables and use them to out put
	more detailed structured data.
	@note
		The get_current_time_str method is not required when creating a
		custom format policy.
	*/
	class ezlogger_format_policy
	{
	public:
		inline static std::string get_log_prefix_format(const char*FileName, 
			int LineNo, const char*FunctionName, ext_data levels_format_usage_data)
		{
			std::string FileNameOnly = FileName;
			size_t pos = FileNameOnly.rfind('\\');
			if (pos != std::string::npos)
			{
				FileNameOnly = FileNameOnly.substr(pos+1);
			}
			char FileNameFormat[999] = {0};
			sprintf(FileNameFormat, "%-32s(%5.5i): , [%-16s], [s_%i] [%s, ", 
				FileNameOnly.c_str(), LineNo, FunctionName, levels_format_usage_data.m_severity_level, get_current_time_str().c_str());
			return FileNameFormat;
		}
	private:
		inline static std::string get_current_time_str()
		{
			time_t t = time(0) ;
			std::string tmp = ctime(&t);
			if (tmp.size()) tmp[tmp.size() -1] = ']';
			return tmp;
		}
	};
}

#endif //EZLOGGER_FORMAT_POLICY_HPP_HEADER_GRD_
