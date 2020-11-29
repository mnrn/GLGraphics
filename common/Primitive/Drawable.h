#pragma once

class Drawable {
public:
  virtual ~Drawable() = default;
  virtual void Render() const = 0;
};
