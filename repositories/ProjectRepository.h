#ifndef CURSE_PROJECTREPOSITORY_H
#define CURSE_PROJECTREPOSITORY_H

#include <vector>
#include <optional>
#include "../domain/Project.h"

/**
 * @brief Интерфейс репозитория для управления объектами проекта.
 *
 * ProjectRepository определяет контракт для работы с объектами проекта, включая их создание, обновление, удаление
 * и поиск. Этот интерфейс может быть реализован для различных хранилищ данных, таких как базы данных, файлы или API.
 */
class ProjectRepository {
public:
    virtual ~ProjectRepository() = default;

    /**
     * @brief Получение списка всех проектов.
     *
     * @return std::vector<Project> Список объектов проекта.
     */
    virtual std::vector<Project> FindAll() = 0;

    /**
     * @brief Поиск проекта по идентификатору.
     *
     * @param id Уникальный идентификатор проекта.
     * @return std::optional<Project> Объект проекта, если найден, или `std::nullopt`, если проект отсутствует.
     */
    virtual std::optional<Project> FindById(const std::string& id) = 0;

    /**
     * @brief Создание нового проекта.
     *
     * @param project Ссылка на объект проекта, который нужно создать.
     */
    virtual void Create(Project& project) = 0;

    /**
     * @brief Обновление существующего проекта.
     *
     * @param project Ссылка на объект проекта с обновленными данными.
     */
    virtual void Update(Project& project) = 0;

    /**
     * @brief Удаление проекта по идентификатору.
     *
     * @param id Уникальный идентификатор проекта, который нужно удалить.
     */
    virtual void Delete(const std::string& id) = 0;
};

#endif // CURSE_PROJECTREPOSITORY_H
