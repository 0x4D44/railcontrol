#pragma once

#include <string>
#include <vector>

struct CliValidationLine {
  bool success{false};
  std::wstring text;
};

struct CliValidationSummary {
  std::vector<CliValidationLine> lines;
  bool anyFailure{false};
};

struct CliValidationOptions {
  bool printId{false};
};

CliValidationSummary EvaluateRcdValidationForTesting(const std::vector<std::wstring>& rawInputs,
                                                     const CliValidationOptions& options = CliValidationOptions{});
