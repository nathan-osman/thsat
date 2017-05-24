set(SRC
    arduino.cpp
    influxdb.cpp
    main.cpp
)

add_executable(thsat ${SRC})
set_target_properties(thsat PROPERTIES CXX_STANDARD 11)

target_link_libraries(thsat Qt5::Network Qt5::SerialPort)

install(TARGETS thsat RUNTIME DESTINATION bin)
