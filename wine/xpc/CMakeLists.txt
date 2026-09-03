# MATLAB R2015b xPC api
add_library(xpcapi
        xpcapi.dll
        xpcinitfree.c
)
target_include_directories(xpcapi PUBLIC include)
install(
        FILES
            xpcapi.dll
            AM2_3_1_01HWfullV2_PCIe.dlm
        DESTINATION lib/${PROJECT_NAME}
)