#ifndef PRINTVARIABLE_LOAD
#include <iostream>
#include <vector>
#include "../DITConfig.hpp"
#ifndef WindowsDetermined
#define WindowsDetermined
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define isWindowsBlock
#include "windows.h"
static const bool isWindows = true;
#else
static const bool isWindows = false;
#endif
#endif
#define PRINTVARIABLE_LOAD


namespace DITCameraTool {
	class Display {
	public:
		Display();
		Display(std::string);
		#ifdef isWindowsBlock
		HANDLE debugHandle;
		#endif
		std::string debugMode = "INFO";
		void cprintf(std::string);
		void printVariable(std::string, std::string);
		void printVariable(std::string, int);
		void printVariable(std::string, float);
		void printVairable(std::string, std::vector<std::string>);
		void printVariable(std::string, DITCameraTool::Config);
	private:
		std::string _debugPrintFormat(std::string);
		std::string _debugEndFormat();

	};
}

#define _PrintVariable(variable) DITCameraTool::Display().printVariable((std::string)#variable, (variable))
#endif