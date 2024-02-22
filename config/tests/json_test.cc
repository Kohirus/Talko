#include <config/json.h>
#include <fstream>
#include <iostream>
#include <istream>
using namespace talko;

int main() {
    config::JsonNode root = config::Json::parseFromFile("./test.json");
    return 0;
}