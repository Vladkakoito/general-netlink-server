#pragma once

#include <string>

namespace server {

// получает текстовое задание, считает, выдаёт ответ
std::string BuildAnswer(const std::string& input);

} // namespace server