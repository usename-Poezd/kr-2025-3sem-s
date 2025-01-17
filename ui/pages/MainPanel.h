//
// Created by Tom on 1/17/2025.
//

#ifndef CURSE_MAINPANEL_H
#define CURSE_MAINPANEL_H


#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/button.h>
#include "../../state/State.h"
#include "../components/DocumentationList.h"
#include "../components/Header.h"

// В любом компоненте (например, панели интерфейса)
class MainPanel : public wxPanel {
private:
    State *appState;
public:
    MainPanel(wxWindow* parent, State* appState)
            : wxPanel(parent), appState(appState) {
        // Подписываемся на событие изменения состояния
        // Сайзер для контейнера
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        // Добавляем Header в контейнер (верхняя панель)
        Header* header = new Header(this, appState);
        mainSizer->Add(header, 0, wxEXPAND | wxALL, 5);  // Закрепляем header сверху

        // Добавляем DocumentationList в контейнер (основная панель)
        DocumentationList* docList = new DocumentationList(this, appState);
        mainSizer->Add(docList, 1, wxEXPAND | wxALL, 5);  // Остальную часть экрана заполняет docList

        this->SetSizer(mainSizer);
    }
};

#endif //CURSE_MAINPANEL_H
