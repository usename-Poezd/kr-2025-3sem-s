#ifndef CURSE_PROJECTREPOSITORY_H
#define CURSE_PROJECTREPOSITORY_H

#include <vector>
#include <optional>
#include "../domain/Project.h"

class ProjectRepository {
public:
    virtual ~ProjectRepository() = default;

    // Получить список всех проектов
    virtual std::vector<Project> FindAll() = 0;

    // Найти проект по ID
    virtual std::optional<Project> FindById(const std::string& id) = 0;

    // Создать новый проект
    virtual void Create(Project& project) = 0;

    // Обновить существующий проект
    virtual void Update(Project& project) = 0;

    // Удалить проект по ID
    virtual void Delete(const std::string& id) = 0;
};

#endif // CURSE_PROJECTREPOSITORY_H
