//
// Created by Tom on 1/17/2025.
//

#ifndef CURSE_DOCUMENTATIONSERVICE_H
#define CURSE_DOCUMENTATIONSERVICE_H


#include "../domain/Documentation.h"
#include "../repositories/DocumentationRepository.h"

class DocumentationService {
private:
    DocumentationRepository& repository;

public:
    DocumentationService(DocumentationRepository& repo) : repository(repo) {}

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
