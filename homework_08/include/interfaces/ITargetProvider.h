#pragma once

#include "Types.h"

class ITargetProvider {
public:
  ITargetProvider() = default;
  ITargetProvider(const ITargetProvider&) = delete;
  ITargetProvider& operator=(const ITargetProvider&) = delete;
  ITargetProvider(ITargetProvider&&) = delete;
  ITargetProvider& operator=(ITargetProvider&&) = delete;
  virtual int getTargetCount() = 0;
  virtual Target getTarget(int index) = 0;
  virtual ~ITargetProvider() = default;
};