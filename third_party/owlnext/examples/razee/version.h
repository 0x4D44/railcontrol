//
// Razee version and build information header.
//

#ifndef VERSION_H_
#define VERSION_H_

#include <owl/version.h>

// Flags
//
#define RAZEE_PRERELEASE 1

// Version Number (major, minor, release, build)
// All other version numbers and strings should be defined in terms of this.
// NB! No leading zeroes in the numbers! (leading zero = octal)
//
#define RAZEE_VERSION (1, 0, 0, OWL_BUILD_REVISION)

// File Version (major, minor, release, build)
// File Version String ("major.minor.release.build")
//
#define RAZEE_APPLY_IMPLEMENTATION(f, args) f##args
#define RAZEE_APPLY(f, args) RAZEE_APPLY_IMPLEMENTATION(f, args)
#define RAZEE_APPLY_VERSION(f) RAZEE_APPLY(f, RAZEE_VERSION)
#define RAZEE_FORMAT_VERSION_4WORD(major, minor, release, build) major, minor, release, build
#define RAZEE_FILEVERSION RAZEE_APPLY_VERSION(RAZEE_FORMAT_VERSION_4WORD)
#define RAZEE_FORMAT_VERSION_STRING(major, minor, release, build) #major "." #minor "." #release "." #build
#define RAZEE_FILEVERSION_STRING RAZEE_APPLY_VERSION(RAZEE_FORMAT_VERSION_STRING)

// Product Version (major, minor, release, build)
// Product Version String ("major.minor.release.build")
//
#define RAZEE_PRODUCTVERSION RAZEE_FILEVERSION
#define RAZEE_PRODUCTVERSION_STRING RAZEE_FILEVERSION_STRING

// Revision information
//
#define RAZEE_BUILD_REVISION OWL_BUILD_REVISION
#define RAZEE_BUILD_REVISION_DATE OWL_BUILD_REVISION_DATE
#define RAZEE_BUILD_MIXED OWL_BUILD_MIXED // If the build is based on mixed revisions of code then 1, else 0.
#define RAZEE_BUILD_DIRTY OWL_BUILD_DIRTY // If the build was using modified source code then 1, else 0.

#endif
