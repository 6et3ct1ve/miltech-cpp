#pragma once

#include "Types.h"
#include "drone_link.h"
#include "interfaces/IUartLink.h"
#include "interfaces/ILinkHandler.h"

#include <string>

class UartLink : public IUartLink {
public:
  UartLink(const std::string& device);
  UartLink(const UartLink&) = delete;
  UartLink& operator=(const UartLink&) = delete;
  UartLink(UartLink&&) = delete;
  UartLink& operator=(UartLink&&) = delete;
  ~UartLink();

  void poll(ILinkHandler& handler) override;
  void sendControl(float accel, float turnRate) override;
  [[nodiscard]] bool isValid() const override;

private:
  int fd_ = -1;
  dlink::Parser parser_;
  DroneConfig config_{};
  float acceleration_ = 0.0f;
  bool hasConfig_ = false;
  bool ok_ = false;
};