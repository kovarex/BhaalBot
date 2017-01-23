#include <Base.hpp>
#include <BaseInDangerDetector.hpp>

void Base::onFrame()
{
  if (this->baseInDangerDetector)
    this->baseInDangerDetector->onFrame();
}
