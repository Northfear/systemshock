include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_APP_NAME "Shockolate")
set(VITA_TITLEID  "SHOK00001")

set(EXECUTABLE_NAME systemshock)

set(VITA_VERSION "00.78")

set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d ATTRIBUTE2=12")
vita_create_self(${EXECUTABLE_NAME}.self ${EXECUTABLE_NAME})

vita_create_vpk(${EXECUTABLE_NAME}.vpk ${VITA_TITLEID} ${EXECUTABLE_NAME}.self
    VERSION ${VITA_VERSION}
    NAME ${VITA_APP_NAME}
    FILE ${CMAKE_SOURCE_DIR}/vita/sce_sys/icon0.png sce_sys/icon0.png
    FILE ${CMAKE_SOURCE_DIR}/vita/sce_sys/pic0.png sce_sys/pic0.png
    FILE ${CMAKE_SOURCE_DIR}/vita/sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
    FILE ${CMAKE_SOURCE_DIR}/vita/sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
    FILE ${CMAKE_SOURCE_DIR}/vita/sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
)

#add_dependencies(${EXECUTABLE_NAME}.self systemshock)
