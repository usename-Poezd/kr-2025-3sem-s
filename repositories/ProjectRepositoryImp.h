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
    // Путь к базе данных
    std::string dbPath;

    // Путь к папке с проектами
    std::string projectsPath;

    // Формирует путь к файлу проекта по его ID
    std::string GetProjectFilePath(const std::string& id) const {
        return projectsPath + "/" + id + ".txt";
    }

    // Получает данные из файла проекта и создает объект Project
    static Project ParseProjectFile(const std::string& id, const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open project file: " + filePath);
        }

        std::string titleLine, dbPathLine;

        // Чтение строки с названием проекта
        std::getline(file, titleLine);
        if (titleLine.substr(0, 7) != "#TITLE ") {
            throw std::runtime_error("Invalid title format in project file: " + filePath);
        }
        std::string title = titleLine.substr(7); // Убираем префикс "#TITLE "

        // Чтение строки с путем к базе данных
        std::getline(file, dbPathLine);
        if (dbPathLine.substr(0, 9) != "#DB_PATH ") {
            throw std::runtime_error("Invalid db_path format in project file: " + filePath);
        }
        std::string dbPath = dbPathLine.substr(9); // Убираем префикс "#DB_PATH "

        return Project{id, title, dbPath};
    }

public:
    // Конструктор, инициализирующий пути к базе данных и проектам
    explicit ProjectRepositoryImp(const std::string& basePath)
            : dbPath(basePath), projectsPath(basePath + "/projects") {
        // Если директория для базы данных не существует, она создается
        if (!fs::exists(dbPath)) fs::create_directory(dbPath);
        // Если директория для проектов не существует, она создается
        if (!fs::exists(projectsPath)) fs::create_directory(projectsPath);
    }

    // Находит все проекты в папке с проектами
    std::vector<Project> FindAll() override {
        std::vector<Project> projects;
        for (const auto& entry : fs::directory_iterator(projectsPath)) {
            // Если файл имеет расширение .txt, это проект
            if (entry.path().extension() == ".txt") {
                std::string id = entry.path().stem().string();
                projects.push_back(ParseProjectFile(id, entry.path().string()));
            }
        }
        return projects;
    }

    // Находит проект по его ID, если такой существует
    std::optional<Project> FindById(const std::string& id) {
        std::string filePath = GetProjectFilePath(id);
        if (!fs::exists(filePath)) return std::nullopt;
        return ParseProjectFile(id, filePath);
    }

    // Создает новый проект, записывая данные в файл
    void Create(Project& project) {
        std::string projectFilePath = GetProjectFilePath(project.id);
        if (fs::exists(projectFilePath)) {
            throw std::runtime_error("Project with ID already exists: " + project.id);
        }

        // Создаем новый файл проекта
        std::ofstream file(projectFilePath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to create project file: " + projectFilePath);
        }

        // Записываем информацию о проекте в файл
        file << "#TITLE " << project.title << "\n"
             << "#DB_PATH " << project.dbPath << "\n";

        file.close();
    }

    // Обновляет проект, перезаписывая его данные в файле
    void Update(Project& project) {
        std::string projectFilePath = GetProjectFilePath(project.id);
        if (!fs::exists(projectFilePath)) {
            throw std::runtime_error("Project not found: " + project.id);
        }

        // Открываем файл для перезаписи данных
        std::ofstream file(projectFilePath, std::ios::trunc);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to update project file: " + projectFilePath);
        }

        // Записываем обновленные данные проекта в файл
        file << "#TITLE " << project.title << "\n"
             << "#DB_PATH " << project.dbPath << "\n";

        file.close();
    }

    // Удаляет проект по его ID, удаляя соответствующий файл
    void Delete(const std::string& id) {
        std::string projectFilePath = GetProjectFilePath(id);
        if (fs::exists(projectFilePath)) {
            fs::remove(projectFilePath);  // Удаление файла проекта
        }
    }
};



#endif //CURSE_PROJECTREPOSITORYIMP_H
