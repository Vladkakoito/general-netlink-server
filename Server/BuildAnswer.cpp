#include "Server/BuildAnswer.h"

#include <charconv>
#include <cstring>
#include <limits>

#include "Common/Actions/Actions.h"
#include "Third-party/SimpleJson/json.hpp"

namespace server {

namespace {

const std::string g_kArgument = "argument_";
constexpr size_t g_kMaxArgsCount = 100;

std::string ParseAndCalculate(const std::string& input) {

  // парвис строку в json
  json::JSON obj = json::JSON::Load(input);

  // действие и аргументы, которые будут доставаться из json
  std::optional<common::actions::TAction> action;
  std::vector<double> args;

  for (const auto& [key, value] : obj.ObjectRange()) {
    if (std::strcmp(key.data(), "action") == 0) {
      action = common::actions::TAction::Make(value.ToString());
      continue;
    }

    // записываем номер аргумента. 
    // не стал уже добавлять проврки на корректность, если что - нулём будет )
    size_t nArg = 0;
    size_t pos = key.find_last_of('_');
    if (pos == std::string::npos || key.size() - 1 <= pos)
      nArg = args.size();
    else
      std::from_chars(key.data() + pos + 1, key.data() + key.size(), nArg);

    // что бы не увеличить случайно массив до миллиарда элементов
    if (nArg > g_kMaxArgsCount)
      return "Argument number too big";

    // аргумент должен начинаться с "argument_"
    if (g_kArgument.compare(0, g_kArgument.size(), key, 0, g_kArgument.size()) == 0) {
      // либо int, либо double. другого библиотека не предоставляет
      bool ok = false;
      double arg = value.ToFloat(ok);
      if (!ok)
        arg = static_cast<double>(value.ToInt(ok));
      if (!ok)
        continue;

      // увеличиваем массив, так что бы аргумент с этим номером влез.
      // хоть в этой бибиотеке и упорядочены ключи (используется map), 
      // но всё-таки это не повод на это надеятся, ведь сам json такой гарантии не даёт
      if (args.size() <= nArg)
        args.resize(nArg + 1, std::numeric_limits<double>::infinity());
      args[nArg] = arg;
    }
  }

  // в jsonе не попалось действие
  if (!action)
    return "No action";

  // проверяем целосность ряда аргументов - если какой-то пропущен, выдаём ошибку
  for (size_t i = 1; i < args.size(); ++ i) {
    if (args[i] == std::numeric_limits<double>::infinity())
      return "Missing argument N" + std::to_string(i);
  }

  // а вот нулевой элемент, можно пропустить. дело вкуса
  // TODO(VF): по хорошему убрать по всему коду infinity и вынести отдельной константой её
  double result = std::numeric_limits<double>::infinity();
  if (args[0] == std::numeric_limits<double>::infinity()) 
    result = action->Calculate(args.data() + 1, args.size() - 1);
  else 
    result = action->Calculate(args.data(), args.size());

  // если деление на ноль, или для действия минус или деление не 2 аргумента, или еще что - infinity
  if (result == std::numeric_limits<double>::infinity())
    return "Error calculating";

  // просто что бы целое число записывалось целым числом
  int isIntResult = static_cast<int>(result);
  // отбрасываем мусорную дробную часть, что бы не захламлять ответ без надобности
  return (result - isIntResult < std::numeric_limits<double>::epsilon()
    ? std::to_string(isIntResult)
    : std::to_string(result));
}

} // namespace 

std::string BuildAnswer(const std::string& input) {
  std::string result = ParseAndCalculate(input);
  json::JSON obj;
  obj["result"] = result;
  return obj.dump();
}

} // namespace server