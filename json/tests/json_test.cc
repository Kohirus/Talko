#include <json/json.h>
#include <fstream>
#include <iostream>
#include <istream>
using namespace talko;

int main() {
    // json::JsonNode root = json::Json::parseFromFile("./test.json");
    json::JsonNode root = json::Json::parseFromFile("config.json");
    return 0;
}