#ifndef EXAMPLE__LATENCY_STATS_HPP_
#define EXAMPLE__LATENCY_STATS_HPP_

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <string>
#include <vector>

namespace example
{

struct LatencyStats
{
  size_t count{0};
  double mean_us{0.0};
  double p50_us{0.0};
  double p95_us{0.0};
  double p99_us{0.0};
  double max_us{0.0};
};

inline LatencyStats compute_latency_stats_us(std::vector<double> samples_us)
{
  LatencyStats s;
  s.count = samples_us.size();
  if (samples_us.empty()) {
    return s;
  }
  std::sort(samples_us.begin(), samples_us.end());
  auto pct = [&](double p) -> double {
    const double idx = p * (samples_us.size() - 1);
    const auto lo = static_cast<size_t>(idx);
    const auto hi = std::min(lo + 1, samples_us.size() - 1);
    const double w = idx - static_cast<double>(lo);
    return samples_us[lo] * (1.0 - w) + samples_us[hi] * w;
  };
  s.p50_us = pct(0.50);
  s.p95_us = pct(0.95);
  s.p99_us = pct(0.99);
  s.max_us = samples_us.back();
  const double sum = std::accumulate(samples_us.begin(), samples_us.end(), 0.0);
  s.mean_us = sum / static_cast<double>(samples_us.size());
  return s;
}

}  // namespace example

#endif  // EXAMPLE__LATENCY_STATS_HPP_
