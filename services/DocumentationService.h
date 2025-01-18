//
// Created by Tom on 1/17/2025.
//

#ifndef CURSE_DOCUMENTATIONSERVICE_H
#define CURSE_DOCUMENTATIONSERVICE_H


#include <algorithm>
#include "../domain/Documentation.h"
#include "../repositories/DocumentationRepository.h"


enum Search {Title, Description};

class DocumentationService {
private:
    DocumentationRepository& repository;

public:
    DocumentationService(DocumentationRepository& repo) : repository(repo) {}

    std::vector<Documentation> Search(const std::string& query, Search searchIn)
    {
        std::vector<Documentation> filteredDocumentation;

        // Получаем все документы из репозитория
        auto allDocs = repository.FindAll();

        // Приводим строку запроса к нижнему регистру для нечувствительного поиска
        std::string lowerQuery = query;
        std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

        // Проходим по всем документам и фильтруем их
        for (const auto& doc : allDocs) {
            bool match = false;

            switch (searchIn) {
                case Search::Title: {
                    // Приводим title к нижнему регистру и ищем вхождение query
                    std::string lowerTitle = doc.title;
                    std::transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), ::tolower);
                    match = (lowerTitle.find(lowerQuery) != std::string::npos);
                    break;
                }
                case Search::Description: {
                    // Приводим description к нижнему регистру и ищем вхождение query
                    std::string lowerDescription = doc.description;
                    std::transform(lowerDescription.begin(), lowerDescription.end(), lowerDescription.begin(), ::tolower);
                    match = (lowerDescription.find(lowerQuery) != std::string::npos);
                    break;
                }
            }

            // Если нашли совпадение, добавляем документ в результат
            if (match) {
                filteredDocumentation.push_back(doc);
            }
        }

        return filteredDocumentation;
    }

    std::vector<Documentation> GetAllDocumentation() {
        return repository.FindAll();
    }

    std::optional<Documentation> GetDocumentationById(const std::string& id) {
        return repository.FindById(id);
    }

    void AddDocumentation(Documentation& doc) {
        repository.Create(doc);
    }

    void UpdateDocumentation(Documentation& doc) {
        repository.Update(doc);
    }

    void RemoveDocumentation(const std::string& id) {
        repository.Delete(id);
    }
};


#endif //CURSE_DOCUMENTATIONSERVICE_H
