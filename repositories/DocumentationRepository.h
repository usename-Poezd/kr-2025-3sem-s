//
// Created by Tom on 1/17/2025.
//

#ifndef CURSE_DOCUMENTATIONREPOSITORY_H
#define CURSE_DOCUMENTATIONREPOSITORY_H

#include <optional>
#include "../domain/Documentation.h"

/**
 * @brief Интерфейс репозитория для управления объектами документации.
 *
 * DocumentationRepository определяет контракт (абстракцию) для работы с объектами документации, включая их
 * создание, обновление, удаление и поиск. Этот интерфейс может быть реализован для различных источников данных,
 * таких как базы данных, файловые системы или API.
 */
class DocumentationRepository {
public:
    virtual ~DocumentationRepository() = default;

    /**
     * @brief Получение списка всех объектов документации.
     *
     * @return std::vector<Documentation> Список всех объектов документации.
     */
    virtual std::vector<Documentation> FindAll() = 0;

    /**
     * @brief Поиск объекта документации по идентификатору.
     *
     * @param id Уникальный идентификатор документации.
     * @return std::optional<Documentation> Объект документации, если он найден, или `std::nullopt`, если объект отсутствует.
     */
    virtual std::optional<Documentation> FindById(const std::string& id) = 0;

    /**
     * @brief Создание нового объекта документации.
     *
     * @param doc Ссылка на объект документации, который нужно создать.
     */
    virtual void Create(Documentation& doc) = 0;

    /**
     * @brief Обновление существующего объекта документации.
     *
     * @param doc Ссылка на объект документации с обновленными данными.
     */
    virtual void Update(Documentation& doc) = 0;

    /**
     * @brief Удаление объекта документации по идентификатору.
     *
     * @param id Уникальный идентификатор документации, которую нужно удалить.
     */
    virtual void Delete(const std::string& id) = 0;
};
#endif //CURSE_DOCUMENTATIONREPOSITORY_H
