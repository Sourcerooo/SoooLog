#include <cassert>
#include <cstdlib>
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
  else {
    LOG_WARNING("LogMessage 5 {} {} {}", 42, 14.3, "141.2")
  }
}

int main()
{
  static constexpr auto mTranslation = GetTranslation("LogMessage 1 {} {}");
  //Check if translations are generated at compile time
  static_assert(mTranslation[0] == LanguageTextPair{ "de", "Protokolnachricht 1 {} {}" });
  //Check if Message Nodes are generated pre-main
  assert(nodes.size() == 3);
  GenerateMetaFiles();
  char square[10] = { 'o', '1', '2', '3' };
  srand(static_cast<unsigned int>(time(NULL)));
  auto x = rand() % 3;
  //  DoSomething(0);
  //  DoSomething(1);
  DoSomething(x);

  
 


  return 0;
}
