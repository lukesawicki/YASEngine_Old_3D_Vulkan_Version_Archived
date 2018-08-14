#ifndef YASLOG_HPP
#define YASLOG_HPP
#include"stdafx.hpp"

//-----------------------------------------------------------------------------|---------------------------------------|

template<typename T>
class YasLog {
	public:
		static void log(std::string prefix, std::string additionalText, T loggedVariable) {
			std::cout << prefix << additionalText << loggedVariable << std::endl;
		}
	private:

};

#endif