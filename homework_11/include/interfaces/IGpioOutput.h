#pragma once

constexpr int kDefaultDropPulseMs = 80;

class IGpioOutput {
 public:
  IGpioOutput() = default;
  IGpioOutput(const IGpioOutput&) = delete;
  IGpioOutput& operator=(const IGpioOutput&) = delete;
  IGpioOutput(IGpioOutput&&) = delete;
  IGpioOutput& operator=(IGpioOutput&&) = delete;
  virtual ~IGpioOutput() = default;

  virtual void raiseStart() = 0;
  virtual void pulseDrop(int durationMs = kDefaultDropPulseMs) = 0;
  [[nodiscard]] virtual bool isValid() const = 0;
};