#pragma once

#include <string>

namespace RailCore {

// Produce a canonical form of an RCD file's bytes for stable ID computation.
// - Converts CRLF to LF
// - Trims trailing whitespace on each line
// - Preserves section/content ordering
std::string CanonicalizeRcdContent(const std::string& raw);

// Compute a stable 64-bit FNV-1a hex ID over schemaTag + "\n" + canonical content.
// This is a placeholder for SHA-256; format is 16 lowercase hex chars.
std::string ComputeRcdIdFromContent(const std::string& canonicalContent,
                                    const char* schemaTag = "rcd:v1");

} // namespace RailCore

