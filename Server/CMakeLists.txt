
set(INCLUDE_FILES
    "Acceptor.h"  "Acceptor.cpp"
    "BuildAnswer.h" "BuildAnswer.cpp"
    "Common.h" 
    "Server.cpp"
  "${CMAKE_SOURCE_DIR}/Third-party/SimpleJson/json.hpp"
)

set(INCLUDE_LIBS
    Actions
    inih
    SimpleLogger
	NlWrapper
)

add_executable(Server ${INCLUDE_FILES})
target_link_libraries(Server PRIVATE ${INCLUDE_LIBS})