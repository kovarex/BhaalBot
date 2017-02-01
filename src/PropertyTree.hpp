#pragma once
#include <string>
#include <cstdint>
#include <type_traits>
#include <limits>
#include <utility>
#include <stdexcept>
#include <vector>
#include <ImmutableString.hpp>
#include <vector>

#pragma pack(push, 1)
class PropertyTree
{
public:
  using iterator = std::vector<PropertyTree>::iterator;
  using const_iterator = std::vector<PropertyTree>::const_iterator;

  using StringType = ImmutableStringTemplate<sizeof(std::vector<PropertyTree>)>;
  using KeyType = ImmutableStringTemplate<2 * sizeof(const char *) - 1>;

  enum class Type: uint8_t
  {
    None,
    Bool,
    Number,
    String,
    List,
    Dictionary
  };

  /** Conastruct an empty dictionary */
  static PropertyTree dictionary();

  /** Conastruct an empty list */
  static PropertyTree list();

  PropertyTree();
  ~PropertyTree() noexcept;

  // The forwards below are not enough for these three
  PropertyTree(const PropertyTree& other);
  PropertyTree(PropertyTree&& other) noexcept;
  PropertyTree& operator=(const PropertyTree& other);

  // Constructors and assignment operators are just forwards
  template <typename ... Ts>
  explicit PropertyTree(Ts&& ... values);
  template <typename T>
  PropertyTree& operator=(T&& value);

  /** Clear the property tree to null */
  void clear();

  bool operator==(const PropertyTree& other) const;
  bool operator!=(const PropertyTree& other) const;

  const_iterator begin() const;
  const_iterator end() const;
  iterator begin();
  iterator end();

  const_iterator find(const char* key) const;
  const_iterator find(const std::string& key) const;
  iterator find(const char* key);
  iterator find(const std::string& key);

  /** Return pointer to a node with given key, or nullptr if the key doesn't exist
  If required is true, throws an exception instead of returning nullptr. */
  const PropertyTree* findp(const char* key, bool required = false) const;
  const PropertyTree* findp(const std::string& key, bool required = false) const;
  PropertyTree* findp(const char* key, bool required = false);
  PropertyTree* findp(const std::string& key, bool required = false);

  /** Like findp, but returns nullptr if the property tree is a list (instead of throwing exception) */
  const PropertyTree* findpSafe(const char* key) const;
  const PropertyTree* findpSafe(const std::string& key) const;
  PropertyTree* findpSafe(const char* key);
  PropertyTree* findpSafe(const std::string& key);

  /** Like findp, but returns nullptr if the property tree is a list (instead of throwing exception).
  In addition, this version checks the type of found node and if it's not the specified type,
  returns nullptr anyway. */
  const PropertyTree* findpSafe(const char* key, Type requiredType) const;
  const PropertyTree* findpSafe(const std::string& key, Type requiredType) const;
  PropertyTree* findpSafe(const char* key, Type requiredType);
  PropertyTree* findpSafe(const std::string& key, Type requiredType);
  void merge(PropertyTree& other);

  /** Get value of the current node */
  template <typename T>
  T get() const;

  /** Get value of a child node, returning a default if the key does not exist.
  If the tree is a list, this just returns the default. */
  template <typename T>
  T getDefault(const char* key, T defaultValue) const;
  template <typename T>
  T getDefault(const std::string& key, T defaultValue) const;
  template <typename T>
  std::string getDefault(const char* key, const std::string& defaultValue) const;
  template <typename T>
  std::string getDefault(const std::string& key, const std::string& defaultValue) const;

  /** Append a value  */
  template <typename ... Ts>
  PropertyTree& add(Ts&& ... values);

  /** Return a child at given index. */
  PropertyTree& operator[](size_t i);
  const PropertyTree& operator[](size_t i) const;

  PropertyTree& operator[](int i) { return this->operator[](size_t(i)); };
  const PropertyTree& operator[](int i) const { return this->operator[](size_t(i)); };

  /** Return a child with given key, create it if it doesn't exist. */
  PropertyTree& operator[](const char* path);
  PropertyTree& operator[](const std::string& path);

  /** Return a child with given key, throw if it doesn't exist. */
  const PropertyTree& operator[](const char* key) const;
  const PropertyTree& operator[](const std::string& key) const;

  /** Create item in dictionary, throw if it already exists. */
  template <typename ... Ts>
  PropertyTree& create(const char* path, Ts&& ... values);
  template <typename ... Ts>
  PropertyTree& create(const std::string& path, Ts&& ... values);

  void erase(const char* key);
  void erase(const std::string& key);

  /** Returns true if the property tree is either None, empty dictionary
  or empty list, true for nonempty dictionary or nonempty list, throw exception
  for anything else */
  bool empty() const;

  /** Return number of list or dictionary items under current node */
  size_t size() const;

  /** Reserve at least n space in the underlying vector.
  Requires the ptree to be of type list or dictionary */
  void reserve(size_t n);

  /** Resize the list to the given size, filling the new items with default constructed ptrees.
  Requires ptree to be a list (and casts it if necessary) */
  void resize(size_t n);

  /** Determine if the current node has a child with given key */
  bool hasChild(const char* key) const;
  bool hasChild(const std::string& key) const;

  /** Return type of this node */
  Type getType() const { return Type(this->type); }
  bool isCompatibleType(PropertyTree::Type t) const;

  /** Throw if the property tree is not a list. */
  void requireList() const;
  /** Throw if the property tree is not a dictionary. */
  void requireDict() const;
  /** Convert existing list to dictionary while keeping the item order.
  Converts integer indices to strings as std::to_string(index + indexOffset). */
  void listToDictionary(uint32_t indexOffset = 0);
  /** Return a key if this node is a member of a dictionary or nullptr */
  const KeyType& getKey() const { return this->key; }

  /** Return a path to the current node.
     The path generally cannot be used for addressing nodes (because of list values)
     and should only be used for error messages and the like. */
  std::string getPath() const;

  /** Return true if the type can be freely cast to any other scalar type */
  bool isAnyType() const { return this->anyTypeFlag; }

  /** Return true if the value of this node has been read. */
  bool isUsed() const { return this->usedFlag; }

  /** Mark this node as read without actually reading it. */
  void markAsUsed() const { this->usedFlag = 1; }

  const PropertyTree* getParent() const { return this->parent; }

  /** Debugging function that logs , */
  void logData() const;

  void forceDictionary();
  void forceList();
private:
  void requireListOrDict() const;
  /** Internal functionality of find, but without any checks. */
  iterator findRaw(const char* key);
  void regenerateParentLinks();

  /** Set value of the current node */
  void setInternal(const PropertyTree& value);
  void setInternal(PropertyTree&& value);
  void setInternal(bool value, bool anyType = false);
  void setInternal(double value, bool anyType = false);
  void setInternal(const char* value, bool anyType = false);
  void setInternal(const char* value, size_t len, bool anyType = false);
  void setInternal(const std::string& value, bool anyType = false);
  void setInternal() {}
  template <typename T>
  void setInternal(T value); // Catching numerical types to be converted to double

  union
  {
    bool boolValue;
    double numberValue;
    StringType stringValue;
    std::vector<PropertyTree> listValue;
  };

  KeyType key;

  uint8_t type : 6;
  uint8_t anyTypeFlag: 1;
  mutable uint8_t usedFlag: 1;

  PropertyTree* parent = nullptr;

  static const StringType trueString;
  static const StringType falseString;
  thread_local static StringType numberConversionString;
};
#pragma pack(pop)

class PropertyTreeException : public std::runtime_error
{
public:
  PropertyTreeException(const PropertyTree* ptree, std::string str)
    : std::runtime_error(str + " in property tree at " + ptree->getPath()) {}
  PropertyTreeException(const PropertyTree& ptree, std::string str)
    : PropertyTreeException(&ptree, str) {}
};

class PropertyTreeKeyError : public PropertyTreeException
{
public:
  PropertyTreeKeyError(const PropertyTree* ptree, const char* key)
    : PropertyTreeException(ptree, "Key \"" + std::string(key) + "\" not found") {}
};

class PropertyTreeTypeError : public PropertyTreeException
{
public:
  PropertyTreeTypeError(const PropertyTree* ptree, std::string typeRequired)
    : PropertyTreeException(ptree, "Value must be a " + typeRequired) {}
};

static_assert(sizeof(PropertyTree) % alignof(PropertyTree) == 0,
              "size and alignment requirements of property tree don't match");

template <typename ... Ts>
PropertyTree::PropertyTree(Ts&& ... values)
{
  this->setInternal(std::forward<Ts>(values) ...);
}

template <typename T>
PropertyTree& PropertyTree::operator=(T&& value)
{
  this->clear();
  this->setInternal(std::forward<T>(value));
  return *this;
}

template <typename T>
T PropertyTree::get() const
{
  static_assert(std::is_arithmetic<T>::value, "Unsupported type used");

  double val = this->get<double>();

  using nl = std::numeric_limits<T>;

  if (nl::is_integer && (val < double(nl::min()) || val > double(nl::max())))
    throw PropertyTreeException(this, "Value outside of range");

  return val;
}

template <typename T>
T PropertyTree::getDefault(const char* key, T defaultValue) const
{
  if (auto p = this->findpSafe(key))
    return p->get<T>();
  else
    return defaultValue;
}

template <typename T>
T PropertyTree::getDefault(const std::string& key, T defaultValue) const
{
  return this->getDefault<T>(key.c_str(), defaultValue);
}

template <typename T>
std::string PropertyTree::getDefault(const char* key, const std::string& defaultValue) const
{
  if (auto p = this->findpSafe(key))
    return p->get<T>();
  else
    return defaultValue;
}

template <typename T>
std::string PropertyTree::getDefault(const std::string& key, const std::string& defaultValue) const
{
  return this->getDefault<T>(key.c_str(), defaultValue);
}

template <typename T>
void PropertyTree::setInternal(T value)
{
  static_assert(std::is_arithmetic<T>::value, "Unsupported type assigned");
  this->setInternal(double(value));
}

template <typename ... Ts>
PropertyTree& PropertyTree::add(Ts&& ... values)
{
  this->forceList();
  this->listValue.emplace_back(std::forward<Ts>(values) ...);
  this->listValue.back().parent = this;
  return this->listValue.back();
}

template <typename ... Ts>
PropertyTree& PropertyTree::create(const char* key, Ts&& ... values)
{
  this->forceDictionary();

  iterator it = this->findRaw(key);

  if (it != this->listValue.end())
    throw PropertyTreeException(this, "Duplicate key \"" + std::string(key) + "\"");

  this->listValue.emplace_back(std::forward<Ts>(values) ...);
  PropertyTree& ret = this->listValue.back();
  ret.key = key;
  ret.parent = this;
  return ret;
}

template <typename ... Ts>
PropertyTree& PropertyTree::create(const std::string& key, Ts&& ... values)
{
  return this->create(key.c_str(), std::forward<Ts>(values) ...);
}

template<> bool                            PropertyTree::get<bool>() const;
template<> double                          PropertyTree::get<double>() const;
template<> const PropertyTree::StringType& PropertyTree::get<const PropertyTree::StringType&>() const;
template<> const char*                     PropertyTree::get<const char*>() const;
template<> std::string                     PropertyTree::get<std::string>() const;
