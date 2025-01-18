#ifndef CURSE_MAINFRAME_H
#define CURSE_MAINFRAME_H

#include <wx/frame.h>
#include <wx/event.h>
#include <wx/log.h>
#include "../../state/State.h"
#include "../pages/DocumentationForm.h"
#include "../components/Header.h"
#include "../components/DocumentationList.h"
#include "../pages/DocumentationPanel.h"
#include "../pages/MainPanel.h"

// Определяем событие для изменения состояния
wxDECLARE_EVENT(EVT_APP_STATE_CHANGED, wxCommandEvent);

class MainFrame : public wxFrame {
private:
    State* appState;  // Ссылка на объект State
    wxPanel* activePanel;
    DocumentationForm* documentationForm;
    DocumentationPanel* documentationPanel;
    MainPanel* mainPanel;
    Pages prevPage = Pages::Main;

    // Обработчик события изменения состояния
    void OnStateChanged(wxCommandEvent& event) {
        event.Skip();
        if (prevPage == appState->page) {
            return;
        }

        prevPage = appState->page;
        activePanel->Show(false);  // Скрываем текущую активную панель

        switch (appState->page) {
            case Main:
                activePanel = mainPanel;  // Переключаем на mainPanel
                break;
            case Add:
                activePanel = documentationForm;  // Переключаем на documentationForm
                break;
            case View:
                activePanel = documentationPanel;  // Переключаем на documentationForm
                break;
            case Edit:
                activePanel = documentationForm;  // Переключаем на documentationForm
                break;
        }

        activePanel->Show();  // Отображаем новую активную панель
        Layout();
    }

public:
    // Конструктор класса MainFrame
    explicit MainFrame(State* state)
            : wxFrame(nullptr, wxID_ANY, "Main Frame"), appState(state) {

        documentationForm = new DocumentationForm(this, appState);
        documentationForm->Show(false);

        documentationPanel = new DocumentationPanel(this, appState);
        documentationPanel->Show(false);

        // Основной сайзер для окна
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        // Создаем контейнер (wxPanel), который будет содержать Header и DocumentationList
        mainPanel = new MainPanel(this, appState);


        activePanel = mainPanel;  // Устанавливаем основную панель как активную

        // Устанавливаем основной сайзер для окна
        mainSizer->Add(mainPanel, 1, wxEXPAND | wxALL, 0);
        mainSizer->Add(documentationPanel, 1, wxEXPAND | wxALL, 0);
        mainSizer->Add(documentationForm, 1, wxEXPAND | wxALL, 0);

        this->SetSizer(mainSizer);

        // Устанавливаем начальный размер окна
        this->SetSize(wxSize(800, 600));

        // Отправляем событие на изменение состояния
        appState->Bind(EVT_APP_STATE_CHANGED, &MainFrame::OnStateChanged, this);
    }

    // Деструктор, отписываемся от события
    ~MainFrame() override {
        appState->Unbind(EVT_APP_STATE_CHANGED, &MainFrame::OnStateChanged, this);
    }
};

#endif // CURSE_MAINFRAME_H
