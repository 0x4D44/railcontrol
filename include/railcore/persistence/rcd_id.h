#pragma once

#include <string>

namespace RailCore {

// Produce a canonical form of an RCD file's bytes for stable ID computation.
// - Converts CRLF to LF
// - Trims trailing whitespace on each line
// - Preserves section/content ordering
std::string CanonicalizeRcdContent(const std::string& raw);

// Compute a stable SHA-256 hex ID over schemaTag + "\n" + canonical content.
// Returns 64 lowercase hex characters. Canonicalization must be applied first.
std::string ComputeRcdIdFromContent(const std::string& canonicalContent,
                                    const char* schemaTag = "rcd:v1");

} // namespace RailCore
