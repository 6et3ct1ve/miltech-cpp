#include "io/UartLink.h"
#include "drone_link.h"

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <iostream>
#include <array>
#include <cstring>

namespace {

constexpr float kMsPerSec = 1000.0f;
constexpr float kTwo = 2.0f;
constexpr size_t kControlBufSize = 64;
constexpr size_t kReadBufSize = 256;
constexpr size_t kPayloadBufSize = 260;

DroneTelemetry toDomain(const dlink::Telemetry& t, float acceleration)  // NOLINT(modernize-use-trailing-return-type)
{
  DroneTelemetry out{};
  out.pos = Coord{t.x, t.y};
  out.altitude = t.z;
  out.speed = t.speed;
  out.direction = t.dir;
  out.mode = static_cast<DroneMode>(t.state);
  out.timeSecSinceStart = static_cast<float>(t.t_ms) / kMsPerSec;
  out.acceleration = acceleration;
  return out;
}

AmmoParams toDomain(const dlink::AmmoCfg& cfg)  // NOLINT(modernize-use-trailing-return-type)
{
  AmmoParams out{};
  out.name = std::string(cfg.name, strnlen(cfg.name, sizeof cfg.name));  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
  out.mass = cfg.mass;
  out.drag = cfg.drag;
  out.lift = cfg.lift;
  return out;
}

}  // namespace

UartLink::UartLink(const std::string& device)
{
  // NOLINTNEXTLINE(cppcoreguidelines-prefer-member-initializer,cppcoreguidelines-pro-type-vararg)
  fd_ = open(device.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (fd_ < 0) {
    std::cerr << "Failed to open UART device: " << device << "\n";
    return;
  }

  termios tty{};
  if (tcgetattr(fd_, &tty) != 0) {
    std::cerr << "Failed to get UART attributes" << "\n";
    close(fd_);
    fd_ = -1;
    return;
  }

  cfmakeraw(&tty);
  cfsetispeed(&tty, B115200);
  cfsetospeed(&tty, B115200);
  tty.c_cflag |= (CLOCAL | CREAD);

  if (tcsetattr(fd_, TCSANOW, &tty) != 0) {
    std::cerr << "Failed to set UART attributes" << "\n";
    close(fd_);
    fd_ = -1;
    return;
  }

  ok_ = true;
}

UartLink::~UartLink()
{
  if (fd_ >= 0) {
    close(fd_);
  }
}

bool UartLink::isValid() const  // NOLINT(modernize-use-trailing-return-type)
{
  return ok_;
}

void UartLink::sendControl(float accel, float turnRate)
{
  if (fd_ < 0) {
    return;
  }

  const dlink::Control ctrl{accel, turnRate};
  std::array<uint8_t, kControlBufSize> out{};
  const size_t len = dlink::encode(dlink::PKT_CONTROL, &ctrl, sizeof ctrl, out.data());

  if (write(fd_, out.data(), len) < 0) {
    std::cerr << "Failed to write to UART\n";
  }
}

void UartLink::poll(ILinkHandler& handler)
{
  if (fd_ < 0) {
    return;
  }

  std::array<uint8_t, kReadBufSize> in{};
  const ssize_t n = read(fd_, in.data(), in.size());
  if (n <= 0) {
    return;
  }

  uint8_t type = 0;
  uint8_t len = 0;
  std::array<uint8_t, kPayloadBufSize> payload{};

  for (ssize_t i = 0; i < n; i++) {
    if (!parser_.feed(
          in[static_cast<size_t>(i)], type, payload.data(), len)) {  // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
      continue;
    }
    switch (type) {
      case dlink::PKT_TARGET: {
        if (len != sizeof(dlink::TargetPos)) {
          break;
        }
        dlink::TargetPos tp{};
        std::memcpy(&tp, payload.data(), sizeof tp);
        handler.onTarget(tp.id, Coord{tp.x, tp.y});
        break;
      }
      case dlink::PKT_TELEMETRY: {
        if (len != sizeof(dlink::Telemetry)) {
          break;
        }
        dlink::Telemetry tel{};
        std::memcpy(&tel, payload.data(), sizeof tel);
        handler.onTelemetry(toDomain(tel, acceleration_));
        break;
      }
      case dlink::PKT_AMMO: {
        if (len != sizeof(dlink::AmmoCfg)) {
          break;
        }
        dlink::AmmoCfg ammo{};
        std::memcpy(&ammo, payload.data(), sizeof ammo);
        config_.hitRadius = ammo.hitRadius;
        handler.onAmmo(toDomain(ammo), ammo.hitRadius, ammo.nTargets);
        break;
      }
      case dlink::PKT_CONFIG: {
        if (len != sizeof(dlink::DroneCfg)) {
          break;
        }
        dlink::DroneCfg cfg{};
        std::memcpy(&cfg, payload.data(), sizeof cfg);

        config_.attackSpeed = cfg.attackSpeed;
        config_.accelPath = cfg.accelerationPath;
        config_.angularSpeed = cfg.angularSpeed;
        config_.turnThreshold = cfg.turnThreshold;
        config_.timeStep = cfg.timeStep;

        acceleration_ = (cfg.accelerationPath > 0.0f) ? (cfg.attackSpeed * cfg.attackSpeed) / (kTwo * cfg.accelerationPath) : 0.0f;
        hasConfig_ = true;

        handler.onConfig(config_);
        break;
      }
    }
  }
}