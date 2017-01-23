#pragma once
#include <iterator>
#include <vector>
#include <PropertyTree.hpp>

/**
 * Helper class that iterates over items of a ptree array if the argument
 * given to it is an array, or over one item collectin containing only the item
 * passed to the constructor, if it is not an array.
 *
 * This class serves both as the iterator and as the collection.
 */
class PtreeItemOrArrayIterator
  : public std::iterator<std::input_iterator_tag, const PropertyTree>
{
public:
  using const_iterator = PtreeItemOrArrayIterator;

  PtreeItemOrArrayIterator(const PropertyTree& input);
  PtreeItemOrArrayIterator(const PropertyTree& input, const std::string& key);
  PtreeItemOrArrayIterator& operator++();
  PtreeItemOrArrayIterator operator++(int32_t);
  bool operator==(PtreeItemOrArrayIterator& other) const;
  bool operator!=(PtreeItemOrArrayIterator& other) const { return !(*this == other); }
  const PropertyTree& operator*() const;
  const PropertyTree* operator->() const { return &(this->operator*()); }

  PtreeItemOrArrayIterator begin() const;
  PtreeItemOrArrayIterator end() const;

private:
  bool isArray() const;
  PtreeItemOrArrayIterator();

  /** Set the current iterator to point to the begining of input */
  void init(const PropertyTree* input);

  /** Set the current iterator to point to the end of input */
  void initEnd(const PropertyTree* input);

  const PropertyTree* generatingPtree;
  bool endFlag;
  PropertyTree::const_iterator it;
};

/** Take contents of a property tree and push it into a vector.
 * if allowSingeItem is true, then this behaves like PtreeItemOrArrayIterator, otherwise
 * the ptree must be a list */
template <typename T>
void loadVectorFromPropertyTree(const PropertyTree& ptree, std::vector<T>& out, bool allowSingeItem = false)
{
  if (allowSingeItem && ptree.getType() != PropertyTree::Type::List)
    out.emplace_back(ptree);
  else
  {
    out.reserve(out.size() + ptree.size());
    for (const PropertyTree& item: ptree)
      out.emplace_back(item);
  }
}

/** Decodes dot separated path in property tree. */
template <typename T>
T& propertyTreeDotPath(T& ptree, const std::string& path);
