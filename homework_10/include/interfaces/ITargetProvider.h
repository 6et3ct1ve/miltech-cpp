#pragma once

#include "Types.h"

class ITargetProvider {
public:
  ITargetProvider() = default;
  ITargetProvider(const ITargetProvider&) = delete;
  ITargetProvider& operator=(const ITargetProvider&) = delete;  // NOLINT(modernize-use-trailing-return-type)
  ITargetProvider(ITargetProvider&&) = delete;
  ITargetProvider& operator=(ITargetProvider&&) = delete;  // NOLINT(modernize-use-trailing-return-type)
  virtual int getTargetCount() = 0;                        // NOLINT(modernize-use-trailing-return-type)
  virtual Target getTarget(int index) = 0;                 // NOLINT(modernize-use-trailing-return-type)
  virtual ~ITargetProvider() = default;
};