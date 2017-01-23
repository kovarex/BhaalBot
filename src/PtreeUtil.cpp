#include <PtreeUtil.hpp>
#include <StringUtil.hpp>

#include <sstream>

bool PtreeItemOrArrayIterator::isArray() const
{
  return this->generatingPtree->getType() == PropertyTree::Type::List;
}

PtreeItemOrArrayIterator::PtreeItemOrArrayIterator()
  : generatingPtree(nullptr)
{}

PtreeItemOrArrayIterator::PtreeItemOrArrayIterator(const PropertyTree& input)
{
  this->init(&input);
}

PtreeItemOrArrayIterator::PtreeItemOrArrayIterator(const PropertyTree& input,
                                                   const std::string& key)
{
  if (auto ptree = input.findp(key))
    this->init(ptree);
  else
    this->initEnd(&input);
}

void PtreeItemOrArrayIterator::init(const PropertyTree* input)
{
  this->generatingPtree = input;
  this->endFlag = false;
  if (this->isArray())
    this->it = input->begin();
}

void PtreeItemOrArrayIterator::initEnd(const PropertyTree* input)
{
  this->generatingPtree = input;
  this->endFlag = true;
  if (this->isArray())
    this->it = input->end();
}

PtreeItemOrArrayIterator& PtreeItemOrArrayIterator::operator++()
{
  if (this->isArray())
  {
    ++(this->it);
    this->endFlag = this->it == this->generatingPtree->end();
  }
  else
    this->endFlag = true;

  return *this;
}

PtreeItemOrArrayIterator PtreeItemOrArrayIterator::operator++(int32_t)
{
  PtreeItemOrArrayIterator oldValue = *this;
  this->operator++();
  return oldValue;
}

bool PtreeItemOrArrayIterator::operator==(PtreeItemOrArrayIterator& other) const
{
  return this->generatingPtree == other.generatingPtree &&
         this->endFlag == other.endFlag &&
         this->it == other.it;
}

const PropertyTree& PtreeItemOrArrayIterator::operator*() const
{
  if (this->isArray())
    return *(this->it);
  else
    return *(this->generatingPtree);
}

PtreeItemOrArrayIterator::const_iterator PtreeItemOrArrayIterator::begin() const
{
  return *this;
}

PtreeItemOrArrayIterator::const_iterator PtreeItemOrArrayIterator::end() const
{
  PtreeItemOrArrayIterator endIt;
  endIt.initEnd(this->generatingPtree);
  return endIt;
}

template <typename T>
T& propertyTreeDotPath(T& ptree, const std::string& path)
{
  std::string::size_type pos = 0;
  T* current = &ptree;
  while (true)
  {
    std::string::size_type next = path.find('.', pos);
    if (next == std::string::npos)
      return (*current)[path.substr(pos)];
    else
    {
      current = &((*current)[path.substr(pos, next - pos)]);
      pos = next + 1;
    }
  }
}

template const PropertyTree& propertyTreeDotPath<const PropertyTree>(const PropertyTree& ptree, const std::string& path);
template PropertyTree& propertyTreeDotPath<PropertyTree>(PropertyTree& ptree, const std::string& path);
