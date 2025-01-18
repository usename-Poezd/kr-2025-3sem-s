//
// Created by Tom on 1/18/2025.
//

#ifndef CURSE_PROJECTREPOSITORYIMP_H
#define CURSE_PROJECTREPOSITORYIMP_H


#include "DocumentationRepositoryImp.h"
#include "../domain/Project.h"
#include "ProjectRepository.h"

class ProjectRepositoryImp : public ProjectRepository {
private:
    std::string dbPath;
    std::string projectsPath;

    std::string GetProjectFilePath(const std::string& id) const {
        return projectsPath + "/" + id + ".txt";
    }

    static Project ParseProjectFile(const std::string& id, const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open project file: " + filePath);
        }

        std::string titleLine, dbPathLine;

        // Читаем TITLE
        std::getline(file, titleLine);
        if (titleLine.substr(0, 7) != "#TITLE ") {
            throw std::runtime_error("Invalid title format in project file: " + filePath);
        }
        std::string title = titleLine.substr(7); // Удаляем "#TITLE "

        // Читаем DB_PATH
        std::getline(file, dbPathLine);
        if (dbPathLine.substr(0, 9) != "#DB_PATH ") {
            throw std::runtime_error("Invalid db_path format in project file: " + filePath);
        }
        std::string dbPath = dbPathLine.substr(9); // Удаляем "#DB_PATH "

        return Project{id, title, dbPath};
    }

public:
    explicit ProjectRepositoryImp(const std::string& basePath)
            : dbPath(basePath), projectsPath(basePath + "/projects") {
        if (!fs::exists(dbPath)) fs::create_directory(dbPath);
        if (!fs::exists(projectsPath)) fs::create_directory(projectsPath);
    }

    std::vector<Project> FindAll() override {
        std::vector<Project> projects;
        for (const auto& entry : fs::directory_iterator(projectsPath)) {
            if (entry.path().extension() == ".txt") {
                std::string id = entry.path().stem().string();
                projects.push_back(ParseProjectFile(id, entry.path().string()));
            }
        }
        return projects;
    }

    std::optional<Project> FindById(const std::string& id) {
        std::string filePath = GetProjectFilePath(id);
        if (!fs::exists(filePath)) return std::nullopt;
        return ParseProjectFile(id, filePath);
    }

    void Create(Project& project) {
        std::string projectFilePath = GetProjectFilePath(project.id);
        if (fs::exists(projectFilePath)) {
            throw std::runtime_error("Project with ID already exists: " + project.id);
        }

        // Создаем файл проекта
        std::ofstream file(projectFilePath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to create project file: " + projectFilePath);
        }

        file << "#TITLE " << project.title << "\n"
             << "#DB_PATH " << project.dbPath << "\n";

        file.close();
    }

    void Update(Project& project) {
        std::string projectFilePath = GetProjectFilePath(project.id);
        if (!fs::exists(projectFilePath)) {
            throw std::runtime_error("Project not found: " + project.id);
        }

        // Перезаписываем файл проекта
        std::ofstream file(projectFilePath, std::ios::trunc);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to update project file: " + projectFilePath);
        }

        file << "#TITLE " << project.title << "\n"
             << "#DB_PATH " << project.dbPath << "\n";

        file.close();
    }

    void Delete(const std::string& id) {
        std::string projectFilePath = GetProjectFilePath(id);
        if (fs::exists(projectFilePath)) {
            fs::remove(projectFilePath);
        }
    }
};



#endif //CURSE_PROJECTREPOSITORYIMP_H
