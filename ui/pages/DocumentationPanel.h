#ifndef CURSE_DOCUMENTATIONPANEL_H
#define CURSE_DOCUMENTATIONPANEL_H

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/hyperlink.h>
#include <wx/filename.h>
#include "../../domain/Documentation.h"
#include "../../state/State.h"

// Панель для отображения информации о документации
class DocumentationPanel : public wxPanel {
public:
    explicit DocumentationPanel(wxWindow* parent, State* state)
            : wxPanel(parent, wxID_ANY), appState(state) {

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        // Хедер с кнопками "Back" и "Edit"
        wxPanel* headerPanel = new wxPanel(this, wxID_ANY);
        wxBoxSizer* headerSizer = new wxBoxSizer(wxHORIZONTAL);

        backButton = new wxButton(headerPanel, wxID_ANY, "Back");
        headerSizer->Add(backButton, 0, wxALL, 5);

        headerText = new wxStaticText(headerPanel, wxID_ANY, "Documentation Details");
        headerSizer->Add(headerText, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        // Добавляем пустое пространство для выравнивания кнопки вправо
        headerSizer->AddStretchSpacer(1);

        // Кнопка "Edit", выравненная по правому краю
        editButton = new wxButton(headerPanel, wxID_ANY, "Edit");
        headerSizer->Add(editButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5); // Добавляем кнопку

        headerPanel->SetSizer(headerSizer);
        mainSizer->Add(headerPanel, 0, wxEXPAND | wxALL, 5);

        // Основная информация
        wxBoxSizer* contentSizer = new wxBoxSizer(wxVERTICAL);

        // Заголовок
        contentSizer->Add(new wxStaticText(this, wxID_ANY, "Title:"), 0, wxALL, 5);
        titleText = new wxStaticText(this, wxID_ANY, "");
        contentSizer->Add(titleText, 0, wxALL | wxEXPAND, 5);

        // Описание
        contentSizer->Add(new wxStaticText(this, wxID_ANY, "Description:"), 0, wxALL, 5);
        descriptionText = new wxStaticText(this, wxID_ANY, "");
        contentSizer->Add(descriptionText, 0, wxALL | wxEXPAND, 5);

        // Теги
        contentSizer->Add(new wxStaticText(this, wxID_ANY, "Tags:"), 0, wxALL, 5);
        tagsText = new wxStaticText(this, wxID_ANY, "");
        contentSizer->Add(tagsText, 0, wxALL | wxEXPAND, 5);

        // Путь к файлу
        contentSizer->Add(new wxStaticText(this, wxID_ANY, "File Path:"), 0, wxALL, 5);
        filePathText = new wxStaticText(this, wxID_ANY, "");
        contentSizer->Add(filePathText, 0, wxALL | wxEXPAND, 5);

        // Кнопка "Открыть файл"
        openFileButton = new wxButton(this, wxID_ANY, "Open File");
        contentSizer->Add(openFileButton, 0, wxALL | wxALIGN_CENTER, 10);

        mainSizer->Add(contentSizer, 1, wxALL | wxEXPAND, 5);

        SetSizerAndFit(mainSizer);

        // Привязываем обработчики событий
        backButton->Bind(wxEVT_BUTTON, &DocumentationPanel::OnBack, this);
        openFileButton->Bind(wxEVT_BUTTON, &DocumentationPanel::OnOpenFile, this);
        editButton->Bind(wxEVT_BUTTON, &DocumentationPanel::OnEdit, this);  // Привязка обработчика к кнопке "Edit"

        // Подписываемся на изменение состояния
        appState->Bind(EVT_APP_STATE_CHANGED, &DocumentationPanel::OnStateChanged, this);

        // Инициализируем UI данными из editDoc
        UpdateUI();
    }

    ~DocumentationPanel() override {
        // Отписываемся от изменений состояния
        appState->Unbind(EVT_APP_STATE_CHANGED, &DocumentationPanel::OnStateChanged, this);
    }

private:
    State* appState;                 // Ссылка на объект состояния
    wxButton* backButton;            // Кнопка "Назад"
    wxButton* editButton;            // Кнопка "Edit"
    wxStaticText* headerText;        // Текст заголовка в хедере
    wxStaticText* titleText;         // Текстовое поле для заголовка
    wxStaticText* descriptionText;   // Текстовое поле для описания
    wxStaticText* tagsText;          // Текстовое поле для тегов
    wxStaticText* filePathText;      // Текстовое поле для пути к файлу
    wxButton* openFileButton;        // Кнопка "Открыть файл"

    // Метод для объединения тегов в строку
    std::string joinTags(const std::vector<std::string>& tags) {
        std::string result;
        for (size_t i = 0; i < tags.size(); ++i) {
            result += tags[i];
            if (i < tags.size() - 1) result += ", ";
        }
        return result;
    }

    // Обновление UI на основе данных из editDoc
    void UpdateUI() {
        if (!appState->editDoc) {
            // Если editDoc пуст, очищаем поля
            titleText->SetLabel("");
            descriptionText->SetLabel("");
            tagsText->SetLabel("");
            filePathText->SetLabel("");
            openFileButton->Enable(false);
            editButton->Enable(false);  // Отключаем кнопку "Edit", если документа нет
        } else {
            // Обновляем данные из editDoc
            titleText->SetLabel(wxString::FromUTF8(appState->editDoc->title.c_str()));
            descriptionText->SetLabel(wxString::FromUTF8(appState->editDoc->description.c_str()));
            tagsText->SetLabel(wxString::FromUTF8(joinTags(appState->editDoc->tags).c_str()));
            filePathText->SetLabel(wxString::FromUTF8(appState->editDoc->filePath.c_str()));
            openFileButton->Enable(true);
            editButton->Enable(true);  // Включаем кнопку "Edit", если документ есть
        }

        // Обновляем отображение
        Layout();
    }

    // Обработчик изменения состояния
    void OnStateChanged(wxCommandEvent& event) {
        event.Skip();
        UpdateUI();
    }

    // Обработчик для кнопки "Back"
    void OnBack(wxCommandEvent& event) {
        appState->SetPage(Pages::Main);
    }

    // Обработчик события для открытия файла
    void OnOpenFile(wxCommandEvent& event) {
        if (!appState->editDoc || appState->editDoc->filePath.empty()) {
            wxMessageBox("No file to open.", "Error", wxOK | wxICON_ERROR);
            return;
        }

        wxString filePath = wxString::FromUTF8(appState->editDoc->filePath.c_str());
        if (!wxFileExists(filePath)) {
            wxMessageBox("The file does not exist or the path is invalid.", "Error", wxOK | wxICON_ERROR);
            return;
        }

        if (!wxLaunchDefaultApplication(filePath)) {
            wxMessageBox("Unable to open the file with the default application.", "Error", wxOK | wxICON_ERROR);

            wxFileName fileName(filePath);
            fileName.MakeAbsolute();  // Теперь fileName.GetFullPath() содержит абсолютный путь

            // Открываем проводник Windows и выделяем файл с абсолютным путем
            wxString command = wxString::Format("explorer /select,%s", fileName.GetFullPath());
            wxExecute(command);
        }
    }

    // Обработчик для кнопки "Edit"
    void OnEdit(wxCommandEvent& event) {
        if (appState->editDoc) {
            appState->SetPage(Pages::Edit);
        }
    }
};
#endif // CURSE_DOCUMENTATIONPANEL_H
