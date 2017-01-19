#include <Cost.hpp>
#include <StringUtil.hpp>

std::string Cost::str() const
{
  return ssprintf("%u/%u", this->minerals, this->gas);
}
