#pragma once
#include <cstdint>
#include <cstring>
#include <string>
#include <stdexcept>
#include <ostream>
#include <cassert>

enum class ImmutableStringFlag: uint8_t
{
  Short = 0, // Data are stored in the shortStr array. This value may also double as short string terminator.
  Owned = 1,
  Borrowed = 2, // We have a pointer, but we don't need to free it. Also used on uninitialised data.
};

#pragma pack(push, 1)
template <size_t N = 2 * sizeof(char*)>
class ImmutableStringTemplate
{
public:
  /** Construct a immutable string that will not free its memory when cleared */
  static ImmutableStringTemplate borrowed(const char* str, uint64_t length);

  ImmutableStringTemplate();

  // The forwards below don't stop the compiler from auto generating copy constructor
  // and copy assignment operator, so we have to define them manually
  ImmutableStringTemplate(const ImmutableStringTemplate& other);
  ImmutableStringTemplate& operator=(const ImmutableStringTemplate& other);

  template <typename ... Args>
  ImmutableStringTemplate(Args&& ... args);
  ~ImmutableStringTemplate();

  template <typename ... Args>
  ImmutableStringTemplate& operator=(Args&& ... args);

  template <typename ... Args>
  void set(Args&& ... args);

  template <size_t N2>
  bool operator==(const ImmutableStringTemplate<N2>& other) const;
  bool operator==(const std::string& other) const;
  bool operator==(const char* other) const;

  template <typename Arg>
  bool operator!=(Arg&& other) const;

  const char* c_str() const;
  uint32_t size() const;
  std::string str() const;

  void clear();
private:
  /** Sets current data to a copy of str, current flag becomes ImmutableStringFlag::Owned.
     Assumes that current ptr can be safely overwritten. */
  void setOwned(const char* str, uint64_t len);

  /** If possible, sets current data to a copy of str and current flag becomes
     ImmutableStringFlag::Short, otherwise returns false.
     Assumes that current ptr can be safely overwritten. */
  bool setShort(const char* str, uint64_t len);

  /** Sets current data to a str, current flag becomes ImmutableStringFlag::Borrowed or ImmutableStringFlag::Short.
     Assumes that current ptr can be safely overwritten. */
  void setBorrowed(const char* str, uint64_t len);

  /** Sets current data to a copy of str, current flag becomes ImmutableStringFlag::Owned or ImmutableStringFlag::Short.
     Assumes that current ptr can be safely overwritten. */
  void setInternal(const char* str, uint64_t len);
  void setInternal(const char* str);
  void setInternal(const std::string& str);
  template <size_t N2>
  void setInternal(const ImmutableStringTemplate<N2>& str);
  template <size_t N2>
  void setInternal(ImmutableStringTemplate<N2>&& str);

  struct LongStr
  {
    const char* ptr;
    uint32_t len;
  };

  union
  {
    LongStr longStr;
    char shortStr[N - 1];
  };
  ImmutableStringFlag flag;

  template <size_t N2>
  friend class ImmutableStringTemplate;
};
#pragma pack(pop)

template <size_t N>
ImmutableStringTemplate<N> ImmutableStringTemplate<N>::borrowed(const char* str, uint64_t length)
{
  ImmutableStringTemplate ret;
  ret.setBorrowed(str, length);
  return ret;
}

template <size_t N>
ImmutableStringTemplate<N>::ImmutableStringTemplate()
  : longStr{nullptr, 0}, flag(ImmutableStringFlag::Borrowed)
{}

template <size_t N>
ImmutableStringTemplate<N>::ImmutableStringTemplate(const ImmutableStringTemplate<N>& other)
{
  this->setInternal(other);
}

template <size_t N>
ImmutableStringTemplate<N>& ImmutableStringTemplate<N>::operator=(const ImmutableStringTemplate<N>& other)
{
  this->clear();
  this->setInternal(other);
  return *this;
}

template <size_t N>
template <typename ... Args>
ImmutableStringTemplate<N>::ImmutableStringTemplate(Args&& ... args)
{
  this->setInternal(std::forward<Args>(args) ...);
}

template <size_t N>
ImmutableStringTemplate<N>::~ImmutableStringTemplate()
{
  this->clear();
}

template <size_t N>
template <typename ... Args>
ImmutableStringTemplate<N>& ImmutableStringTemplate<N>::operator=(Args&& ... args)
{
  this->clear();
  this->setInternal(std::forward<Args>(args) ...);
  return *this;
}

template <size_t N>
template <typename ... Args>
void ImmutableStringTemplate<N>::set(Args&& ... args)
{
  this->clear();
  this->setInternal(std::forward<Args>(args) ...);
}

template <size_t N>
template <size_t N2>
bool ImmutableStringTemplate<N>::operator==(const ImmutableStringTemplate<N2>& other) const
{
  const char* s1 = this->c_str();
  const char* s2 = other.c_str();

  if ((s1 == nullptr) != (s2 == nullptr))
    return false;
  else if (s1 == nullptr)
    return true;
  else if (this->flag == ImmutableStringFlag::Short || other.flag == ImmutableStringFlag::Short)
    return strcmp(s1, s2) == 0;
  else if (this->longStr.len != other.longStr.len)
    return false;
  else
    return memcmp(s1, s2, this->longStr.len) == 0;
}

template <size_t N>
bool ImmutableStringTemplate<N>::operator==(const std::string& other) const
{
  if (this->flag == ImmutableStringFlag::Short)
    return strcmp(this->c_str(), other.c_str()) == 0;
  else if (this->longStr.ptr == nullptr)
    return false; // std::string can never be null
  else if (this->longStr.len != other.size())
    return false;
  else
    return memcmp(this->longStr.ptr, other.c_str(), this->longStr.len) == 0;
}

template <size_t N>
bool ImmutableStringTemplate<N>::operator==(const char* other) const
{
  const char* s1 = this->c_str();
  const char* s2 = other;

  if ((s1 == nullptr) != (s2 == nullptr))
    return false;
  else if (s1 == nullptr)
    return true;
  else
    return strcmp(s1, s2) == 0;
}

template <size_t N>
template <typename Arg>
bool ImmutableStringTemplate<N>::operator!=(Arg&& other) const
{
  return !(*this == std::forward<Arg>(other));
}

template <size_t N>
const char* ImmutableStringTemplate<N>::c_str() const
{
  if (this->flag == ImmutableStringFlag::Short)
    return this->shortStr;
  else
    return this->longStr.ptr;
}

template <size_t N>
uint32_t ImmutableStringTemplate<N>::size() const
{
  if (this->flag == ImmutableStringFlag::Short)
    return uint32_t(strlen(this->shortStr));
  else
    return this->longStr.len;
}

template <size_t N>
std::string ImmutableStringTemplate<N>::str() const
{
  if (this->flag == ImmutableStringFlag::Short)
    return std::string(this->shortStr);
  else if (this->longStr.ptr == nullptr)
    return std::string();
  else
    return std::string(this->longStr.ptr, this->longStr.len);
}

template <size_t N>
void ImmutableStringTemplate<N>::clear()
{
  if (this->flag == ImmutableStringFlag::Owned)
    delete[] this->longStr.ptr;

  this->longStr.ptr = nullptr;
  this->longStr.len = 0;
  this->flag = ImmutableStringFlag::Borrowed;
}

template <size_t N>
void ImmutableStringTemplate<N>::setOwned(const char* str, uint64_t len)
{
  if (len > UINT32_MAX)
    throw std::length_error("Immutable string can only store strings up to UINT32_MAX long");

  if (str == nullptr)
  {
    assert(len == 0);
    this->flag = ImmutableStringFlag::Borrowed;
    this->longStr.ptr = nullptr;
    this->longStr.len = 0;
  }
  else
  {
    this->flag = ImmutableStringFlag::Owned;
    this->longStr.len = uint32_t(len);
    this->longStr.ptr = new char[unsigned int(len + 1)];
    memcpy(const_cast<char*>(this->longStr.ptr), str, std::size_t(len + 1));
    assert(this->longStr.ptr[len] == '\0');
  }
}

template <size_t N>
bool ImmutableStringTemplate<N>::setShort(const char* str, uint64_t len)
{
  if ((len >= N) || (str == nullptr) || (memchr(str, '\0', std::size_t(len)) != nullptr))
    return false;

  strncpy(this->shortStr, str, std::size_t(len));
  if (len < N - 1)
    this->shortStr[len] = '\0';
  this->flag = ImmutableStringFlag::Short;

  return true;
}

template <size_t N>
void ImmutableStringTemplate<N>::setInternal(const char* str, uint64_t len)
{
  if (!this->setShort(str, len))
    this->setOwned(str, len);
}

template <size_t N>
void ImmutableStringTemplate<N>::setInternal(const char* str)
{
  this->setInternal(str, (str == nullptr) ? 0 : strlen(str));
}

template <size_t N>
void ImmutableStringTemplate<N>::setInternal(const std::string& str)
{
  this->setInternal(str.data(), str.size());
}

template <size_t N>
template <size_t N2>
void ImmutableStringTemplate<N>::setInternal(const ImmutableStringTemplate<N2>& other)
{
  if (((void*)this) == ((void*)&other))
    return;

  const char* otherStr = other.c_str();
  uint64_t otherSize = other.size();

  if (this->setShort(otherStr, otherSize))
    return;

  if (other.flag == ImmutableStringFlag::Borrowed)
  {
    this->flag = ImmutableStringFlag::Borrowed;
    this->longStr.len = uint32_t(otherSize);
    this->longStr.ptr = other.longStr.ptr;
  }
  else
    this->setOwned(otherStr, otherSize);
}

template <size_t N>
template <size_t N2>
void ImmutableStringTemplate<N>::setInternal(ImmutableStringTemplate<N2>&& other)
{
  const char* otherStr = other.c_str();
  uint32_t otherSize = uint32_t(other.size());

  if (this->setShort(otherStr, otherSize))
    return;

  if (other.flag == ImmutableStringFlag::Short)
    this->setOwned(otherStr, otherSize);
  else
  {
    this->flag = other.flag;
    this->longStr.len = otherSize;
    this->longStr.ptr = other.longStr.ptr;
    other.longStr.len = 0;
    other.longStr.ptr = nullptr;
    other.flag = ImmutableStringFlag::Borrowed;
  }
}

template <size_t N>
void ImmutableStringTemplate<N>::setBorrowed(const char* str, uint64_t len)
{
  if (this->setShort(str, len))
    return;

  if (len > UINT32_MAX)
    throw std::length_error("Immutable string can only store strings up to UINT32_MAX long");

  this->flag = ImmutableStringFlag::Borrowed;
  this->longStr.ptr = str;
  this->longStr.len = len;
}

template <size_t N>
std::ostream& operator<<(std::ostream& stream, const ImmutableStringTemplate<N>& str)
{
  stream << str.c_str();
  return stream;
}

using ImmutableString = ImmutableStringTemplate<>;
