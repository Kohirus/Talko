#include <config/json_expection.h>
#include <config/json_node.h>
#include <iostream> // TODO: delete

namespace talko::config {
const int kNull    = 0;
const int kInteger = 1;
const int kDecimal = 2;
const int kBoolean = 3;
const int kString  = 4;
const int kObject  = 5;
const int kArray   = 6;

JsonNode JsonNode::null() {
    return JsonNode();
}

JsonNode& JsonNode::operator[](const std::string& key) {
    if (data_.index() != kObject) {
        throw JsonParseException("Not object node");
    }
    return std::get<ObjectNode>(data_)[key];
}

const JsonNode& JsonNode::at(const std::string& key) const {
    if (data_.index() != kObject) {
        throw JsonParseException("Not object node");
    }
    return std::get<ObjectNode>(data_).at(key);
}

JsonNode& JsonNode::operator[](size_t index) {
    if (data_.index() != kArray) {
        throw JsonParseException("Not array node");
    }
    return std::get<ArrayNode>(data_)[index];
}

const JsonNode& JsonNode::at(size_t index) const {
    if (data_.index() != kArray) {
        throw JsonParseException("Not array node");
    }
    return std::get<ArrayNode>(data_).at(index);
}

bool JsonNode::isNull() const {
    return data_.index() == kNull;
}

void JsonNode::append(const std::string& key, const JsonNode& node) {
    if (data_.index() == kObject) {
        std::get<ObjectNode>(data_)[key] = node;
    } else if (data_.index() == kNull) {
        data_ = ObjectNode { { key, node } };
    } else {
        throw JsonParseException("Not object node");
    }
}

void JsonNode::append(const JsonNode& node) {
    if (data_.index() == kArray) {
        std::get<ArrayNode>(data_).push_back(node);
    } else if (data_.index() == 0) {
        data_ = ArrayNode { node };
    } else {
        throw JsonParseException("Not array node");
    }
}

size_t JsonNode::count() const {
    if (data_.index() == kObject) {
        return std::get<ObjectNode>(data_).size();
    } else if (data_.index() == kArray) {
        return std::get<ArrayNode>(data_).size();
    } else {
        throw JsonParseException("Not container node");
    }
}

bool JsonNode::has(const std::string& key) {
    if (data_.index() == kObject) {
        return std::get<ObjectNode>(data_).count(key) != 0;
    } else {
        throw JsonParseException("Not object node");
    }
}

void JsonNode::print(int level) {
    for (int i = 0; i < level; ++i) {
        std::cout << "  ";
    }
    switch (data_.index()) {
    case 0:
        std::cout << "Null";
        break;
    case 1:
        std::cout << "Number(int)" << std::endl;
        break;
    case 2:
        std::cout << "Number(double)" << std::endl;
        break;
    case 3:
        std::cout << "Boolean" << std::endl;
        break;
    case 4:
        std::cout << "String" << std::endl;
        break;
    case 5: {
        std::cout << "Object" << std::endl;
        for (auto& item : std::get<ObjectNode>(data_)) {
            item.second.print(level + 1);
        }
    } break;
    case 6: {
        std::cout << "Array" << std::endl;
        for (auto& item : std::get<ArrayNode>(data_)) {
            item.print(level + 1);
        }
    } break;
    default:
        throw JsonParseException("Unknown node type");
    }
}
} // namespace talko::config