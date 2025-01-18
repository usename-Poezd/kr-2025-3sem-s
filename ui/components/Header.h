#ifndef CURSE_HEADER_H
#define CURSE_HEADER_H

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <map>
#include "../../state/State.h"

class Header : public wxPanel {
private:
    State* appState;               // Указатель на состояние приложения
    wxTextCtrl* searchField;       // Поле ввода для поиска
    wxChoice* searchChoice;        // Выпадающий список для выбора области поиска
    wxButton* searchButton;        // Кнопка "Поиск"
    wxString currentSearchText;    // Текущий текст в поле поиска
    Search currentSearchFilter;  // Текущий выбранный фильтр
    std::map<wxString, Search> searchOptionsMap = {
            {L"в названии", Search::Title},
            {L"в описании", Search::Description}
    };


public:
    explicit Header(wxWindow* parent, State* state)
            : wxPanel(parent, wxID_ANY), appState(state) {
        // Создаем горизонтальный сайзер для размещения элементов
        wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

        // Высота элементов
        const int elementHeight = 30;

        // Заполняем выпадающий список метками из мапы
        wxArrayString choiceLabels;
        for (const auto& [label, value] : searchOptionsMap) {
            choiceLabels.Add(label);
        }

        searchChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(-1, elementHeight), choiceLabels);
        searchChoice->SetSelection(0); // Установить первый вариант по умолчанию
        currentSearchFilter = searchOptionsMap[searchChoice->GetString(0)]; // Первый фильтр по умолчанию

        // Привязываем обработчик выбора элемента
        searchChoice->Bind(wxEVT_CHOICE, &Header::OnSearchFilterChanged, this);

        mainSizer->Add(searchChoice, 0, wxALL | wxALIGN_CENTER_VERTICAL, 15);

        // Текстовое поле для поиска с кнопкой
        wxPanel* searchPanel = new wxPanel(this, wxID_ANY);
        wxBoxSizer* searchSizer = new wxBoxSizer(wxHORIZONTAL);

        searchField = new wxTextCtrl(searchPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, elementHeight), wxBORDER_NONE);
        searchField->SetHint(L"Поиск");
        // Привязываем обработчик изменения текста
        searchField->Bind(wxEVT_TEXT, &Header::OnSearchTextChanged, this);

        searchButton = new wxButton(searchPanel, wxID_ANY, L"Поиск", wxDefaultPosition, wxSize(50, elementHeight), wxBU_EXACTFIT);
        // Привязываем обработчик нажатия кнопки
        searchButton->Bind(wxEVT_BUTTON, &Header::OnSearchButtonClicked, this);

        searchSizer->Add(searchField, 1, wxEXPAND | wxALL, 0);
        searchSizer->Add(searchButton, 0, wxALL, 0);
        searchPanel->SetSizer(searchSizer);
        searchPanel->SetMinSize(wxSize(GetSize().GetWidth() * 0.7, elementHeight));

        mainSizer->Add(searchPanel, 1, wxALL | wxALIGN_CENTER_VERTICAL, 15);

        // Круглая кнопка с плюсом
        wxButton* addButton = new wxButton(this, wxID_ANY, "+", wxDefaultPosition, wxSize(elementHeight, elementHeight));
        mainSizer->Add(addButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 15);

        // Привязываем обработчик нажатия кнопки "Добавить"
        addButton->Bind(wxEVT_BUTTON, &Header::OnAddButtonClicked, this);

        // Установка основного сайзера
        this->SetSizer(mainSizer);
    }

    // Обработчик для изменения текста в строке поиска
    void OnSearchTextChanged(wxCommandEvent& event) {
        currentSearchText = searchField->GetValue();
    }

    // Обработчик для изменения выбора фильтра
    void OnSearchFilterChanged(wxCommandEvent& event) {
        int selection = searchChoice->GetSelection();
        wxString selectedLabel = searchChoice->GetString(selection);

        currentSearchFilter = searchOptionsMap[selectedLabel];
    }

    void OnSearchButtonClicked(wxCommandEvent& event) {
        appState->SetDocumentations(appState->GetDocumentationService()->Search(currentSearchText.ToStdString(), currentSearchFilter));
    }

    // Обработчик для нажатия кнопки "Добавить"
    void OnAddButtonClicked(wxCommandEvent& event) {
        appState->SetPage(Pages::Add);
    }
};

#endif //CURSE_HEADER_H
