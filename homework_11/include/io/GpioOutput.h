#pragma once

#include "interfaces/IGpioOutput.h"

#include <gpiod.h>
#include <string>

class GpioOutput : public IGpioOutput {
public:
  GpioOutput(const std::string& chipName, int startLine, int dropLine);
  GpioOutput(const GpioOutput&) = delete;
  GpioOutput& operator=(const GpioOutput&) = delete;
  GpioOutput(GpioOutput&&) = delete;
  GpioOutput& operator=(GpioOutput&&) = delete;
  ~GpioOutput() override;

  void raiseStart() override;
  void pulseDrop(int durationMs) override;
  [[nodiscard]] bool isValid() const override;

private:
  gpiod_chip* chip_ = nullptr;
  gpiod_line* start_ = nullptr;
  gpiod_line* drop_ = nullptr;
  bool ok_ = false;
};