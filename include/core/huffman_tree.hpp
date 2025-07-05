#ifndef HUFFMAN_TREE_HPP
#define HUFFMAN_TREE_HPP

#include <cstdint>
#include <map>
#include <memory>
#include <string>

class HuffmanTree {
 private:
  struct Node {
    uint8_t byte;
    uint64_t frequency;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    bool is_leaf;

    Node(uint8_t b, uint64_t freq, bool leaf = false)
        : byte(b), frequency(freq), is_leaf(leaf) {}
  };

  std::unique_ptr<Node> root;

 public:
  HuffmanTree() = default;
  ~HuffmanTree() = default;

  void build_tree(const std::map<uint8_t, uint64_t>& frequencies);
  std::map<uint8_t, std::string> generate_codes() const;
};

#endif
