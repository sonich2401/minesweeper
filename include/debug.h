#pragma once



#include <stdio.h>
#include <stdlib.h>


#define DFATAL(...) fprintf(stderr, "FATAL: %s %s %u"", ", __PRETTY_FUNCTION__, __FILE__, __LINE__);fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n");
#define SMART_ASSERT(expr, ...) if(!(expr)){DFATAL(__VA_ARGS__); abort();}
