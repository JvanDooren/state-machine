set(TARGET vendingmachine)
add_executable(${TARGET} main.cc)

target_link_libraries(${TARGET} PRIVATE
    statemachine-core
)
