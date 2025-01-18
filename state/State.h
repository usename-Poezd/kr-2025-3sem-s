//
// Created by Tom on 1/17/2025.
//

#ifndef CURSE_STATE_H
#define CURSE_STATE_H

#include <wx/event.h>

#include <utility>
#include "../services/DocumentationService.h"
#include "../services/ProjectService.h"

// Создаем новое пользовательское событие для уведомлений об изменении состояния приложения
wxDECLARE_EVENT(EVT_APP_STATE_CHANGED, wxCommandEvent);

// Перечисление (enum), представляющее возможные страницы в приложении
enum Pages {
    ProjectMain, // Основная страница выбора проектов
    ProjectAdd,  // Страница добавления нового проекта
    Main,        // Главная страница со списком документации и поиском
    Add,         // Страница добавления документации
    View,        // Страница просмотра документации
    Edit         // Страница редактирования документации
};

// Класс State управляет состоянием всего приложения.
// Наследуется от wxEvtHandler, что позволяет отправлять события wxWidgets.
class State : public wxEvtHandler {
private:
    // Указатели на сервисные классы, управляющие данными (документацией и проектами)
    DocumentationService* docService;
    ProjectService* projectService;

public:
    // Текущая страница приложения
    Pages page;

    // Списки с данными для отображения на страницах
    std::vector<Documentation> documentations; // Список документации
    std::vector<Project> projects;             // Список проектов

    // Указатели на текущий проект и документ, которые просматриваются/редактируются
    Project* project = nullptr;
    Documentation* editDoc = nullptr;

    // Конструктор, принимающий ссылки на сервисы проектов и документации.
    // Инициализирует начальную страницу `Main` и загружает список проектов.
    explicit State(ProjectService* projectService, DocumentationService* service)
            : page(Pages::Main), projectService(projectService), docService(service) {
        // Загружаем список проектов из ProjectService при инициализации
        projects = projectService->GetAllProjects();
    }

    // Уведомление об изменении состояния через пользовательское событие EVT_APP_STATE_CHANGED.
    // Это событие может быть обработано другими частями приложения, такими как окна и страницы.
    void NotifyStateChanged() {
        wxCommandEvent event(EVT_APP_STATE_CHANGED);
        ProcessEvent(event); // Отправляем событие, чтобы подписчики могли обновить свои данные
    }

    // Возвращает указатель на сервис управления проектами
    ProjectService* GetProjectService() {
        return projectService;
    }

    // Возвращает указатель на сервис управления документацией
    DocumentationService* GetDocumentationService() {
        return docService;
    }

    // Устанавливает новый сервис управления документацией.
    // Старый сервис удаляется, чтобы предотвратить утечку памяти.
    void SetDocumentationService(DocumentationService* newDocService) {
        delete this->docService; // Освобождаем память старого сервиса
        this->docService = newDocService; // Устанавливаем новый сервис
    }

    // Изменяет текущую страницу приложения.
    // Выполняются действия, связанные с переходом между страницами,
    // например, обновление списков или сброс текущих данных.
    void SetPage(Pages p) {
        page = p; // Устанавливаем текущую страницу
        switch (p) {
            case Main:
                // При переходе на главную страницу загружаем всю документацию
                this->documentations = docService->GetAllDocumentation();
                this->SetEditDoc(nullptr); // Сбрасываем выбранный документ
                break;
            case Add:
                // При переходе на страницу добавления документации сбрасываем выбранный документ
                this->SetEditDoc(nullptr);
                break;
            case View:
                // Просмотр документации не требует дополнительных действий
                break;
            case Edit:
                // Редактирование документации не требует дополнительных действий
                break;
            case ProjectMain:
                // При переходе на страницу выбора проектов сбрасываем текущий проект
                this->SetProject(nullptr);
                projects = projectService->GetAllProjects(); // Обновляем список проектов
                break;
            case ProjectAdd:
                // Добавление нового проекта не требует дополнительных действий
                break;
        }
        NotifyStateChanged(); // Уведомляем об изменении состояния
    }

    // Устанавливает новый список документации и уведомляет об изменении состояния
    void SetDocumentations(std::vector<Documentation> newDocumentations) {
        this->documentations.clear(); // Очищаем текущий список
        this->documentations = std::move(newDocumentations); // Перемещаем новый список
        NotifyStateChanged(); // Уведомляем об изменении состояния
    }

    // Устанавливает текущий документ для редактирования и освобождает память предыдущего
    void SetEditDoc(Documentation *newDoc) {
        delete this->editDoc; // Удаляем старый документ
        this->editDoc = newDoc; // Устанавливаем новый документ
    }

    // Устанавливает текущий проект и освобождает память предыдущего
    void SetProject(Project *newProj) {
        delete this->project; // Удаляем старый проект
        this->project = newProj; // Устанавливаем новый проект
    }
};

#endif //CURSE_STATE_H
