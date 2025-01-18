cmake_minimum_required(VERSION 3.28)
project(curse)

set(CMAKE_CXX_STANDARD 17)

set(wxWidgets_ROOT_DIR "C:/wxWidgets")
set(wxWidgets_LIB_DIR  "C:/wxWidgets/lib/gcc810_x64_dll")
find_package(wxWidgets REQUIRED ui/components net core base)
if(wxWidgets_USE_FILE) # not defined in CONFIG mode
    include(${wxWidgets_USE_FILE})
endif()

add_executable(curse WIN32  main.cpp
        ui/components/Header.cpp
        ui/components/Header.h
        domain/Documentation.cpp
        domain/Documentation.h
        repositories/DocumentationRepository.h
        repositories/DocumentationRepositoryImp.cpp
        repositories/DocumentationRepositoryImp.h
        services/DocumentationService.cpp
        services/DocumentationService.h
        state/State.cpp
        state/State.h
        ui/pages/MainPanel.cpp
        ui/pages/MainPanel.h
        ui/pages/DocumentationForm.cpp
        ui/pages/DocumentationForm.h
        ui/frames/MainFrame.cpp
        ui/frames/MainFrame.h
        ui/components/DocumentationList.cpp
        ui/components/DocumentationList.h
        ui/pages/DocumentationPanel.cpp
        ui/pages/DocumentationPanel.h
        domain/Project.cpp
        domain/Project.h
        repositories/ProjectRepositoryImp.cpp
        repositories/ProjectRepositoryImp.h
        services/ProjectService.cpp
        services/ProjectService.h
        ui/pages/ProjectForm.cpp
        ui/pages/ProjectForm.h
        ui/frames/ProjectFrame.cpp
        ui/frames/ProjectFrame.h)
target_link_libraries(curse ${wxWidgets_LIBRARIES})
target_include_directories(curse PRIVATE ${wxWidgets_INCLUDE_DIRS})
