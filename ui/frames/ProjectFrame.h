#ifndef CURSE_PROJECTFRAME_H
#define CURSE_PROJECTFRAME_H

#include <wx/frame.h>
#include <wx/event.h>
#include <wx/log.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include "../../state/State.h"
#include "../../domain/Project.h"
#include "../pages/ProjectForm.h"
#include "../../repositories/DocumentationRepositoryImp.h"
#include "MainFrame.h"

// Определяем событие для изменения состояния
wxDECLARE_EVENT(EVT_APP_STATE_CHANGED, wxCommandEvent);

class ProjectFrame : public wxFrame {
private:
    State* appState;  // Ссылка на объект State
    wxPanel* activePanel;
    wxChoice* projectSelect;    // Выбор проекта
    wxButton* openButton;       // Кнопка для открытия проекта
    wxButton* addButton;        // Кнопка для добавления нового проекта
    ProjectForm* projectForm;   // Панель для добавления нового проекта
    wxPanel* mainPanel;         // Панель для основного экрана
    Pages prevPage = Pages::ProjectMain;
    MainFrame* mainFrame;

    // Обработчик события изменения состояния
    void OnStateChanged(wxCommandEvent& event) {
        event.Skip();
        if (prevPage == appState->page) {
            return;
        }

        prevPage = appState->page;
        activePanel->Show(false);  // Скрываем текущую активную панель

        switch (appState->page) {
            case ProjectMain:
                activePanel = mainPanel;  // Переключаем на mainPanel
                break;
            case ProjectAdd:
                activePanel = projectForm;  // Переключаем на projectForm
                break;
        }

        activePanel->Show();  // Отображаем новую активную панель
        Layout();

        // Обновляем список проектов при изменении состояния
        UpdateProjectSelect();
    }

    // Обновление списка проектов в projectSelect
    void UpdateProjectSelect() {
        projectSelect->Clear();  // Очистим текущий список

        // Добавляем все проекты из appState
        for (const auto& project : appState->projects) {
            projectSelect->Append(wxString::FromUTF8(project.title.c_str()));
        }

        // Если есть выбранный проект, устанавливаем его в выбор
        if (appState->project != nullptr) {
            int selectedIdx = -1;
            for (size_t i = 0; i < appState->projects.size(); ++i) {
                if (appState->projects[i].id == appState->project->id) {
                    selectedIdx = i;
                    break;
                }
            }

            if (selectedIdx != -1) {
                projectSelect->SetSelection(selectedIdx);  // Устанавливаем выбранный проект
            }
        }
    }

    // Обработчик выбора проекта
    void OnProjectSelect(wxCommandEvent& event) {
        int selection = projectSelect->GetSelection();
        if (selection != wxNOT_FOUND) {
            appState->SetProject(new Project(appState->projects[selection]));  // Устанавливаем текущий проект в state
        }
    }

    // Обработчик кнопки "Открыть проект"
    void OnOpenProject(wxCommandEvent& event) {
        if (appState->project == nullptr) {
            wxMessageBox("Please select a project first.", "Error", wxOK | wxICON_ERROR);
            return;
        }

        // Создаем сервис для документации, используя путь из проекта
        auto* repo = new DocumentationRepositoryImp(appState->project->dbPath);
        auto* dicService = new DocumentationService(*repo);

        // Получаем сервис документации и передаем его в appState
        appState->SetDocumentationService(dicService);

        // Переключаем на MainFrame
        appState->SetPage(Pages::Main);

        mainFrame->Show(true);
        this->Show(false);
    }

    // Обработчик кнопки "Добавить проект"
    void OnAddProject(wxCommandEvent& event) {
        appState->SetPage(Pages::ProjectAdd);  // Переключаем на форму добавления проекта
    }

public:
    // Конструктор класса ProjectFrame
    explicit ProjectFrame(State* state, MainFrame* mainFrame)
            : wxFrame(nullptr, wxID_ANY, "Project Frame"), appState(state), mainFrame(mainFrame) {

        projectForm = new ProjectForm(this, appState);
        projectForm->Show(false);

        // Основной сайзер для окна
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        // Создаем панель для основного экрана с проектами
        wxPanel* mainPanelContainer = new wxPanel(this);
        wxBoxSizer* mainPanelSizer = new wxBoxSizer(wxVERTICAL);

        // Заголовок
        mainPanelSizer->Add(new wxStaticText(mainPanelContainer, wxID_ANY, "Projects"), 0, wxALL, 5);

        // Выбор проекта
        projectSelect = new wxChoice(mainPanelContainer, wxID_ANY);
        mainPanelSizer->Add(projectSelect, 0, wxALL | wxEXPAND, 5);
        projectSelect->Bind(wxEVT_CHOICE, &ProjectFrame::OnProjectSelect, this);

        // Кнопка "Открыть проект"
        openButton = new wxButton(mainPanelContainer, wxID_ANY, "Open Project");
        mainPanelSizer->Add(openButton, 0, wxALL | wxEXPAND, 5);
        openButton->Bind(wxEVT_BUTTON, &ProjectFrame::OnOpenProject, this);

        // Кнопка "Добавить проект"
        addButton = new wxButton(mainPanelContainer, wxID_ANY, "Add Project");
        mainPanelSizer->Add(addButton, 0, wxALL | wxEXPAND, 5);
        addButton->Bind(wxEVT_BUTTON, &ProjectFrame::OnAddProject, this);

        // Устанавливаем слайзер для панели
        mainPanelContainer->SetSizer(mainPanelSizer);
        mainPanel = mainPanelContainer;

        // Устанавливаем основной сайзер для окна
        mainSizer->Add(mainPanelContainer, 1, wxEXPAND | wxALL, 0);
        mainSizer->Add(projectForm, 1, wxEXPAND | wxALL, 0);

        activePanel = mainPanel;
        this->SetSizer(mainSizer);

        // Устанавливаем начальный размер окна
        this->SetSize(wxSize(800, 600));

        // Отправляем событие на изменение состояния
        appState->Bind(EVT_APP_STATE_CHANGED, &ProjectFrame::OnStateChanged, this);

        // Инициализация проекта
        UpdateProjectSelect();  // Обновляем список проектов на старте
    }

    // Деструктор, отписываемся от события
    ~ProjectFrame() override {
        appState->Unbind(EVT_APP_STATE_CHANGED, &ProjectFrame::OnStateChanged, this);
    }
};

#endif // CURSE_PROJECTFRAME_H
