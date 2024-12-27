#include "shinc.system.h"

#ifdef _WIN32

#include <Windows.h>

void	sh_setcp() {
	SetConsoleOutputCP(CP_UTF8);
}

#endif