
qt_get_tool_target_name(target_name package-uploader)
qt_internal_add_tool(${target_name}
    USER_FACING
    EXCEPTIONS
    TOOLS_TARGET AppManMainPrivate
    SOURCES
        uploader.cpp
    PUBLIC_LIBRARIES
        Qt::Network
        Qt::AppManCommonPrivate
)
