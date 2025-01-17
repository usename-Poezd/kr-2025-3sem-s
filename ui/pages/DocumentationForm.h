#ifndef CURSE_DOCUMENTATIONFORM_H
#define CURSE_DOCUMENTATIONFORM_H

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/filepicker.h>
#include <wx/tokenzr.h>
#include <wx/button.h>
#include <random>
#include <sstream>
#include <iomanip>
#include "../../domain/Documentation.h"
#include "../../services/DocumentationService.h"
#include "../../state/State.h"

class DocumentationForm : public wxPanel {
public:
    // Конструктор формы, поддерживающий два режима: создание и редактирование
    DocumentationForm(wxWindow* parent, State* state)
            : wxPanel(parent, wxID_ANY), appState(state) {

        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

        wxPanel* headerPanel = new wxPanel(this, wxID_ANY);
        wxBoxSizer* headerSizer = new wxBoxSizer(wxHORIZONTAL);

        backButton = new wxButton(headerPanel, wxID_ANY, "Back");
        headerSizer->Add(backButton, 0, wxALL, 5);

        titleText = new wxStaticText(headerPanel, wxID_ANY, "Adding Documentation");
        headerSizer->Add(titleText, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL, 5);

        headerPanel->SetSizer(headerSizer);
        sizer->Add(headerPanel, 0, wxEXPAND | wxALL, 5);


        // Заголовок
        sizer->Add(new wxStaticText(this, wxID_ANY, "Title:"), 0, wxALL, 5);
        titleCtrl = new wxTextCtrl(this, wxID_ANY);
        sizer->Add(titleCtrl, 0, wxALL | wxEXPAND, 5);

        // Описание
        sizer->Add(new wxStaticText(this, wxID_ANY, "Description:"), 0, wxALL, 5);
        descriptionCtrl = new wxTextCtrl(this, wxID_ANY);
        sizer->Add(descriptionCtrl, 0, wxALL | wxEXPAND, 5);

        // Теги
        sizer->Add(new wxStaticText(this, wxID_ANY, "Tags (comma separated):"), 0, wxALL, 5);
        tagsCtrl = new wxTextCtrl(this, wxID_ANY);
        sizer->Add(tagsCtrl, 0, wxALL | wxEXPAND, 5);

        // Файл
        sizer->Add(new wxStaticText(this, wxID_ANY, "Attach File:"), 0, wxALL, 5);
        filePicker = new wxFilePickerCtrl(this, wxID_ANY);
        sizer->Add(filePicker, 0, wxALL | wxEXPAND, 5);

        // Кнопка для сохранения
        saveButton = new wxButton(this, wxID_ANY, "Save");
        sizer->Add(saveButton, 0, wxALL | wxALIGN_CENTER, 10);

        SetSizerAndFit(sizer);


        // Подключаем обработчик событий для кнопки "Сохранить"
        backButton->Bind(wxEVT_BUTTON, &DocumentationForm::OnBack, this);
        saveButton->Bind(wxEVT_BUTTON, &DocumentationForm::OnSave, this);
        appState->Bind(EVT_APP_STATE_CHANGED, &DocumentationForm::OnStateChanged, this);
    }

    ~DocumentationForm() override {
        appState->Unbind(EVT_APP_STATE_CHANGED, &DocumentationForm::OnStateChanged, this);
    }



private:
    wxTextCtrl* titleCtrl;
    wxTextCtrl* descriptionCtrl;
    wxTextCtrl* tagsCtrl;
    wxFilePickerCtrl* filePicker;
    wxButton* saveButton;
    wxButton* backButton;
    wxStaticText* titleText;

    State* appState;  // Ссылка на объект State

    void OnBack(wxCommandEvent& event) {
        appState->SetPage(Pages::Main);
    }

    // Преобразует список тегов в строку с разделителем ","
    std::string joinTags(const std::vector<std::string>& tags) {
        std::string result;
        for (size_t i = 0; i < tags.size(); ++i) {
            result += tags[i];
            if (i < tags.size() - 1) result += ", ";
        }
        return result;
    }

    void OnStateChanged(wxCommandEvent& event) {
        // Если мы редактируем существующий документ, заполняем форму значениями документа
        if (appState->editDoc) {
            titleCtrl->SetValue(wxString::FromUTF8(appState->editDoc->title.c_str()));
            descriptionCtrl->SetValue(wxString::FromUTF8(appState->editDoc->description.c_str()));
            tagsCtrl->SetValue(wxString::FromUTF8(joinTags(appState->editDoc->tags).c_str()));
            filePicker->SetPath(wxString::FromUTF8(appState->editDoc->filePath.c_str()));
            saveButton->SetLabel("Update");  // Меняем текст на "Обновить"
            titleText->SetLabel("Editing Documentation");
            return;
        }

        titleCtrl->Clear();
        descriptionCtrl->Clear();
        tagsCtrl->Clear();
        filePicker->SetPath("");
        saveButton->SetLabel("Save");
        titleText->SetLabel("Adding Documentation");
    }


    void OnSave(wxCommandEvent& event) {
        wxString title = titleCtrl->GetValue();
        wxString description = descriptionCtrl->GetValue();
        wxString tags = tagsCtrl->GetValue();
        wxString filePath = filePicker->GetPath();


        // Валидация
        if (title.IsEmpty() || description.IsEmpty() || tags.IsEmpty()) {
            wxMessageBox("Please fill all the fields.", title, wxOK | wxICON_ERROR);
            return;
        }

        std::vector<std::string> tagList;
        wxStringTokenizer tokenizer(tags, ",");
        while (tokenizer.HasMoreTokens()) {
            tagList.push_back(tokenizer.GetNextToken().ToStdString(wxConvUTF8));
        }

        std::string id = generateUUID();
        if (appState->editDoc) {
            id = appState->editDoc->id;
        }
        // Создаем или обновляем объект документации
        Documentation doc(id,
                          title.ToStdString(wxConvUTF8),
                          description.ToStdString(wxConvUTF8),
                          tagList,
                          filePath.ToStdString(wxConvUTF8));

        // Получаем ссылку на DocumentationService через state
        DocumentationService& docService = appState->GetDocumentationService();

        try {
            if (appState->editDoc) {
                // Если это редактирование, обновляем документацию
                docService.UpdateDocumentation(doc);
                wxMessageBox("Documentation updated successfully.", "Success", wxOK | wxICON_INFORMATION);
            } else {
                // Если это создание, добавляем новую документацию
                docService.AddDocumentation(doc);
                wxMessageBox("Documentation saved successfully.", "Success", wxOK | wxICON_INFORMATION);
            }
        } catch (const std::exception& e) {
            wxMessageBox("Error saving documentation: " + wxString(e.what()), "Error", wxOK | wxICON_ERROR);
        }
    }

    std::string generateUUID() {
        std::random_device rd;
        std::uniform_int_distribution<int> dist(0, 15);

        std::stringstream ss;
        ss << std::hex;

        // UUID состоит из 32 символов, разделенных дефисами в стандартном формате 8-4-4-4-12
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

#endif // CURSE_DOCUMENTATIONFORM_H
