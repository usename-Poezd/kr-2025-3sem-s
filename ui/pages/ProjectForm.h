#ifndef CURSE_PROJECTFORM_H
#define CURSE_PROJECTFORM_H

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/filepicker.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <sstream>
#include <random>
#include "../../domain/Project.h"
#include "../../state/State.h"
#include "../../services/ProjectService.h"

class ProjectForm : public wxPanel {
public:
    ProjectForm(wxWindow* parent, State* state)
            : wxPanel(parent, wxID_ANY), appState(state) {

        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

        wxPanel* headerPanel = new wxPanel(this, wxID_ANY);
        wxBoxSizer* headerSizer = new wxBoxSizer(wxHORIZONTAL);

        backButton = new wxButton(headerPanel, wxID_ANY, "Back");
        headerSizer->Add(backButton, 0, wxALL, 5);

        titleText = new wxStaticText(headerPanel, wxID_ANY, "Add Project");
        headerSizer->Add(titleText, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL, 5);

        headerPanel->SetSizer(headerSizer);
        sizer->Add(headerPanel, 0, wxEXPAND | wxALL, 5);

        // Название проекта
        sizer->Add(new wxStaticText(this, wxID_ANY, "Project Title:"), 0, wxALL, 5);
        titleCtrl = new wxTextCtrl(this, wxID_ANY);
        sizer->Add(titleCtrl, 0, wxALL | wxEXPAND, 5);

        // Папка с базой данных
        sizer->Add(new wxStaticText(this, wxID_ANY, "Database Folder:"), 0, wxALL, 5);
        dbPathPicker = new wxDirPickerCtrl(this, wxID_ANY);
        sizer->Add(dbPathPicker, 0, wxALL | wxEXPAND, 5);

        // Кнопка для сохранения
        saveButton = new wxButton(this, wxID_ANY, "Save");
        sizer->Add(saveButton, 0, wxALL | wxALIGN_CENTER, 10);

        SetSizerAndFit(sizer);

        // Подключаем обработчики событий
        backButton->Bind(wxEVT_BUTTON, &ProjectForm::OnBack, this);
        saveButton->Bind(wxEVT_BUTTON, &ProjectForm::OnSave, this);
        appState->Bind(EVT_APP_STATE_CHANGED, &ProjectForm::OnStateChanged, this);
    }

    ~ProjectForm() override {
        appState->Unbind(EVT_APP_STATE_CHANGED, &ProjectForm::OnStateChanged, this);
    }

private:
    wxTextCtrl* titleCtrl;
    wxDirPickerCtrl* dbPathPicker;
    wxButton* saveButton;
    wxButton* backButton;
    wxStaticText* titleText;

    State* appState;  // Ссылка на объект State

    void OnBack(wxCommandEvent& event) {
        appState->SetPage(Pages::ProjectMain);
    }

    void OnStateChanged(wxCommandEvent& event) {
        event.Skip();
        titleCtrl->Clear();
        dbPathPicker->SetPath("");
        saveButton->SetLabel("Save");
        titleText->SetLabel("Add Project");
    }

    void OnSave(wxCommandEvent& event) {
        wxString title = titleCtrl->GetValue();
        wxString dbPath = dbPathPicker->GetPath();

        // Валидация
        if (title.IsEmpty() || dbPath.IsEmpty()) {
            wxMessageBox("Please fill all the fields.", title, wxOK | wxICON_ERROR);
            return;
        }

        std::string id = generateUUID();
        Project project(id, title.ToStdString(wxConvUTF8), dbPath.ToStdString(wxConvUTF8));

        try {
            // Получаем ссылку на сервис проекта через state
            ProjectService* projectService = appState->GetProjectService();
            projectService->CreateProject(project);
            wxMessageBox("Project saved successfully.", "Success", wxOK | wxICON_INFORMATION);

            appState->SetPage(Pages::ProjectMain);
        } catch (const std::exception& e) {
            wxMessageBox("Error saving project: " + wxString(e.what()), "Error", wxOK | wxICON_ERROR);
        }
    }

    std::string generateUUID() {
        std::random_device rd;
        std::uniform_int_distribution<int> dist(0, 15);

        std::stringstream ss;
        ss << std::hex;

        for (int i = 0; i < 8; ++i) {
            ss << dist(rd);
        }
        ss << "-";
        for (int i = 0; i < 4; ++i) {
            ss << dist(rd);
        }
        ss << "-";
        for (int i = 0; i < 4; ++i) {
            ss << dist(rd);
        }
        ss << "-";
        for (int i = 0; i < 4; ++i) {
            ss << dist(rd);
        }
        ss << "-";
        for (int i = 0; i < 12; ++i) {
            ss << dist(rd);
        }

        return ss.str();
    }
};

#endif // CURSE_PROJECTFORM_H
