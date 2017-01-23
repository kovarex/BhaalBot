#pragma once
class Base;

class BaseInDangerDetector
{
public:
  BaseInDangerDetector(Base& base);
  ~BaseInDangerDetector();
  void onFrame();

  Base& base;
  double dangerLevel = 0;
};
