#include <wx/wx.h>
#include <wx/sizer.h>
#include "repositories/DocumentationRepositoryImp.h"
#include "services/DocumentationService.h"
#include "state/State.h"
#include "ui/frames/MainFrame.h"
#include "repositories/ProjectRepositoryImp.h"
#include "ui/frames/ProjectFrame.h"


class MyApp : public wxApp {
private:
    State* appState;
    MainFrame* mainFrame;
    ProjectFrame* projectFrame;


public:
    virtual bool OnInit() override {
        setlocale(LC_ALL, "");
        std::locale::global(std::locale(""));

        // Создаем репозиторий, сервис и состояние
        auto* repo = new ProjectRepositoryImp("./db");
        auto* projService = new ProjectService(*repo);

        appState = new State(projService, nullptr);

        mainFrame = new MainFrame(appState);
        projectFrame = new ProjectFrame(appState, mainFrame);

        // Привязываем обработчики событий закрытия окон
        projectFrame->Bind(wxEVT_CLOSE_WINDOW, &MyApp::OnProjectFrameClose, this);
        mainFrame->Bind(wxEVT_CLOSE_WINDOW, &MyApp::OnMainFrameClose, this);

        projectFrame->Show();
        return true;
    }

    // Обработчик закрытия ProjectFrame
    void OnProjectFrameClose(wxCloseEvent& event) {
        // Закрываем MainFrame при закрытии ProjectFrame
        if (mainFrame != nullptr) {
            mainFrame->Destroy();  // Закрываем MainFrame
            mainFrame = nullptr; // Удаляем ссылку на MainFrame
        }
        event.Skip();  // Сигнализируем системе, что событие обработано
    }

    // Обработчик закрытия MainFrame
    void OnMainFrameClose(wxCloseEvent& event) {
        // Закрываем ProjectFrame при закрытии MainFrame
        if (projectFrame != nullptr && !projectFrame->IsShown()) {
            projectFrame->Destroy();  // Закрываем ProjectFrame
        }
        event.Skip();  // Сигнализируем системе, что событие обработано
    }
};

wxIMPLEMENT_APP(MyApp);
