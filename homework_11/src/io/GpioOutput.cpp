#include "io/GpioOutput.h"

#include <iostream>
#include <sys/types.h>
#include <unistd.h>

namespace {
constexpr int kUsPerMs = 1000;
}  // namespace

GpioOutput::GpioOutput(const std::string& chipName, int startLine, int dropLine)
{
  chip_ = gpiod_chip_open_by_name(chipName.c_str());  // NOLINT(cppcoreguidelines-prefer-member-initializer)
  if (!chip_) {
    std::cerr << "Failed to open GPIO chip: " << chipName << "\n";
    return;
  }

  start_ = gpiod_chip_get_line(chip_, static_cast<unsigned int>(startLine));
  drop_ = gpiod_chip_get_line(chip_, static_cast<unsigned int>(dropLine));

  if (!start_ || !drop_) {
    std::cerr << "Failed to get GPIO lines: start=" << startLine << ", drop=" << dropLine << "\n";
    return;
  }

  if (gpiod_line_request_output(start_, "gpio_output", 0) < 0 || gpiod_line_request_output(drop_, "gpio_output", 0) < 0) {
    std::cerr << "Failed to request GPIO lines as output\n";
    return;
  }

  ok_ = true;
}

GpioOutput::~GpioOutput()
{
  if (start_) {
    gpiod_line_release(start_);
  }
  if (drop_) {
    gpiod_line_release(drop_);
  }
  if (chip_) {
    gpiod_chip_close(chip_);
  }
}

void GpioOutput::raiseStart()
{
  if (ok_ && start_) {
    gpiod_line_set_value(start_, 1);
  }
}

void GpioOutput::pulseDrop(int durationMs)
{
  if (ok_ && drop_) {
    gpiod_line_set_value(drop_, 1);
    usleep(static_cast<useconds_t>(durationMs) * kUsPerMs);
    gpiod_line_set_value(drop_, 0);
  }
}

bool GpioOutput::isValid() const  // NOLINT(modernize-use-trailing-return-type)
{
  return ok_;
}