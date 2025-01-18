//
// Created by Tom on 1/18/2025.
//

#ifndef CURSE_PROJECTSERVICE_H
#define CURSE_PROJECTSERVICE_H

#include <string>
#include <vector>
#include <optional>
#include "../repositories/ProjectRepository.h"

class ProjectService {
private:
    ProjectRepository& projectRepository;

public:
    explicit ProjectService(ProjectRepository& repository)
            : projectRepository(repository) {}

    // Получить все проекты
    std::vector<Project> GetAllProjects() {
        return projectRepository.FindAll();
    }

    // Найти проект по ID
    std::optional<Project> GetProjectById(const std::string& id) {
        return projectRepository.FindById(id);
    }

    // Создать новый проект
    void CreateProject(Project& project) {
        projectRepository.Create(project);
    }

    // Обновить существующий проект
    void UpdateProject(Project& project) {
        projectRepository.Update(project);
    }

    // Удалить проект по ID
    void DeleteProject(const std::string& id) {
        projectRepository.Delete(id);
    }
};

#endif // CURSE_PROJECTSERVICE_H
