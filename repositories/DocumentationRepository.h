//
// Created by Tom on 1/17/2025.
//

#ifndef CURSE_DOCUMENTATIONREPOSITORY_H
#define CURSE_DOCUMENTATIONREPOSITORY_H

#include <optional>
#include "../domain/Documentation.h"

class DocumentationRepository {
public:
    virtual ~DocumentationRepository() = default;

    virtual std::vector<Documentation> FindAll() = 0;
    virtual std::optional<Documentation> FindById(const std::string& id) = 0;
    virtual void Create(Documentation& doc) = 0;
    virtual void Update(Documentation& doc) = 0;
    virtual void Delete(const std::string& id) = 0;
};
#endif //CURSE_DOCUMENTATIONREPOSITORY_H
