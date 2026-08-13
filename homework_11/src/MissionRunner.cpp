#include "MissionRunner.h"

MissionRunner::MissionRunner(
  MissionProcessor& mission, DroneController& controller, TargetTracker& tracker, IUartLink& link, IGpioOutput& gpio)
  : mission_(mission)
  , controller_(controller)
  , tracker_(tracker)
  , link_(link)
  , gpio_(gpio)
{
}

void MissionRunner::onTelemetry(const DroneTelemetry& tlm)
{
  lastTime_ = tlm.timeSecSinceStart;

  if (!mission_.isReady()) {
    return;
  }

  const DroneCommand cmd = mission_.step(tlm, tracker_.targets());
  const ControlOutput ctrl = controller_.toControl(cmd);
  link_.sendControl(ctrl.accel, ctrl.turnRate);

  if (!dropped_ && mission_.shouldDrop()) {
    gpio_.pulseDrop(100);
    dropped_ = true;
  }
}

void MissionRunner::onTarget(int id, Coord pos)
{
  tracker_.update(id, pos, lastTime_);
}

void MissionRunner::onAmmo(const AmmoParams& ammo, float hitRadius, int nTargets)
{
  ammo_ = ammo;
  nTargets_ = nTargets;
  config_.hitRadius = hitRadius;
  gotAmmo_ = true;
  tryConfigure();
}

void MissionRunner::onConfig(const DroneConfig& config)
{
  const float hitRadius = config_.hitRadius;
  config_ = config;
  if (config_.hitRadius <= 0.0f) {
    config_.hitRadius = hitRadius;
  }
  gotConfig_ = true;
  tryConfigure();
}

bool MissionRunner::isDropped() const
{
  return dropped_;
}

void MissionRunner::tryConfigure()
{
  if (!gotAmmo_ || !gotConfig_ || mission_.isReady()) {
    return;
  }
  tracker_.resize(nTargets_);
  mission_.configure(config_, ammo_, nTargets_);
  controller_.configure(config_);
}