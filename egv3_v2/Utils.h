#pragma once

auto ctof = [] (float Value) -> float {
  return (Value * 9.0/5.0) + 32.0;
};

auto getPercent = [] (float Whole, float Percentage) -> float {
  return (Percentage / 100) * Whole;
};

auto SecondsToMillis = [] (float Time) -> unsigned long {
  return (Time * 1000);
};