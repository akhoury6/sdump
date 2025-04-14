#include "util.h"
#include <string.h>

unsigned int u_atoi(const char* string) {
	if (strlen(string) == 0) return 0;
	unsigned int result = 0;
	for (int i = (strlen(string) - 1); i >= 0; i--) {
		switch (string[i]) {
			case '0': result += 0; break;
			case '1': result += 1 * ul_pow(10, strlen(string) - 1 - i); break;
			case '2': result += 2 * ul_pow(10, strlen(string) - 1 - i); break;
			case '3': result += 3 * ul_pow(10, strlen(string) - 1 - i); break;
			case '4': result += 4 * ul_pow(10, strlen(string) - 1 - i); break;
			case '5': result += 5 * ul_pow(10, strlen(string) - 1 - i); break;
			case '6': result += 6 * ul_pow(10, strlen(string) - 1 - i); break;
			case '7': result += 7 * ul_pow(10, strlen(string) - 1 - i); break;
			case '8': result += 8 * ul_pow(10, strlen(string) - 1 - i); break;
			case '9': result += 9 * ul_pow(10, strlen(string) - 1 - i); break;
		}
	}
	return result;
}

unsigned long ul_pow(const int base, const unsigned int power) {
	if (power == 0) return 1;
	unsigned long result = base;
	for (unsigned int i = 1; i < power; i++) {
		result *= base;
	}
	return result;
}
