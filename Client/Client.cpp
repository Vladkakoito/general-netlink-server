#include <cstring>
#include <signal.h>
#include <string>

#include "Client/Config.h"
#include "Client/Controller.h"
#include "Common/SimpleLogger/SimpleLogger.h"
#include "Common/StopHandler.h"
#include "Third-party/inih/INIReader.h"

void StopFunction(int) {
  common::TStopHandler::Call();
}

int main(int argc, char* argv[]) {

  signal(SIGINT, StopFunction);

  const std::string titleIni = "-ini=";
  const std::string sectionClient = "Client";
  
  std::string iniFile = "Client.ini";
  for (int i = 1; i < argc; ++i) {
    if (std::strncmp(titleIni.data(), argv[i], titleIni.length()) == 0) {
      iniFile = argv[i] + titleIni.length();
    }
  }

  // первый попавшийся парсер из интернетов
  INIReader confReader(iniFile);
  client::TConfig config;
  if (confReader.ParseError() < 0) {
    MyErr("client: error parsing .ini file \"" + iniFile + '"');
    return EXIT_FAILURE;
  }
  
  config.serverPort = confReader.GetUnsigned(sectionClient, "ServerPort", 0);
  //config.workersCount = confReader.GetUnsigned(sectionClient, "WorkersCount", 1);   
  config.messagesFile = confReader.Get(sectionClient, "MessagesFile", "Messages.json");
  config.answerDir = confReader.Get(sectionClient, "AnswersDirectory", "Answers");
  
  if (config.serverPort == 0) {
    MyErr("client: server port not changed");
    return EXIT_FAILURE;
  }

  // shared ptr передаётся в Stop(который по ctrl+c вызыввается) - вдруг Stop ещё не завершится,
  // а тут основной поток уже уничтожит объект
  std::shared_ptr<client::TController> controller = client::TController::Make(config);

  if (!controller->Init())
    return EXIT_FAILURE; 

  controller->Start();

  return EXIT_SUCCESS;
}