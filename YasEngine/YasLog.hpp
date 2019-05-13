#ifndef YASLOG_HPP
#define YASLOG_HPP
#include"stdafx.hpp"

//-----------------------------------------------------------------------------|---------------------------------------|

template<typename T>
class YasLog
{
	public:
		static void log(std::string prefix, std::string text, T loggedVariable)
		{
			std::cout << prefix << text << loggedVariable << std::endl;
		}

	private:
};

#endif
