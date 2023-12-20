
set(INCLUDE_FILES
    "Client.cpp"
    "Config.h"
    "Controller.h" "Controller.cpp"
    "Common.h"
    "Worker.h" "Worker.cpp"  
)

set(INCLUDE_LIBS
    inih
    NlWrapper
    SimpleLogger
)

add_executable(Client ${INCLUDE_FILES})
target_link_libraries(Client PRIVATE ${INCLUDE_LIBS})