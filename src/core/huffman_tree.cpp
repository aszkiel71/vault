#include "core/huffman_tree.hpp"

#include <algorithm>
#include <functional>
#include <queue>
#include <stdexcept>
#include <vector>

#include "core/bit_stream.hpp"

void HuffmanTree::build_tree(const std::map<uint8_t, uint64_t>& frequencies) {
  if (frequencies.empty()) {
    throw std::invalid_argument("Frequencies map cannot be empty");
  }

  auto cmp = [](const std::unique_ptr<Node>& a,
                const std::unique_ptr<Node>& b) {
    return a->frequency > b->frequency;
  };
  std::priority_queue<std::unique_ptr<Node>, std::vector<std::unique_ptr<Node>>,
                      decltype(cmp)>
      min_heap(cmp);

  // Create leaf nodes for each byte and add them to the priority queue
  for (const auto& pair : frequencies) {
    min_heap.push(std::make_unique<Node>(pair.first, pair.second, true));
  }

  // Build the Huffman tree
  while (min_heap.size() > 1) {
    // Move from the top of the priority queue
    auto left = std::move(const_cast<std::unique_ptr<Node>&>(min_heap.top()));
    min_heap.pop();
    auto right = std::move(const_cast<std::unique_ptr<Node>&>(min_heap.top()));
    min_heap.pop();

    auto parent = std::make_unique<Node>(0, left->frequency + right->frequency);
    parent->left = std::move(left);
    parent->right = std::move(right);
    parent->is_leaf = false;

    min_heap.push(std::move(parent));
  }

  // Handle the final node(s) in the priority queue
  if (min_heap.size() == 1) {
    // Get the remaining node (could be a leaf or internal node)
    auto remaining_node =
        std::move(const_cast<std::unique_ptr<Node>&>(min_heap.top()));
    min_heap.pop();

    // Check if this is a single leaf node (edge case)
    if (remaining_node->is_leaf) {
      // Special case: only one unique character in the file
      // Create a dummy root with the leaf as left child
      auto dummy_root = std::make_unique<Node>(0, remaining_node->frequency);
      dummy_root->left = std::move(remaining_node);
      dummy_root->right = nullptr;
      dummy_root->is_leaf = false;
      root = std::move(dummy_root);
    } else {
      // This is the normal case - the root of a proper tree
      root = std::move(remaining_node);
    }
  } else {
    // This should never happen in normal flow
    throw std::runtime_error(
        "Unexpected state: priority queue should have exactly one element");
  }
}

std::map<uint8_t, std::string> HuffmanTree::generate_codes() const {
  std::map<uint8_t, std::string> codes;

  if (!root) {
    return codes;  // Empty tree
  }

  // Special case: single character (root has only left child)
  if (root->left && !root->right && root->left->is_leaf) {
    codes[root->left->byte] = "0";  // Assign single bit code
    return codes;
  }

  std::function<void(const Node*, const std::string&)> generate_codes_helper;

  generate_codes_helper = [&](const Node* node, const std::string& code) {
    if (!node) return;

    if (node->is_leaf) {
      codes[node->byte] = code;
      return;
    }

    generate_codes_helper(node->left.get(), code + "0");
    generate_codes_helper(node->right.get(), code + "1");
  };

  generate_codes_helper(root.get(), "");
  return codes;
}

uint8_t HuffmanTree::decode_byte(BitStream& bit_stream) const {
  if (!root) {
    throw std::runtime_error("HuffmanTree: Tree is empty");
  }

  const Node* current = root.get();

  // Handle single character case (dummy root with only left child)
  if (current->left && !current->right && current->left->is_leaf) {
    // Read one bit (should be 0 for single character)
    bit_stream.read_bit();
    return current->left->byte;
  }

  // Traverse the tree following the bits
  while (!current->is_leaf) {
    bool bit = bit_stream.read_bit();
    if (bit) {
      current = current->right.get();
    } else {
      current = current->left.get();
    }

    if (!current) {
      throw std::runtime_error("HuffmanTree: Invalid code sequence");
    }
  }

  return current->byte;
}
