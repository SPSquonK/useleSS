#pragma once

#include <rapidjson/document.h>

struct Credentials {
	char dbName[256]   = "";
	char username[256] = "";
	char password[256] = "";

	void Load(const rapidjson::Value::ConstObject & jsonObject);
};
