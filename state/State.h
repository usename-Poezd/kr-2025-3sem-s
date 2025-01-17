//
// Created by Tom on 1/17/2025.
//

#ifndef CURSE_STATE_H
#define CURSE_STATE_H

#include <wx/event.h>
#include "../services/DocumentationService.h"

// Создаем новое событие для уведомлений об изменении состояния
wxDECLARE_EVENT(EVT_APP_STATE_CHANGED, wxCommandEvent);

enum Pages {
    Main,
    Add,
    View,
    Edit
};

class State : public wxEvtHandler {
private:
    DocumentationService& docService;

public:
    Pages page;
    std::vector<Documentation> documentations;
    Documentation* editDoc = nullptr;

    explicit State(DocumentationService& service) : page(Pages::Main), docService(service) {
        this->documentations = docService.GetAllDocumentation();
    }

    // Метод для отправки уведомления об изменении состояния
    void NotifyStateChanged() {
        wxCommandEvent event(EVT_APP_STATE_CHANGED);
        wxPostEvent(this, event);
    }

    // Возвращаем ссылку на DocumentationService
    DocumentationService& GetDocumentationService() {
        return docService;
    }

    void SetPage(Pages p) {
        page = p;
        switch (p) {
            case Main:
                this->documentations = docService.GetAllDocumentation();
                this->editDoc = nullptr;
                break;
            case Add:
                this->editDoc = nullptr;
                break;
            case View:
                break;
            case Edit:
                break;
        }
        NotifyStateChanged();
    }
};

#endif //CURSE_STATE_H
