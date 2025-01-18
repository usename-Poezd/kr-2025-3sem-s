//
// Created by Tom on 1/17/2025.
//

#ifndef CURSE_DOCUMENTATIONLIST_H
#define CURSE_DOCUMENTATIONLIST_H


#include <wx/wx.h>
#include <wx/scrolwin.h>
#include <memory>
#include "../../state/State.h"

class DocumentationList : public wxPanel {
private:
    wxScrolledWindow* scrollWindow;  // Прокручиваемая область
    wxBoxSizer* scrollSizer;         // Сайзер для прокручиваемого окна
    State *appState;


    void UpdateList() {
        // Очистим старые элементы
        scrollWindow->DestroyChildren();

        // Добавим новые элементы документации
        for (const auto& doc : appState->documentations) {
            wxPanel* docPanel = new wxPanel(scrollWindow, wxID_ANY);
            wxBoxSizer* docSizer = new wxBoxSizer(wxHORIZONTAL);

            // Заголовок документации
            wxStaticText* titleText = new wxStaticText(docPanel, wxID_ANY, wxString::FromUTF8(doc.title));
            titleText->Wrap(200); // Установим ширину для переноса текста

            // Описание документации
            wxStaticText* descriptionText = new wxStaticText(docPanel, wxID_ANY, wxString::FromUTF8(doc.description));
            descriptionText->Wrap(400);

            // Кнопка "Посмотреть"
            wxButton* viewButton = new wxButton(docPanel, wxID_ANY, L"Посмотреть");
            viewButton->Bind(wxEVT_BUTTON, [this, doc](wxCommandEvent&) {
                OnViewClicked(doc);
            });

            // Кнопка "Удалить"
            wxButton* deleteButton = new wxButton(docPanel, wxID_ANY, L"Удалить");
            deleteButton->Bind(wxEVT_BUTTON, [this, doc](wxCommandEvent&) {
                OnDeleteClicked(doc);
            });

            // Добавляем элементы в горизонтальный сайзер
            docSizer->Add(titleText, 1, wxEXPAND | wxALL, 5);
            docSizer->Add(descriptionText, 2, wxEXPAND | wxALL, 5);
            docSizer->Add(viewButton, 0, wxALL, 5);
            docSizer->Add(deleteButton, 0, wxALL, 5);

            // Применяем сайзер к панели
            docPanel->SetSizer(docSizer);
            scrollSizer->Add(docPanel, 0, wxEXPAND | wxALL, 5);
        }

        scrollWindow->Layout();
    }

    void OnViewClicked(const Documentation& doc) {
        appState->SetEditDoc(new Documentation(doc));
        appState->SetPage(Pages::View);
    }

    // Обработчик клика на "Удалить"
    void OnDeleteClicked(const Documentation& doc) {
        wxMessageDialog dialog(
                this,
                L"Вы уверены, что хотите удалить эту документацию?",
                L"Подтверждение удаления",
                wxYES_NO | wxNO_DEFAULT | wxICON_WARNING
        );

        if (dialog.ShowModal() == wxID_YES) {
            try {
                appState->GetDocumentationService()->RemoveDocumentation(doc.id);
                appState->SetPage(Pages::Main);
            } catch (const std::exception& e) {
                wxMessageBox("Ошибка удаления документации: " + wxString(e.what()), "Ошибка", wxOK | wxICON_ERROR);
            }
        }
    }


public:
    explicit DocumentationList(wxWindow* parent, State* state)
            : wxPanel(parent, wxID_ANY), appState(state) {
        // Создаем прокручиваемое окно
        scrollWindow = new wxScrolledWindow(this, wxID_ANY);
        scrollWindow->SetScrollRate(5, 5);  // Настроим скорость прокрутки

        // Сайзер для прокручиваемого окна
        scrollSizer = new wxBoxSizer(wxVERTICAL);
        scrollWindow->SetSizer(scrollSizer);

        // Главный сайзер
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        mainSizer->Add(scrollWindow, 1, wxEXPAND | wxALL, 5);
        this->SetSizer(mainSizer);


        UpdateList();

        // Подписываемся на изменение состояния
        appState->Bind(EVT_APP_STATE_CHANGED, &DocumentationList::OnStateChanged, this);
    }

    // Обновление списка документации
    void OnStateChanged(wxCommandEvent& event) {
        event.Skip();
        UpdateList();
    }

    // Деструктор, отписываемся от события
    ~DocumentationList() override {
        appState->Unbind(EVT_APP_STATE_CHANGED, &DocumentationList::OnStateChanged, this);
    }


};

#endif //CURSE_DOCUMENTATIONLIST_H
