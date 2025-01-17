//
// Created by Tom on 1/17/2025.
//

#ifndef CURSE_DOCUMENTATION_H
#define CURSE_DOCUMENTATION_H

#include <string>
#include <utility>
#include <vector>

class Documentation {
public:
    std::string id;           // ID документации
    std::string title;        // Название
    std::string description;  // Описание
    std::vector<std::string> tags;  // Теги
    std::string filePath;     // Путь к прикрепленному файлу

    Documentation() = default;
    Documentation(std::string  id,
                  std::string  title,
                  std::string  description,
                  const std::vector<std::string>& tags,
                  std::string  filePath)
            : id(std::move(id)), title(std::move(title)), description(std::move(description)), tags(tags), filePath(std::move(filePath)) {}
};


#endif //CURSE_DOCUMENTATION_H
