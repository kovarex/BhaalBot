#pragma once

template <class T, class NumberType>
NumberType getMapCount(const std::map<T, NumberType>& map, const T& key)
{
  auto position = map.find(key);
  if (position == map.end())
    return 0;
  return position->second;
}
