# Курсовая работа

Курсовая работа по созданию приложения на C++ с графическим интерфейсом, в качестве библиотеки для графичесого интерфейса выбран [wxWidgets](https://www.wxwidgets.org/)
## Содержание
- [wxWidgets](#wxwidgets)
  - [Библиотека](#биоблетека)
  - [Основные сущности](#основные-сущности)
  - [Обработка событий](#обработка-событий)


## wxWidgets
### Биоблетека
Биоблитека для кроссплатформенной разработки(под Widnows, Linux, MaxOs) приложений с гарфическим интерфейсом.
Примеры работ с использованием wxWidgets:
- [Maxima](https://en.wikipedia.org/wiki/Maxima_(software))
- [Code::Blocks](https://en.wikipedia.org/wiki/Code::Blocks)
- [PgAdmin](https://en.wikipedia.org/wiki/PgAdmin)

Библиотека используются в комерческих решениях и зарекомендовала себя на рынке.

### Основные сущности
Библиотека оперирует сущностями для управления и отрисовки графического интерфейся, одни из:
- **App** - основной класс приложения, является точкой входа всего приложения(через него оно запускается), через него регистрируются окна(Frames)
- **Frame** - класс окна в операционной системе, он регестрируется в **App** и после этого отображается у пользователя

Пример использования App и регистрации Frame в [main.cpp](main.cpp)

```c++
#include <wx/wx.h>
#include <wx/sizer.h>
#include "repositories/DocumentationRepositoryImp.h"
#include "services/DocumentationService.h"
#include "state/State.h"
#include "ui/frames/MainFrame.h"
#include "repositories/ProjectRepositoryImp.h"
#include "ui/frames/ProjectFrame.h"


// Основной класс приложения
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

        // Создаем окна для управления проектами и документациями
        mainFrame = new MainFrame(appState);
        projectFrame = new ProjectFrame(appState, mainFrame);

        // Привязываем обработчики событий закрытия окон
        projectFrame->Bind(wxEVT_CLOSE_WINDOW, &MyApp::OnProjectFrameClose, this);
        mainFrame->Bind(wxEVT_CLOSE_WINDOW, &MyApp::OnMainFrameClose, this);

        
        // При этом показываем сначала только окно управления проектами
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

// Запускаем приложение
wxIMPLEMENT_APP(MyApp);
```

- **Panel**(Pages) - класс который является неким контейнером, для других частей графического интерфейса, он нужен чтобы логически связывать другие части интерфейса(Кнопки, надписи, текстовые поля и тд.)

Более детально с панелями(страницами) можно ознакомится в папке [ui/pages](./ui/pages)(Там лежат страницы) м [ui/compnonents](./ui/components)(Там лежат компоненты, которые исподльзуются в страницах)

** **Остальные сущности более детально не описываются**, такие как кнопки, тестовые поля и тд, так как по коду они будут использоватся Panels

### Обработка событий

У каждой класса(сущности) графического интерфейса есть определенный набор событий, который можно отслеживать. Например у кнопки это будет - клик; у текстового поля ввод - текста; у окна(Frame) - его закрытие. Так же разработчик вправе создавать свои пользовательские события и отправлять.
На каждое событие можно поставить обработчик - это функция, которая будет вызвана если в данной сущности произойдет какое-то событие.

Вернемся к примеру с [main.cpp](main.cpp), в данном классе мы вешаем обработчики на закрытие окон(Frame) для классов MainFrame и ProjectFrame
```c++
// Привязываем обработчики событий закрытия окон
projectFrame->Bind(wxEVT_CLOSE_WINDOW, &MyApp::OnProjectFrameClose, this);
mainFrame->Bind(wxEVT_CLOSE_WINDOW, &MyApp::OnMainFrameClose, this);
```

Фунция `Bind` позволяет нам зарегестрировать обработчие, она принимает 3 аргумента:
- тип события
- ссылка на метод класса обработчика (Проще говоря функцию, которая будет обрабатывать запрос)
- сам класс обработчика(в данном случае это класс MyApp, тк this указывает на этот класс)

Функция обработчика выглядит следующим образом
```c++
 // Обработчик закрытия ProjectFrame
    void OnProjectFrameClose(wxCloseEvent& event) {
        // Закрываем MainFrame при закрытии ProjectFrame
        if (mainFrame != nullptr) {
            mainFrame->Destroy();  // Закрываем MainFrame
            mainFrame = nullptr; // Удаляем ссылку на MainFrame
        }
        event.Skip();  // Сигнализируем системе, что событие обработано
    }
```

Обратим внимания на эту строчку:
```с++
event.Skip();  // Сигнализируем системе, что событие обработано
```
Этот код необходимо вызвать, в тех случаях, когда существует несколько обработчиков на одно и то же событие, чтобы "пробрасывать" событие дальше, другим обработчикам, иначе же до других обработчиков не дойдет событие, так как оно будет "поглащено" конкретно этим обработчиком.