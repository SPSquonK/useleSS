#include "StdAfx.h"
#include "DBCredentials.h"
#include "Query.h"

void Credentials::Load(const rapidjson::Value::ConstObject & jsonObject) {
  lstrcpy(dbName, jsonObject["dbname"].GetString());
  lstrcpy(username, jsonObject["username"].GetString());
  ::GetPWDFromToken(jsonObject["password"].GetString(), password);
}
