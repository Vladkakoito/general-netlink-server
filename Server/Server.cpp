#include <cstring>
#include "signal.h"

#include "Common/StopHandler.h"
#include "Server/Acceptor.h"
#include "Third-party/inih/INIReader.h"

void StopFunction(int) {
  common::TStopHandler::Call();
}

int main(int argc, char* argv[]) {
  signal(SIGINT, StopFunction);

  const std::string titleIni = "-ini=";
  const std::string sectionServer = "Server";

  // ini файл можно указать при запуске
  std::string iniFile = "Server.ini";
  for (int i = 1; i < argc; ++i) {
    if (std::strncmp(titleIni.data(), argv[i], titleIni.length()) == 0) {
      iniFile = argv[i] + titleIni.length();
    }
  }

  // первый попавшийся парсер из интернетов
  INIReader confReader(iniFile);
  if (confReader.ParseError() < 0) {
    MyErr("server: error parsing config file");
    return EXIT_FAILURE;
  }

  // порт сервера указываем при запуске - что бы знать ,куда коннектить клиентов
  uint32_t port = confReader.GetInteger(sectionServer, "port", 0);
  if (port == 0) {
    MyLog("server: port not changed");
    return EXIT_FAILURE;
  }

  // shared ptr передаётся в Stop(который по ctrl+c вызыввается) - вдруг Stop ещё не завершится,
  // а тут основной поток уже уничтожит объект
  std::shared_ptr<server::TAcceptor> acceptor = server::TAcceptor::Make();

  if (!acceptor->Init(port))
    return EXIT_FAILURE;

  acceptor->Start();
  acceptor->Join();

  MyLog("server: end program");

  return EXIT_SUCCESS;
}