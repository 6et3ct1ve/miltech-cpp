#pragma once

class IThreadedComponent {
public:
  IThreadedComponent() = default;
  IThreadedComponent(const IThreadedComponent&) = delete;
  IThreadedComponent& operator=(const IThreadedComponent&) = delete;
  IThreadedComponent(IThreadedComponent&&) = delete;
  IThreadedComponent& operator=(IThreadedComponent&&) = delete;
  virtual ~IThreadedComponent() = default;

  virtual void run() = 0;
  virtual void start() = 0;
  virtual void stop() = 0;
  [[nodiscard]] virtual bool isThreadReady() const = 0;
};