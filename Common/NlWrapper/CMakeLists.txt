set(INCLUDE_FILES
    "NlWrapper.h" "NlWrapper.cpp"
)

set(INCLUDE_LIBS
    nl-3
)

add_library(NlWrapper ${INCLUDE_FILES})
target_link_libraries(NlWrapper PRIVATE ${INCLUDE_LIBS})