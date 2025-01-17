#ifndef CURSE_DOCUMENTATIONPANEL_H
#define CURSE_DOCUMENTATIONPANEL_H

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/hyperlink.h>
#include "../../domain/Documentation.h"
#include "../../state/State.h"

// Панель для отображения информации о документации
class DocumentationPanel : public wxPanel {
public:
    explicit DocumentationPanel(wxWindow* parent, State* state)
            : wxPanel(parent, wxID_ANY), appState(state) {

        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

        // Заголовок
        sizer->Add(new wxStaticText(this, wxID_ANY, "Title:"), 0, wxALL, 5);
        titleText = new wxStaticText(this, wxID_ANY, "");
        sizer->Add(titleText, 0, wxALL | wxEXPAND, 5);

        // Описание
        sizer->Add(new wxStaticText(this, wxID_ANY, "Description:"), 0, wxALL, 5);
        descriptionText = new wxStaticText(this, wxID_ANY, "");
        sizer->Add(descriptionText, 0, wxALL | wxEXPAND, 5);

        // Теги
        sizer->Add(new wxStaticText(this, wxID_ANY, "Tags:"), 0, wxALL, 5);
        tagsText = new wxStaticText(this, wxID_ANY, "");
        sizer->Add(tagsText, 0, wxALL | wxEXPAND, 5);

        // Путь к файлу
        sizer->Add(new wxStaticText(this, wxID_ANY, "File Path:"), 0, wxALL, 5);
        filePathText = new wxStaticText(this, wxID_ANY, "");
        sizer->Add(filePathText, 0, wxALL | wxEXPAND, 5);

        // Кнопка "Открыть файл"
        openFileButton = new wxButton(this, wxID_ANY, "Open File");
        sizer->Add(openFileButton, 0, wxALL | wxALIGN_CENTER, 10);

        // Привязываем обработчик события для кнопки
        openFileButton->Bind(wxEVT_BUTTON, &DocumentationPanel::OnOpenFile, this);

        // Устанавливаем сайзер
        SetSizerAndFit(sizer);

        // Подписываемся на изменения состояния
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
        } else {
            // Обновляем данные из editDoc
            titleText->SetLabel(wxString::FromUTF8(appState->editDoc->title.c_str()));
            descriptionText->SetLabel(wxString::FromUTF8(appState->editDoc->description.c_str()));
            tagsText->SetLabel(wxString::FromUTF8(joinTags(appState->editDoc->tags).c_str()));
            filePathText->SetLabel(wxString::FromUTF8(appState->editDoc->filePath.c_str()));
            openFileButton->Enable(true);
        }

        // Обновляем отображение
        Layout();
    }

    // Обработчик изменения состояния
    void OnStateChanged(wxCommandEvent& event) {
        UpdateUI();
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
        }
    }
};

#endif // CURSE_DOCUMENTATIONPANEL_H
