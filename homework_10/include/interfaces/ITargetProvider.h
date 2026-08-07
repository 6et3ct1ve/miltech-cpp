#pragma once

#include "Types.h"

class ITargetProvider {
public:
  ITargetProvider() = default;
  ITargetProvider(const ITargetProvider&) = delete;
  ITargetProvider& operator=(const ITargetProvider&) = delete;  // NOLINT(modernize-use-trailing-return-type)
  ITargetProvider(ITargetProvider&&) = delete;
  ITargetProvider& operator=(ITargetProvider&&) = delete;  // NOLINT(modernize-use-trailing-return-type)
  [[nodiscard]] virtual int getTargetCount() const = 0;
  [[nodiscard]] virtual Target getTarget(int index) const = 0;  // NOLINT(modernize-use-trailing-return-type)
  virtual ~ITargetProvider() = default;
};