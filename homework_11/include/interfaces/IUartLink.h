#pragma once

#include "interfaces/ILinkHandler.h"

class IUartLink {
 public:
  IUartLink() = default;
  IUartLink(const IUartLink&) = delete;
  IUartLink& operator=(const IUartLink&) = delete;
  IUartLink(IUartLink&&) = delete;
  IUartLink& operator=(IUartLink&&) = delete;
  virtual ~IUartLink() = default;

  virtual void poll(ILinkHandler& handler) = 0;
  virtual void sendControl(float accel, float turnRate) = 0;
  [[nodiscard]] virtual bool isValid() const = 0;
};