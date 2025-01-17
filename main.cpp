#include <wx/wx.h>
#include <wx/sizer.h>
#include "repositories/DocumentationRepositoryImp.h"
#include "services/DocumentationService.h"
#include "state/State.h"
#include "ui/frames/MainFrame.h"


class MyApp : public wxApp {
private:
    State* appState;

public:
    virtual bool OnInit() override {
        setlocale(LC_ALL, "");
        std::locale::global(std::locale(""));
        // Создаем репозиторий, сервис и состояние
        auto* repo = new DocumentationRepositoryImp("./db");
        auto* service = new DocumentationService(*repo);
        appState = new State(*service);

        wxFrame* mainFrame = new MainFrame(appState);
        mainFrame->Show();
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);