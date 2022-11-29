#pragma once
#include <string>
#include <vector>
#include <memory>

enum class NodeType
{
    ID,
    FN,
    COMPOUND,
    NOOP
};

struct Node
{
    NodeType type{ NodeType::NOOP };

    Node() = default;
    Node(NodeType type)
        : type(type) {}

    // ID
    std::string id;

    // Function
    std::string fn_name;
    std::vector<std::unique_ptr<Node>> fn_args;

    // Compound
    std::vector<std::unique_ptr<Node>> comp_values;
};

