//
// Created by Tom on 1/18/2025.
//

#ifndef CURSE_PROJECT_H
#define CURSE_PROJECT_H


#include <string>

class Project {
public:
    std::string id;          // ID проекта
    std::string title;       // Название проекта
    std::string dbPath;      // Путь до папки с документацией

    Project() = default;

    Project(std::string id, std::string title, std::string dbPath)
            : id(std::move(id)), title(std::move(title)), dbPath(std::move(dbPath)) {}
};



#endif //CURSE_PROJECT_H
