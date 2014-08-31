#pragma once

#define EXIT_ASSERT(exp) if (!(exp)) \
	{\
		printf("error: %s: %d", __FILE__, __LINE__); \
		throw; \
	}

#include <iostream>
#define PRINT std::cout << __FILE__ << ": " << __LINE__ << ": " 

