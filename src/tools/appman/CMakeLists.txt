
qt_get_tool_target_name(target_name appman)
qt_internal_add_tool(${target_name}
    USER_FACING
    EXCEPTIONS
    TOOLS_TARGET AppManMainPrivate
    SOURCES
        appman.cpp
    PUBLIC_LIBRARIES
        Qt::AppManMainPrivate
)

if (TARGET ${target_name})
    include(QtAppManBuildConfig)

    qtam_internal_add_build_config(${target_name})
endif()
