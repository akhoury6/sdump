#ifndef SETTINGS_H
#define SETTINGS_H

#define MAX_INI_LINE_LENGTH 255
#define MAX_PARAMETERS 8
#define MAX_KEY_LENGTH 10
#define MAX_VALUE_LENGTH 10

class Settings {
	private:
		struct KVPair {
			char key[MAX_KEY_LENGTH];
			char value[6];
		};
		KVPair parameters[MAX_PARAMETERS];
		int parameterCount;
		void trim(char* str);
		void addParameter(const char* key, const char* value);
		void parse_file(const char* filename);
	public:
		Settings(const char* filename);
		const char* getParameter(const char* key);
};

#endif
