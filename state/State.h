//
// Created by Tom on 1/17/2025.
//

#ifndef CURSE_STATE_H
#define CURSE_STATE_H

#include <wx/event.h>

#include <utility>
#include "../services/DocumentationService.h"
#include "../services/ProjectService.h"

// Создаем новое событие для уведомлений об изменении состояния
wxDECLARE_EVENT(EVT_APP_STATE_CHANGED, wxCommandEvent);

enum Pages {
    ProjectMain,
    ProjectAdd,

    Main,
    Add,
    View,
    Edit
};

class State : public wxEvtHandler {
private:
    DocumentationService* docService;
    ProjectService* projectService;

public:
    Pages page;
    std::vector<Documentation> documentations;
    std::vector<Project> projects;
    Project* project = nullptr;
    Documentation* editDoc = nullptr;

    explicit State( ProjectService* projectService, DocumentationService* service) : page(Pages::Main), projectService(projectService), docService(service) {
        projects = projectService->GetAllProjects();
    }

    // Метод для отправки уведомления об изменении состояния
    void NotifyStateChanged() {
        wxCommandEvent event(EVT_APP_STATE_CHANGED);
        ProcessEvent(event);
    }

    ProjectService* GetProjectService() {
        return projectService;
    }


    // Возвращаем ссылку на DocumentationService
    DocumentationService* GetDocumentationService() {
        return docService;
    }
    void SetDocumentationService(DocumentationService* newDocService) {
        delete this->docService;
        this->docService = newDocService;
    }

    void SetPage(Pages p) {
        page = p;
        switch (p) {
            case Main:
                this->documentations = docService->GetAllDocumentation();
                this->SetEditDoc(nullptr);
                break;
            case Add:
                this->SetEditDoc(nullptr);
                break;
            case View:
                break;
            case Edit:
                break;
            case ProjectMain:
                this->SetProject(nullptr);
                projects = projectService->GetAllProjects();
                break;
            case ProjectAdd:
                break;
        }
        NotifyStateChanged();
    }

    void SetDocumentations(std::vector<Documentation> newDocumentations) {
        this->documentations.clear();
        this->documentations = std::move(newDocumentations);
        NotifyStateChanged();
    }

    void SetEditDoc(Documentation *newDoc) {
        delete this->editDoc;
        this->editDoc = newDoc;
    }

    void SetProject(Project *newProj) {
        delete this->project;
        this->project = newProj;
    }
};

#endif //CURSE_STATE_H
