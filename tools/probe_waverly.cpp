#include <iostream>
#include <filesystem>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
using namespace RailCore;
int main(){
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = std::filesystem::path("Game files/WAVERLY.RCD"); d.name = "WAVERLY";
  Status s = engine->LoadLayout(d);
  std::cout << (int)s.code << " " << s.message << "\n";
  return 0;
}
