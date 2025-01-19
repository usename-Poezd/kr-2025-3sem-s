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
    // Путь к базе данных
    std::string dbPath;

    // Путь к директории, где хранятся текстовые файлы документаций
    std::string docsPath;

    // Путь к директории, где хранятся прикрепленные файлы документаций
    std::string docsFilesPath;

    // Возвращает полный путь к файлу документации по его ID
    std::string GetDocFilePath(const std::string& id) const {
        return docsPath + "/" + id + ".txt";
    }

    // Возвращает путь к файлу-вложению документации по его ID и расширению
    std::string GetDocAttachmentPath(const std::string& id, const std::string& extension) const {
        return docsFilesPath + "/" + id + "." + extension;
    }

    // Читает и получает файл документации, создавая объект `Documentation`
    static Documentation ParseDocFile(const std::string& id, const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open documentation file: " + filePath);
        }

        std::string title, descriptionLine, tagsLine, filePathLine;

        // Читаем заголовок документации
        std::getline(file, title);
        if (title.substr(0, 7) != "#TITLE ") {
            throw std::runtime_error("Invalid title format in documentation file: " + filePath);
        }
        title = title.substr(7);  // Удаляем "#TITLE "

        // Читаем многострочное описание документации
        std::string description;
        while (std::getline(file, descriptionLine)) {
            // Прерываем чтение описания, когда встречаем строку с тегами
            if (descriptionLine.substr(0, 6) == "#TAGS ") {
                tagsLine = descriptionLine;  // Сохраняем строку с тегами
                break;
            }
            description += descriptionLine + "\n";  // Добавляем строку в описание
        }

        // Удаляем последнюю лишнюю новую строку
        if (!description.empty()) {
            description = description.substr(0, description.size() - 1);
        }
        description = description.substr(13); // Удаляем "#DESCRIPTION "

        // Читаем теги документации
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

        // Читаем путь к прикрепленному файлу, если он указан
        std::string filePathValue;
        if (std::getline(file, filePathLine) && filePathLine.rfind("#FILEPATH ", 0) == 0) {
            filePathValue = filePathLine.substr(10);  // Удаляем "#FILEPATH "
        }

        return Documentation{id, title, description, tags, filePathValue};
    }

public:
    // Конструктор класса: инициализирует пути к директориям и создает их, если они не существуют
    DocumentationRepositoryImp(const std::string& basePath)
            : dbPath(basePath), docsPath(basePath + "/documentations"), docsFilesPath(basePath + "/documentations_files") {
        if (!fs::exists(dbPath)) fs::create_directory(dbPath);
        if (!fs::exists(docsPath)) fs::create_directory(docsPath);
        if (!fs::exists(docsFilesPath)) fs::create_directory(docsFilesPath);
    }

    // Возвращает список всех документаций
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

    // Возвращает документацию по её ID, если она существует
    std::optional<Documentation> FindById(const std::string& id) override {
        std::string filePath = GetDocFilePath(id);
        if (!fs::exists(filePath)) return std::nullopt;
        return ParseDocFile(id, filePath);
    }

    // Создает новую документацию
    void Create(Documentation& doc) override {
        std::string docFilePath = GetDocFilePath(doc.id);
        if (fs::exists(docFilePath))
            throw std::runtime_error("Documentation with ID already exists: " + doc.id);

        // Если указан файл, копируем его в директорию
        if (!doc.filePath.empty()) {
            // Открываем исходный файл для чтения
            std::ifstream src(doc.filePath, std::ios::binary);
            if (!src.is_open()) {
                throw std::runtime_error("Failed to open source file: " + doc.filePath); // Ошибка, если файл нельзя открыть
            }

            // Извлекаем расширение исходного файла
            std::string ext = fs::path(doc.filePath).extension().string().substr(1);

            // Генерируем путь для сохранения прикрепленного файла
            std::string attachmentPath = GetDocAttachmentPath(doc.id, ext);

            // Создаем и записываем файл в директории
            std::ofstream dest(attachmentPath, std::ios::binary);
            dest << src.rdbuf(); // Копируем содержимое исходного файла
            dest.close(); // Закрываем новый файл

            // Обновляем путь к прикрепленному файлу в объекте `doc`
            doc.filePath = attachmentPath;
        }

        // Создаем файл метаданных документации
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

    // Обновляет существующую документацию
    void Update(Documentation& doc) override {
        std::string docFilePath = GetDocFilePath(doc.id);
        if (!fs::exists(docFilePath))
            throw std::runtime_error("Documentation not found: " + doc.id);

        // Читаем текущие данные документа из файла
        Documentation currentDoc = ParseDocFile(doc.id, docFilePath);

        // Если указан новый файл, копируем его и удаляем старый
        if (!doc.filePath.empty() && doc.filePath != currentDoc.filePath) {
            // Удаляем старый прикрепленный файл
            fs::remove(currentDoc.filePath);

            // Открываем исходный файл для чтения
            std::ifstream src(doc.filePath, std::ios::binary);
            if (!src.is_open()) {
                throw std::runtime_error("Failed to open source file: " + doc.filePath); // Ошибка, если файл нельзя открыть
            }

            // Извлекаем расширение нового файла
            std::string ext = fs::path(doc.filePath).extension().string().substr(1);

            // Генерируем путь для нового прикрепленного файла
            std::string newAttachmentPath = GetDocAttachmentPath(doc.id, ext);

            // Создаем и записываем новый файл по указанному пути
            std::ofstream dest(newAttachmentPath, std::ios::binary);
            dest << src.rdbuf(); // Копируем содержимое исходного файла
            dest.close(); // Закрываем новый файл

            // Обновляем путь к прикрепленному файлу в объекте `doc`
            doc.filePath = newAttachmentPath;
        }

        // Перезаписываем файл метаданных
        std::ofstream file(docFilePath, std::ios::trunc);
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

    // Удаляет документацию по её ID
    void Delete(const std::string& id) override {
        std::string docFilePath = GetDocFilePath(id);
        if (fs::exists(docFilePath)) fs::remove(docFilePath);

        for (const auto& entry : fs::directory_iterator(docsFilesPath)) {
            if (entry.path().stem() == id) fs::remove(entry.path());
        }
    }
};

#endif //CURSE_DOCUMENTATIONREPOSITORYIMP_H
