#pragma once

#include <owl/dialog.h>
#include <memory>

auto CreateStaticDemoDialog(owl::TWindow* parent) -> std::unique_ptr<owl::TDialog>;
