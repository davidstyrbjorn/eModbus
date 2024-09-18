idf_build_get_property(__hack_component_targets __COMPONENT_TARGETS)

list(APPEND EMODBUS_PRIV_REQUIRES
    bblanchon__arduinojson
    app_update
)

if("arduino" IN_LIST BUILD_COMPONENTS OR __hack_component_targets MATCHES "__idf_arduino")
    list(APPEND EMODBUS_PRIV_REQUIRES
        arduino
    )
elseif("framework-arduinoespressif32" IN_LIST BUILD_COMPONENTS OR __hack_component_targets MATCHES "___idf_framework-arduinoespressif32")
    list(APPEND EMODBUS_PRIV_REQUIRES
        framework-arduinoespressif32
    )
elseif("espressif__arduino-esp32" IN_LIST BUILD_COMPONENTS OR __hack_component_targets MATCHES "___idf_espressif__arduino-esp32")
    list(APPEND EMODBUS_PRIV_REQUIRES
        espressif__arduino-esp32
    )
endif()

# List all the source files
set(SRCS
    "src/ModbusClient.cpp"
    "src/ModbusClientRTU.cpp"
    "src/ModbusClientTCP.cpp"
    "src/ModbusMessage.cpp"
    "src/ModbusServer.cpp"
    "src/ModbusServerRTU.cpp"
    "src/ModbusServerTCP.cpp"
    "src/ModbusTypeDefs.cpp"
    "src/CoilData.cpp"
    "src/RTUutils.cpp"
)

# List all the include directories
set(INCLUDE_DIRS
    "src"
)

# Register the component
idf_component_register(
    SRCS ${SRCS}
    INCLUDE_DIRS ${INCLUDE_DIRS}
    PRIV_REQUIRES ${EMODBUS_PRIV_REQUIRES}
)