#pragma once

#include <string_view>
#include <initializer_list>
#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <memory>
#include <functional>
#include <span>
#include <stdexcept>

namespace wcr {

    template<typename T>
    class TPathTree {
    public:
        
        TPathTree() = default;

        void Insert(std::span<std::string_view> path, T value) {
            if (path.size() == 0)
                throw std::invalid_argument(
                    "TPathTree::Insert: path cannot be empty"
                    );

            FolderNode* current = &root_;

            std::string_view const * pathData = path.data();
            for (std::size_t i = 0; i < path.size(); ++i) {
                const std::string_view segment = pathData[i];
                if (i + 1 == path.size()) {
                    // Last segment: create or replace leaf
                    Node* leafNode = FindChild(*current, segment);
                    if (!leafNode) {
                        auto newNode = std::make_unique<Node>();
                        newNode->name = segment;
                        newNode->data = LeafNode{std::move(value)};
                        current->children.push_back(std::move(newNode));
                    } else {
                        if (!std::holds_alternative<LeafNode>(leafNode->data))
                            throw std::runtime_error(
                                "TPathTree::Insert: segment exists as a folder"
                                );
                        
                        std::get<LeafNode>(leafNode->data).value = std::move(value);
                    }
                } else {
                    Node* folderNode = FindOrCreateChild(*current, segment);
                    if (auto* folder = std::get_if<FolderNode>(&folderNode->data)) {
                        current = folder;
                    } else {
                        throw std::runtime_error(
                            "TPathTree::Insert: segment already a leaf"
                            );
                    }
                }
            }
        }

        void Insert(std::initializer_list<std::string_view> path, T value) {
            Insert(std::span{path}, value);
        }

        std::optional<std::reference_wrapper<T const>>
        Lookup(std::span<std::string_view> path) const {
            if (path.size() == 0)
                return std::nullopt;

            const FolderNode* current = &root_;
            const std::string_view* pathData = path.data();
            for (std::size_t i = 0; i < path.size(); ++i) {
                const std::string_view segment = pathData[i];
                const Node* child = FindChildConst(*current, segment);
                if (!child)
                    return std::nullopt;

                if (i + 1 == path.size()) {
                    // Last segment must be leaf
                    if (!std::holds_alternative<LeafNode>(child->data))
                        return std::nullopt;
                    const LeafNode& leaf = std::get<LeafNode>(child->data);
                    return std::ref(leaf.value);
                } else {
                    // Intermediate segments must be folders
                    if (!std::holds_alternative<FolderNode>(child->data))
                        return std::nullopt;
                    current = &std::get<FolderNode>(child->data);
                }
            }
            return std::nullopt;
        }

        std::optional<std::reference_wrapper<T const>>
        Lookup(std::initializer_list<std::string_view> path) const {
            return Lookup(std::span{path});
        }

        T * MutableLookup(std::span<std::string_view> path) {
            if (path.size() == 0)
                return nullptr;

            FolderNode* current = &root_;
            std::string_view const * pathData = path.data();
            for (std::size_t i = 0; i < path.size(); ++i) {
                const std::string_view segment = pathData[i];
                Node* child = FindChild(*current, segment);
                if (!child)
                    return nullptr;
                if (i + 1 == path.size()) {
                    if (!std::holds_alternative<LeafNode>(child->data))
                        return nullptr;
                    return &std::get<LeafNode>(child->data).value;
                } else {
                    if (!std::holds_alternative<FolderNode>(child->data))
                        return nullptr;
                    current = &std::get<FolderNode>(child->data);
                }
            }
            return nullptr;
        }

        T * MutableLookup(std::initializer_list<std::string_view> path) {
            return MutableLookup(std::span{path});
        }

    private:

        struct Node;

        struct LeafNode {
            T value;
        };

        struct FolderNode {
            std::vector<std::unique_ptr<Node>> children;
        };

        struct Node {
            std::string name;
            std::variant<FolderNode, LeafNode> data;
        };

        FolderNode root_;

        Node* FindChild(FolderNode& folder, std::string_view name) {
            for (auto& child : folder.children) {
                if (child->name == name)
                    return child.get();
            }
            return nullptr;
        }

        const Node* FindChildConst(const FolderNode& folder, std::string_view name) const {
            for (const auto& child : folder.children) {
                if (child->name == name)
                    return child.get();
            }
            return nullptr;
        }

        Node* FindOrCreateChild(FolderNode& folder, std::string_view name) {
            Node* existing = FindChild(folder, name);
            if (existing)
                return existing;
            auto newNode = std::make_unique<Node>();
            newNode->name = name;
            newNode->data = FolderNode{};
            Node* ptr = newNode.get();
            folder.children.push_back(std::move(newNode));
            return ptr;
        }
    };

}

