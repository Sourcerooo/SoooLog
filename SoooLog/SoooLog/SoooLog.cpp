#include <cassert>
#include <cstdlib>
#include <random>

#include "Translation.h"
#include "MetaData.h"
#include "Log.h"

void DoSomething(int i)
{
  if (i == 0) {
    LOG_DEBUG("LogMessage 1 {} {}", 42, 12.2)
  }
  else if (i == 1) {
    LOG_ERROR("LogMessage 2 {} {} {}", 42, 13.5, 421.3)
  }
  else if (i == 2) {
    LOG_ERROR("LogMessage 3", 321)
  }
  else {
    LOG_WARNING("LogMessage 5 {} {} {}", 42, 14.3, std::string{ "132.53" })
  }
  //LOG_DEBUG("Logged after doosomesing")
}

int main()
{
  static constexpr auto mTranslation = GetTranslation(2);
  //Check if translations are generated at compile time
  static_assert(mTranslation[0] == LanguageTextPair{ "de", "Protokolnachricht 1 {} {}" });
  //Check if Message Nodes are generated pre-main 
  assert(!gNodes.empty() && "Metadata nodes were not generated pre-main");

  std::random_device              rd;        // Will be used to obtain a seed for the random number engine
  std::mt19937                    gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution dis(0, 2); // Same distribution as before, but explicit and without bias
  auto const                      x = dis(gen);
  DoSomething(5);
  GenerateMetaFiles();
  for(auto* node : gNodes) {
    std::cout << "Logging Meta. Id: " << node->mMetaData->mMetaData.mId << " Size MetaDescriptors: " << node->mMetaData->mDescriptors.size() << std::endl;
    for (auto const& descriptor : node->mMetaData->mDescriptors) {

      std::cout << "    Index: " << descriptor.index();
      if (descriptor.index() == 0) {
        std::cout << " Type: " << std::get<0>(descriptor).GetType();
      }
      if (descriptor.index() == 1) {
        std::cout << " Type: " << std::get<1>(descriptor).GetType();
      }
      if (descriptor.index() == 2) {
        std::cout << " Type: " << std::get<2>(descriptor).GetType();
      }
      if (descriptor.index() == 3) {
        std::cout << " Type: " << std::get<3>(descriptor).GetType();
      }
      if (descriptor.index() == 4) {
        std::cout << " Type: " << std::get<4>(descriptor).GetType();
      }
    }
    std::cout << std::endl;
  }
  LOG_ERROR("Log after doosomesing")
 
  //static constexpr std::array<int,0> tempDescriptors{  };
  //static constexpr std::span descriptors{ tempDescriptors };
  //static constexpr MetaDataStatement m{ F{}(), descriptors };

  return 0;
}
