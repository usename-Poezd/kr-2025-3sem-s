//
// Created by Tom on 1/17/2025.
//

#ifndef CURSE_DOCUMENTATIONREPOSITORYIMP_H
#define CURSE_DOCUMENTATIONREPOSITORYIMP_H

#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <optional>
#include "DocumentationRepository.h"

namespace fs = std::filesystem;

class DocumentationRepositoryImp : public DocumentationRepository {
private:
    std::string dbPath;
    std::string docsPath;
    std::string docsFilesPath;

    std::string GetDocFilePath(const std::string& id) const {
        return docsPath + "/" + id + ".txt";
    }

    std::string GetDocAttachmentPath(const std::string& id, const std::string& extension) const {
        return docsFilesPath + "/" + id + "." + extension;
    }

    static Documentation ParseDocFile(const std::string& id, const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open documentation file: " + filePath);
        }

        std::string title, descriptionLine, tagsLine, filePathLine;

        // Читаем TITLE
        std::getline(file, title);
        if (title.substr(0, 7) != "#TITLE ") {
            throw std::runtime_error("Invalid title format in documentation file: " + filePath);
        }
        title = title.substr(7);  // Удаляем "#TITLE "

        // Читаем DESCRIPTION (многострочное описание)
        std::string description;
        while (std::getline(file, descriptionLine)) {
            // Прерываем, когда встречаем #TAGS
            if (descriptionLine.substr(0, 6) == "#TAGS ") {
                tagsLine = descriptionLine;  // Сохраняем строку с тегами
                break;  // Выход из цикла, так как описание закончено
            }
            description += descriptionLine + "\n";  // Добавляем строку в описание
        }

        // Убираем последнюю лишнюю новую строку
        if (!description.empty()) {
            description = description.substr(0, description.size() - 1);
        }
        description = description.substr(13); // Удаляем "#DESCRIPTION "


        // Читаем TAGS
        if (tagsLine.substr(0, 6) != "#TAGS ") {
            throw std::runtime_error("Invalid tags format in documentation file: " + filePath);
        }
        tagsLine = tagsLine.substr(6);  // Удаляем "#TAGS "

        std::vector<std::string> tags;
        std::istringstream tagStream(tagsLine);
        std::string tag;
        while (std::getline(tagStream, tag, ',')) {
            tags.push_back(tag);
        }

        // Читаем FILEPATH, если он существует
        std::string filePathValue;
        if (std::getline(file, filePathLine) && filePathLine.rfind("#FILEPATH ", 0) == 0) {
            filePathValue = filePathLine.substr(10);  // Удаляем "#FILEPATH "
        }

        return Documentation{id, title, description, tags, filePathValue};
    }
public:
    DocumentationRepositoryImp(const std::string& basePath)
            : dbPath(basePath), docsPath(basePath + "/documentations"), docsFilesPath(basePath + "/documentations_files") {
        if (!fs::exists(dbPath)) fs::create_directory(dbPath);
        if (!fs::exists(docsPath)) fs::create_directory(docsPath);
        if (!fs::exists(docsFilesPath)) fs::create_directory(docsFilesPath);
    }

    std::vector<Documentation> FindAll() override {
        std::vector<Documentation> docs;
        for (const auto& entry : fs::directory_iterator(docsPath)) {
            if (entry.path().extension() == ".txt") {
                std::string id = entry.path().stem().string();
                docs.push_back(ParseDocFile(id, entry.path().string()));
            }
        }
        return docs;
    }

    std::optional<Documentation> FindById(const std::string& id) override {
        std::string filePath = GetDocFilePath(id);
        if (!fs::exists(filePath)) return std::nullopt;
        return ParseDocFile(id, filePath);
    }

    void Create(Documentation& doc) override {
        std::string docFilePath = GetDocFilePath(doc.id);
        if (fs::exists(docFilePath))
            throw std::runtime_error("Documentation with ID already exists: " + doc.id);

        // Создаем файл, если filePath указан
        if (!doc.filePath.empty()) {
            std::ifstream src(doc.filePath, std::ios::binary);
            if (!src.is_open()) {
                throw std::runtime_error("Failed to open source file: " + doc.filePath);
            }

            // Генерируем расширение и путь для сохранения файла
            std::string ext = fs::path(doc.filePath).extension().string().substr(1);  // Получаем расширение без точки
            std::string attachmentPath = GetDocAttachmentPath(doc.id, ext);

            std::ofstream dest(attachmentPath, std::ios::binary);
            dest << src.rdbuf();
            dest.close();

            // Обновляем путь в объекте doc
            doc.filePath = attachmentPath;
        }

        // Создаем метаданные документа
        std::ofstream file(docFilePath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to create documentation file: " + docFilePath);
        }

        file << "#TITLE " << doc.title << "\n"
             << "#DESCRIPTION " << doc.description << "\n"
             << "#TAGS ";
        for (size_t i = 0; i < doc.tags.size(); ++i) {
            file << doc.tags[i];
            if (i < doc.tags.size() - 1) file << ",";
        }

        // Записываем путь к файлу, если он существует
        if (!doc.filePath.empty()) {
            file << "\n#FILEPATH " << doc.filePath;
        }

        file.close();
    }

    void Update(Documentation& doc) override {
        std::string docFilePath = GetDocFilePath(doc.id);
        if (!fs::exists(docFilePath))
            throw std::runtime_error("Documentation not found: " + doc.id);

        // Читаем текущие данные документа из файла
        Documentation currentDoc = ParseDocFile(doc.id, docFilePath);
//
//        fs::remove(docFilePath);


        // Проверяем, указан ли новый путь к файлу
        if (!doc.filePath.empty() && doc.filePath != currentDoc.filePath) {
            fs::remove(currentDoc.filePath);
            // Копируем новый файл
            std::ifstream src(doc.filePath, std::ios::binary);
            if (!src.is_open()) {
                throw std::runtime_error("Failed to open source file: " + doc.filePath);
            }

            // Генерируем расширение и путь для сохранения нового файла
            std::string ext = fs::path(doc.filePath).extension().string().substr(1);  // Получаем расширение без точки
            std::string newAttachmentPath = GetDocAttachmentPath(doc.id, ext);

            std::ofstream dest(newAttachmentPath, std::ios::binary);
            dest << src.rdbuf();
            dest.close();

            // Обновляем путь в объекте doc
            doc.filePath = newAttachmentPath;
        }

        // Обновляем данные текущего документа
        doc.title = doc.title;
        doc.description = doc.description;
        doc.tags = doc.tags;

        // Перезаписываем файл метаданных
        std::ofstream file(docFilePath, std::ios::trunc); // Открываем с флагом truncate для перезаписи
        if (!file.is_open()) {
            throw std::runtime_error("Failed to update documentation file: " + docFilePath);
        }

        file << "#TITLE " << doc.title << "\n"
             << "#DESCRIPTION " << doc.description << "\n"
             << "#TAGS ";
        for (size_t i = 0; i < doc.tags.size(); ++i) {
            file << doc.tags[i];
            if (i < doc.tags.size() - 1) file << ",";
        }

        // Записываем путь к файлу, если он существует
        if (!doc.filePath.empty()) {
            file << "\n#FILEPATH " << doc.filePath;
        }

        file.close();
    }

    void Delete(const std::string& id) override {
        std::string docFilePath = GetDocFilePath(id);
        if (fs::exists(docFilePath)) fs::remove(docFilePath);

        for (const auto& entry : fs::directory_iterator(docsFilesPath)) {
            if (entry.path().stem() == id) fs::remove(entry.path());
        }
    }
};


#endif //CURSE_DOCUMENTATIONREPOSITORYIMP_H
