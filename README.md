# Курсовая работа

Курсовая работа по созданию приложения на C++ с графическим интерфейсом, в качестве библиотеки для графичесого интерфейса выбран [wxWidgets](https://www.wxwidgets.org/)
## Содержание
- [wxWidgets](#wxwidgets)
  - [Библиотека](#биоблетека)
  - [Основные сущности](#основные-сущности)
  - [Обработка событий](#обработка-событий)
- [Структура проекта](#структура-проекта)
  - [Описание каталогов](#описание-каталогов)
  - [Основные классы](#основные-классы)
    - [State](#state)
    - [Domain](#domain)
    - [Repositories](#repositories)
      - [База данных](#база-данных)
    - [Services](#services)
   


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

## Структура проекта
В этом пункте мы разберем общую файловую структуру проекта, его основные составляющие


### Описание каталогов
Каждый каталог(папка) имеет свою предназначение для хранение того или иного вида файлов, общая структура приложения такая:

```
C:\STUDY\CURSE
├───.idea           // Папка для хранения файлов для CLion IDE
│
├───domain          // В этой папке хранятся основные сущности приложения, проще говоря тут хранятся классы, 
│                      которые являются отображением данных в нашей файловой БД
│
├───repositories    // Папка для хранения классов, предназначенных для работы с файловой БД
│
├───services        // Папка для хранения классов, прежназначенных для манипулирования данными 
│                      и осуществлением каких то действий над данными. В этой папке кранятся сервис классы
│                      с которые работают с классами из папки repositories и после получения данных из repositories
│                      занимаются обработкой/филтрацией данных
│
├───state           // Папка с одним классом State, который отвечает за состояние ВСЕЙ программы.
│                      Этот класс отправляет событие о изменении его состояния, которое можно обрабатывать из других классов 
│
├───ui              // Папка в которой находятся все классы, с помощью которых рисуется графический интерфейс
│   │
│   ├───components  // Папка для компонентов - переизспользуемых частей интерфейса
│   │
│   ├───frames      // Папка для фреймов(окон) - в ней лежат классы всех окна, которые используются в приложении
│   │
│   └───pages       // Папка для Panels(страниц) - в ней лежат классы всех страницы, которые используются в приложении
│
└─── main.cpp       // Файл запуска программы
```

### Основные классы
Этот раздел посвящен основным классам системы

<br>
<hr>

#### [State](./state/State.h)
Представляет собой класс для реализации состояния всего приложения. Он отcылает событие `EVT_APP_STATE_CHANGED`, к которому можно подключится обработчикам для всех элементов интерфейса.

<br>

**Зачем это нужно?**

С помощью State окно(Frame) понимает:
- Какую страницу ему отображать

С помощью State страница(Page) понимает:
- Какие данные отображать
- Какой класс сейчас просматривает/редактирует пользователь

Так же в State находятся сервис классы(см. папку [services](./services)), так как экземпляр этих классов, так же как и классов репозиториев(см. папку [repositories](./repositories)), должен быть один, чтобы не вызывать неожиданную запись данных в БД.

<br>

**Почему он удобен?**

Механизм событий в State делает приложение гибким и модульным:
- Отправка событий:
  State не "знает", кто подписан на события, он просто отправляет их через ProcessEvent.

  Это позволяет легко добавлять новые подписчики (например, страницы или вспомогательные окна) без изменения самого State.

- Расширяемость:
  Если потребуется добавить новое событие, его можно объявить аналогично EVT_APP_STATE_CHANGED и использовать для другой логики (например, уведомления об ошибках).

- Асинхронность:
  События обрабатываются независимо, что позволяет избежать жесткой связности между State и интерфейсом.

<br>

**Как работает механизм событий в State?**

Механизм событий в классе State построен на основе функциональности событийной системы, предоставляемой библиотекой wxWidgets.

<br>

**Давайте разберем его работу пошагово:**
1. **Объявление пользовательского события**

    Событие `EVT_APP_STATE_CHANGED` объявлено с помощью макроса `wxDECLARE_EVENT`:
    ```c++
    wxDECLARE_EVENT(EVT_APP_STATE_CHANGED, wxCommandEvent);
    ```
    **Что это делает?**
      - Создает идентификатор пользовательского события EVT_APP_STATE_CHANGED.
      - Это событие можно отправлять и обрабатывать в разных частях приложения.

<hr/>

2. **Отправка события**

    Для уведомления других частей приложения о изменении состояния, в классе State используется метод NotifyStateChanged:
    ```c++
    void NotifyStateChanged() {
        wxCommandEvent event(EVT_APP_STATE_CHANGED); // Создаем объект события
        ProcessEvent(event); // Отправляем событие, чтобы его могли обработать подписчики
    }
    ```

    **Как это работает?**
   - Создается объект события wxCommandEvent с идентификатором `EVT_APP_STATE_CHANGED`.
   - Метод ProcessEvent(event) отправляет это событие всем зарегистрированным обработчикам (например, окнам или страницам).
   - Это позволяет уведомить интерфейс (или другие части программы) о том, что состояние изменилось.

<hr/>

3. **Подключение обработчиков**

    Чтобы другие части приложения могли реагировать на событие, используется метод Bind (или макросы EVT_* в wxWidgets):

    **Пример подключения [обработчика](#обработка-событий):**
    ```c++
    frame->Bind(EVT_APP_STATE_CHANGED, &MainFrame::OnStateChanged, this);
    ```

    **Что делает обработчик?**
    - В обработчике можно, например, обновить элементы интерфейса (перерисовать страницы, обновить списки и т. д.) или выполнить дополнительные действия на основе измененного состояния.

<br>
<hr>

#### [Domain](./domain)
Это классы, которые представляют собой данные. В нашем случае это:

- Класс Проекта
    ```c++
    class Project {
    public:
    std::string id;          // ID проекта
    std::string title;       // Название проекта
    std::string dbPath;      // Путь до папки с документацией
    
        Project() = default;
        Project(std::string id, std::string title, std::string dbPath)
                : id(std::move(id)), title(std::move(title)), dbPath(std::move(dbPath)) {}
    };
    ```  
- Класс Документации
    ```c++
    class Documentation {
    public:
        std::string id;           // ID документации
        std::string title;        // Название
        std::string description;  // Описание
        std::vector<std::string> tags;  // Теги
        std::string filePath;     // Путь к прикрепленному файлу
    
        Documentation() = default;
        Documentation(std::string  id,
                      std::string  title,
                      std::string  description,
                      const std::vector<std::string>& tags,
                      std::string  filePath)
                : id(std::move(id)), title(std::move(title)), description(std::move(description)), tags(tags), filePath(std::move(filePath)) {}
    };
    ```
  
У каждого класса есть свойство `id`, которое предоставляет собой уникальный идентификатор конкретных данных.

** P. S.  По логике `id` напоминает серию и номер паспорта для идентификации личности.

<br>
<hr>

#### [Repositories](./repositories)
Классы репозиториев представляют собой реализацию файловой базы данных.

Классы репозиториев — это объектно-ориентированный шаблон проектирования, предназначенный для управления доступом к данным. Они служат абстракцией между приложением и хранилищем данных (например, базой данных, файлами или API). В папке [repositories](./repositories) лежат два интерфейса для описания, какие методы для работы с данными должен обладать класс репозитория для работы с проектами и документацией.

В программировании **интерфейс**(не путать с графическим интерфейсом) — это абстракция, определяющая контракт, который должен реализовать класс. Интерфейс описывает что должно быть сделано, но не как это сделать.

**Зачем нужен интерфейс?**

Интерфейс позволяет менять реализацию класса репозитория и не переписывая весь код программы. Только меняя классы, отвечающие за реализацию этих интерфейсов мы можем в какой-то момент времени поменять БД с нашей(файловой) на реальную(PostgreSQL) или БД другого рода(Excel).

** P. S. Чаговец говорил, что мы не можем использовать нормальную базу данных в проекте(Так как мы ее не проходили), но нужно было написать код таким образом, чтобы когда мы пройдем Базы Данных наш код нужно было быстро переписать. Именно поэтому мы сначала описали поведение класса через интерфейс, а после - сам класс. В остальных частях системы мы работаем через интерфейс, то есть как бы "по контракту", чтобы не привязываться к конкретной реализации.


Интерфейсов(как и самих репозиториев) в приложении 2:
- [Интерфейс для репозитория Документации](./repositories/DocumentationRepository.h)
- [Интерфейс для репозитория Проекта](./repositories/ProjectRepository.h)

Реализация интерфейсов:
- [Документации](./repositories/DocumentationRepositoryImp.h)
- [Проекта](./repositories/ProjectRepositoryImp.h)

<br>
<hr>

##### База данных

Для понимания реализации репозиториев нужно понять как реализована база данных.

База данных для документации представляет собой две папки `documentations` и `documentations_files` для файлов с данными по документации и ее файлу соответственно.

Структура папки базы данных документации:
```
./db
├───documentations              // Папка с данными по документациям
│   │
│   ├───documentationId1.txt    // Файл для описании данных документации у которой id = documentationId1
│   └───documentationId2.txt    // Файл для описании данных документации у которой id = documentationId2
│
└───documentations_files        // Папка с файлами, которые прикреплены к документации
    │
    ├───documentationId1.docx   // Файл, прикрепленный к документации у которой id = documentationId1
    └───documentationId2.png    // Файл, прикрепленный к документации у которой id = documentationId2
```

Файл для описания документации выглядит так:
```
#TITLE Названии документации
#DESCRIPTION Описание документации
Оно может быть многострочное
#TAGS тег1,тег2
#FILEPATH C:\study\curse\cmake-build-debug-mingw\newProjDoc/documentations_files/379c88cb-6a9f-d295-6344-9dbf0505974f.htm
```

Каждое свойство класса [Documentation](./domain/Documentation.h) записано под соответствующим #, по которому в последствии получается и записывается в свойство класса в функции [ParseDocFile](./repositories/DocumentationRepositoryImp.h#L41)

<br>

База данных для документации проектов представляет собой папку `projects` для файлов с данными по проекту.

Структура папки базы данных проектов:
```
./db
└───projects                    // Папка с данными по проектам
    │
    ├───projectId1.txt    // Файл для описании данных проекта у которого id = projectId1
    └───projectId1.txt    // Файл для описании данных проекта у которого id = projectId1
```

Файл для описания проекта выглядит так:
```
#TITLE Название проекта
#DB_PATH C:\study\curse\cmake-build-debug-mingw\newProjDoc
```

Каждое свойство класса [Project](./domain/Project.h) записано под соответствующим #, по которому в последствии получается и записывается в свойство класса в функции [ParseProjectFile](./repositories/ProjectRepositoryImp.h#L27)

### Services

В папке [services](./services) лежат классы для работы с domain классами.
Сервисные классы нужны для того, чтобы упрощать бизнес-логику и сделать её более организованной. Они играют роль посредников между основными частями приложения (например графическим интерфейсом/State и репозиториями).
