
qt_get_tool_target_name(target_name appman-packager)
qt_internal_add_tool(${target_name}
    USER_FACING
    EXCEPTIONS
    TOOLS_TARGET AppManMainPrivate
    SOURCES
        packager.cpp
        packagingjob.cpp packagingjob.h
    PUBLIC_LIBRARIES
        Qt::Network
        Qt::AppManApplicationPrivate
        Qt::AppManCommonPrivate
        Qt::AppManCryptoPrivate
        Qt::AppManPackagePrivate
)
