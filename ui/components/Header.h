//
// Created by Tom on 1/17/2025.
//

#ifndef CURSE_HEADER_H
#define CURSE_HEADER_H


#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/button.h>
#include "../../state/State.h"

class Header : public wxPanel {
private:
    State* appState;
public:
    explicit Header(wxWindow* parent, State* state)
            : wxPanel(parent, wxID_ANY), appState(state) {
        // Создаем горизонтальный сайзер для размещения элементов
        wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

        // Высота элементов
        const int elementHeight = 30;

        // Выпадающий список с вариантами поиска
        wxArrayString searchOptions;
        searchOptions.Add(L"в названии");
        searchOptions.Add(L"в описании");
        wxChoice* searchChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(-1, elementHeight), searchOptions);
        searchChoice->SetSelection(0); // Установить первый вариант по умолчанию
        mainSizer->Add(searchChoice, 0, wxALL | wxALIGN_CENTER_VERTICAL, 15);

        // Текстовое поле для поиска с кнопкой
        wxPanel* searchPanel = new wxPanel(this, wxID_ANY);
        wxBoxSizer* searchSizer = new wxBoxSizer(wxHORIZONTAL);

        wxTextCtrl* searchField = new wxTextCtrl(searchPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, elementHeight), wxBORDER_NONE);
        searchField->SetHint(L"Поиск");

        wxButton* searchButton = new wxButton(searchPanel, wxID_ANY, L"Поиск", wxDefaultPosition, wxSize(50, elementHeight), wxBU_EXACTFIT);

        searchSizer->Add(searchField, 1, wxEXPAND | wxALL, 0);
        searchSizer->Add(searchButton, 0, wxALL, 0);
        searchPanel->SetSizer(searchSizer);
        searchPanel->SetMinSize(wxSize(GetSize().GetWidth() * 0.7, elementHeight));

        mainSizer->Add(searchPanel, 1, wxALL | wxALIGN_CENTER_VERTICAL, 15);

        // Круглая кнопка с плюсом
        wxButton* addButton = new wxButton(this, wxID_ANY, "+", wxDefaultPosition, wxSize(elementHeight, elementHeight));
        mainSizer->Add(addButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 15);


        addButton->Bind(wxEVT_BUTTON, &Header::OnAddButtonClicked, this);

        // Установка основного сайзера
        this->SetSizer(mainSizer);
    }

    void OnAddButtonClicked(wxCommandEvent& event) {
        appState->SetPage(Pages::Add);
    }

};



#endif //CURSE_HEADER_H
