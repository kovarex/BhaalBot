#define ENABLE_DEBUG_LOG
#include <PropertyTree.hpp>

#include <utility>
#include <stdexcept>

#include <CStringReadStream.hpp>
#include <StringWriteStream.hpp>
#include <PropertyTreeJson.hpp>
#include <StringUtil.hpp>
#include <Log.hpp>

#include <cstring>
#include <cfloat>

const PropertyTree::StringType PropertyTree::trueString("true");
const PropertyTree::StringType PropertyTree::falseString("false");
thread_local PropertyTree::StringType PropertyTree::numberConversionString;

PropertyTree::PropertyTree()
  : anyTypeFlag(0)
  , usedFlag(0)
{
  this->type = uint8_t(Type::None);
}

PropertyTree PropertyTree::dictionary()
{
  PropertyTree ret;
  ret.forceDictionary();
  return ret;
}

PropertyTree PropertyTree::list()
{
  PropertyTree ret;
  ret.forceList();
  return ret;
}

PropertyTree::PropertyTree(const PropertyTree& other)
{
  this->setInternal(other);
  this->key = other.key;
}

PropertyTree::PropertyTree(PropertyTree&& other) noexcept
{
  this->setInternal(std::move(other));
  this->key = std::move(other.key);
  this->parent = other.parent;
}

PropertyTree& PropertyTree::operator=(const PropertyTree& other)
{
  this->clear();
  this->setInternal(other);
  return *this;
}

PropertyTree::~PropertyTree() noexcept
{
  this->clear();
}

void PropertyTree::clear()
{
  switch (this->getType())
  {
    case Type::None:
    case Type::Bool:
    case Type::Number:
      // Nothing else neeeds to be deleted.
      break;
    case Type::String:
      this->stringValue.~StringType();
      break;
    case Type::List:
    case Type::Dictionary:
      this->listValue.~vector<PropertyTree>();
      break;
  }
  this->type = uint8_t(Type::None);
  this->anyTypeFlag = 0;
  this->usedFlag = 0;
}

template<> bool PropertyTree::get<bool>() const
{
  this->markAsUsed();
  if (this->getType() == Type::Bool)
    return this->boolValue;

  if (this->anyTypeFlag)
    switch (this->getType())
    {
      case Type::None:
        return false;
      case Type::Number:
        return this->numberValue != 0;
      case Type::String:
        if (this->stringValue == trueString)
          return true;
        else if (this->stringValue == falseString)
          return false;
        else
        {
          CStringReadStream stream(this->stringValue.c_str());
          ParseReadState readState(&stream);
          double ret = readRawJsonNumber(readState, false);
          if (readState.read() == ParseReadState::eof)
            return ret != 0;
        }
      default:
        break;
    }

  throw PropertyTreeTypeError(this, "bool");
}

template<> double PropertyTree::get<double>() const
{
  this->markAsUsed();
  if (this->getType() == Type::Number)
    return this->numberValue;

  if (this->anyTypeFlag)
    switch (this->getType())
    {
      case Type::Bool:
        return this->boolValue ? 1 : 0;
      case Type::String:
      {
        CStringReadStream stream(this->stringValue.c_str());
        ParseReadState readState(&stream);
        double ret = readRawJsonNumber(readState, false);
        if (readState.read() == ParseReadState::eof)
          return ret;
      }
      default:
        break;
    }

  throw PropertyTreeTypeError(this, "number");
}

template<> const PropertyTree::StringType& PropertyTree::get<const PropertyTree::StringType&>() const
{
  this->markAsUsed();
  if (this->getType() == Type::String)
    return this->stringValue;
  if (this->anyTypeFlag)
    switch (this->getType())
    {
      case Type::Bool:
        return this->boolValue ? trueString : falseString;
      case Type::Number:
        numberConversionString = ssprintf("%.*g", DBL_DIG, this->numberValue);
        return numberConversionString;
      default:
        break;
    }

  throw PropertyTreeTypeError(this, "string");
}

template<> const char* PropertyTree::get<const char*>() const
{
  return this->get<const StringType&>().c_str();
}

template<> std::string PropertyTree::get<std::string>() const
{
  return this->get<const StringType&>().str();
}

bool PropertyTree::operator==(const PropertyTree& other) const
{
  if (this->type != other.type)
    return false;

  switch (this->getType())
  {
    case Type::None:
      return true;
    case Type::Bool:
      return this->boolValue == other.boolValue;
    case Type::Number:
      return this->numberValue == other.numberValue;
    case Type::String:
      return this->stringValue == other.stringValue;
    case Type::List:
    case Type::Dictionary:
    {
      if (this->listValue.size() != other.listValue.size())
        return false;

      const_iterator thisIt = this->listValue.begin();
      const_iterator otherIt = other.listValue.begin();

      while (thisIt != this->listValue.end())
      {
        if (thisIt->key != otherIt->key)
          return false;

        if (*thisIt != *otherIt)
          return false;

        ++thisIt;
        ++otherIt;
      }
      return true;
    }
    default:
      assert(false);
  }
  return false;
}

bool PropertyTree::operator!=(const PropertyTree& other) const
{
  return !this->operator==(other);
}

void PropertyTree::setInternal(const PropertyTree& other)
{
  this->type = other.type;
  this->anyTypeFlag = other.anyTypeFlag;
  switch (other.getType())
  {
    case Type::None:
      break;
    case Type::Bool:
      this->boolValue = other.boolValue;
      break;
    case Type::Number:
      this->numberValue = other.numberValue;
      break;
    case Type::String:
      new (&this->stringValue)StringType(other.stringValue);
      break;
    case Type::List:
    case Type::Dictionary:
      new (&this->listValue) std::vector<PropertyTree>();
      this->listValue.reserve(other.listValue.size());
      for (const PropertyTree& item: other.listValue)
        this->listValue.emplace_back(item);
      this->regenerateParentLinks();
      break;
  }
}

void PropertyTree::setInternal(PropertyTree&& other)
{
  this->type = other.type;
  this->anyTypeFlag = other.anyTypeFlag;
  switch (other.getType())
  {
    case Type::None:
      break;
    case Type::Bool:
      this->boolValue = other.boolValue;
      break;
    case Type::Number:
      this->numberValue = other.numberValue;
      break;
    case Type::String:
      new (&this->stringValue)StringType(std::move(other.stringValue));
      break;
    case Type::List:
    case Type::Dictionary:
      new (&this->listValue) std::vector<PropertyTree>(std::move(other.listValue));
      this->regenerateParentLinks();
      break;
  }
}

void PropertyTree::setInternal(bool value, bool anyType)
{
  this->type = uint8_t(Type::Bool);
  this->anyTypeFlag = anyType;
  this->boolValue = value;
}

void PropertyTree::setInternal(double value, bool anyType)
{
  this->type = uint8_t(Type::Number);
  this->anyTypeFlag = anyType;
  this->numberValue = value;
}

void PropertyTree::setInternal(const char* value, bool anyType)
{
  this->type = uint8_t(Type::String);
  this->anyTypeFlag = anyType;
  new (&this->stringValue)StringType(value);
}

void PropertyTree::setInternal(const char* value, size_t len, bool anyType)
{
  this->type = uint8_t(Type::String);
  this->anyTypeFlag = anyType;
  new (&this->stringValue)StringType(value, len);
}

void PropertyTree::setInternal(const std::string& value, bool anyType)
{
  this->setInternal(value.c_str(), value.size(), anyType);
}

PropertyTree::iterator PropertyTree::begin()
{
  this->markAsUsed();
  this->requireListOrDict();
  return this->listValue.begin();
}

PropertyTree::iterator PropertyTree::end()
{
  this->requireListOrDict();
  return this->listValue.end();
}

PropertyTree::const_iterator PropertyTree::begin() const
{
  this->markAsUsed();
  this->requireListOrDict();
  return this->listValue.begin();
}

PropertyTree::const_iterator PropertyTree::end() const
{
  this->requireListOrDict();
  return this->listValue.end();
}

PropertyTree::const_iterator PropertyTree::find(const char* key) const
{
  return const_cast<PropertyTree*>(this)->find(key);
}

PropertyTree::const_iterator PropertyTree::find(const std::string& key) const
{
  return this->find(key.c_str());
}

PropertyTree::iterator PropertyTree::find(const char* key)
{
  this->markAsUsed();

  assert(strchr(key, '.') == nullptr);
  this->requireDict();
  return this->findRaw(key);
}

PropertyTree::iterator PropertyTree::findRaw(const char* key)
{
  iterator it = this->listValue.begin();
  while (it != this->listValue.end())
  {
    assert(it->key != nullptr);
    if (it->key == key)
      return it;
    else
      ++it;
  }

  return it;
}

PropertyTree::iterator PropertyTree::find(const std::string& key)
{
  return this->find(key.c_str());
}

PropertyTree* PropertyTree::findp(const char* key, bool required)
{
  auto it = this->find(key);
  if (it != this->listValue.end())
    return &*it;
  else if (required)
    throw PropertyTreeKeyError(this, key);
  else
    return nullptr;
}

PropertyTree* PropertyTree::findp(const std::string& key, bool required)
{
  return this->findp(key.c_str(), required);
}

const PropertyTree* PropertyTree::findp(const char* key, bool required) const
{
  return const_cast<PropertyTree*>(this)->findp(key, required);
}

const PropertyTree* PropertyTree::findp(const std::string& key, bool required) const
{
  return this->findp(key.c_str(), required);
}

PropertyTree* PropertyTree::findpSafe(const char* key, PropertyTree::Type requiredType)
{
  if (this->getType() == Type::None)
    return nullptr;
  PropertyTree* ret = this->findp(key);

  if (ret && ret->getType() == requiredType)
    return ret;
  else
    return nullptr;
}

PropertyTree* PropertyTree::findpSafe(const std::string& key, PropertyTree::Type requiredType)
{
  return this->findpSafe(key.c_str(), requiredType);
}

const PropertyTree* PropertyTree::findpSafe(const char* key, PropertyTree::Type requiredType) const
{
  return const_cast<PropertyTree*>(this)->findpSafe(key, requiredType);
}

const PropertyTree* PropertyTree::findpSafe(const std::string& key, PropertyTree::Type requiredType) const
{
  return this->findpSafe(key.c_str(), requiredType);
}

PropertyTree* PropertyTree::findpSafe(const char* key)
{
  if (this->getType() == Type::List || this->getType() == Type::None)
    return nullptr;

  auto it = this->find(key);
  if (it != this->listValue.end())
    return &*it;
  else
    return nullptr;
}

PropertyTree* PropertyTree::findpSafe(const std::string& key)
{
  return this->findpSafe(key.c_str());
}

const PropertyTree* PropertyTree::findpSafe(const char* key) const
{
  return const_cast<PropertyTree*>(this)->findpSafe(key);
}

const PropertyTree* PropertyTree::findpSafe(const std::string& key) const
{
  return this->findpSafe(key.c_str());
}

void PropertyTree::merge(PropertyTree& other)
{
  if (other.getType() == Type::Dictionary)
  {
    this->forceDictionary();
    for (auto& item: other)
      (*this)[item.getKey().c_str()].merge(item);
  }
  else
    *this = other;
}

PropertyTree& PropertyTree::operator[](size_t i)
{
  this->markAsUsed();
  this->requireListOrDict();
  return this->listValue[i];
}

const PropertyTree& PropertyTree::operator[](size_t i) const
{
  this->markAsUsed();
  this->requireListOrDict();
  return this->listValue[i];
}

PropertyTree& PropertyTree::operator[](const char* key)
{
  this->forceDictionary();

  iterator it = this->find(key);

  if (it == this->listValue.end())
  {
    this->listValue.emplace_back();
    PropertyTree& ret = this->listValue.back();
    ret.key = key;
    ret.parent = this;
    return ret;
  }
  else
    return *it;
}

PropertyTree& PropertyTree::operator[](const std::string& path)
{
  return this->operator[](path.c_str());
}

const PropertyTree& PropertyTree::operator[](const char* key) const
{
  const_iterator it = this->find(key);

  if (it == this->listValue.end())
    throw PropertyTreeKeyError(this, key);
  else
    return *it;
}

const PropertyTree& PropertyTree::operator[](const std::string& key) const
{
  return this->operator[](key.c_str());
}

void PropertyTree::erase(const char* key)
{
  this->requireDict();

  iterator it = this->find(key);
  if (it == this->listValue.end())
    throw PropertyTreeKeyError(this, key);

  assert(it->getKey() == key);

  // Move assignment is not going to work here because it doesn't overwrite the key.
  iterator prev = it++;
  while (it != this->listValue.end())
  {
    prev->setInternal(std::move(*it));
    prev->key = std::move(it->key);
    prev = it++;
  }
  this->listValue.pop_back();
}

void PropertyTree::erase(const std::string& key)
{
  this->erase(key.c_str());
}

void PropertyTree::requireListOrDict() const
{
  if (this->getType() != Type::List && this->getType() != Type::Dictionary)
    throw PropertyTreeTypeError(this, "list or dictionary");
}

void PropertyTree::requireList() const
{
  if (this->getType() != Type::List)
    throw PropertyTreeTypeError(this, "list");
}

void PropertyTree::requireDict() const
{
  if (this->getType() != Type::Dictionary)
    throw PropertyTreeTypeError(this, "dictionary");
}

void PropertyTree::listToDictionary(uint32_t indexOffset)
{
  this->requireList();
  for (size_t i = 0; i < this->listValue.size(); ++i)
    this->listValue[i].key = std::to_string(i + indexOffset);

  this->type = uint8_t(Type::Dictionary);
}

bool PropertyTree::empty() const
{
  this->markAsUsed();
  if (this->getType() == Type::None)
    return true;

  this->requireListOrDict();
  return this->listValue.empty();
}

size_t PropertyTree::size() const
{
  this->markAsUsed();
  this->requireListOrDict();
  return this->listValue.size();
}

void PropertyTree::reserve(size_t n)
{
  this->requireListOrDict();
  this->listValue.reserve(n);
}

void PropertyTree::resize(size_t n)
{
  this->requireList();
  this->listValue.resize(n);
  this->regenerateParentLinks();
}

bool PropertyTree::hasChild(const char* key) const
{
  return this->find(key) != this->listValue.end();
}

bool PropertyTree::hasChild(const std::string& key) const
{
  return this->hasChild(key.c_str());
}

void PropertyTree::forceDictionary()
{
  if (this->getType() == Type::None)
  {
    new (&this->listValue) std::vector<PropertyTree>();
    this->type = uint8_t(Type::Dictionary);
  }
  else if (this->getType() != Type::Dictionary)
    throw PropertyTreeTypeError(this, "dictionary or none");
}

void PropertyTree::forceList()
{
  if (this->getType() == Type::None)
  {
    new (&this->listValue) std::vector<PropertyTree>();
    this->type = uint8_t(Type::List);
  }
  else if (this->getType() != Type::List)
    throw PropertyTreeTypeError(this, "list or none");
}

void PropertyTree::regenerateParentLinks()
{
  assert(this->getType() == Type::List || this->getType() == Type::Dictionary);
  for (PropertyTree& child: this->listValue)
    child.parent = this;
}

std::string PropertyTree::getPath() const
{
  const PropertyTree* pt = this;
  const PropertyTree* parent = this->parent;
  std::string ret;

  while (parent)
  {
    if (parent->getType() == Type::Dictionary)
    {
      assert(pt->key != nullptr);
      ret = "." + pt->key.str() + ret;
    }
    else if (parent->getType() == Type::List)
    {
      for (size_t i = 0; i < parent->listValue.size(); ++i)
        if (&(parent->listValue[i]) == pt)
        {
          ret = ssprintf("[%zu]", i) + ret;
          break;
        }
    }
    else
      assert(false);

    pt = parent;
    parent = pt->parent;
  }

  return "ROOT" + ret;
}

void PropertyTree::logLayout()
{
  LOG_DEBUG_VALUE(sizeof(PropertyTree));
  LOG_DEBUG_VALUE(alignof(PropertyTree));
  LOG_DEBUG_VALUE(offsetof(PropertyTree, numberValue));
  LOG_DEBUG_VALUE(offsetof(PropertyTree, stringValue));
  LOG_DEBUG_VALUE(offsetof(PropertyTree, listValue));
  LOG_DEBUG_VALUE(offsetof(PropertyTree, key));
  //LOG_DEBUG_VALUE(offsetof(PropertyTree, type));
  //LOG_DEBUG_VALUE(offsetof(PropertyTree, parent));
}

void PropertyTree::logData() const
{
  StringWriteStream stream;
  stream.write('\n');
  writeJson(stream, *this, true);
  LOG_DEBUG(stream.str());
}
