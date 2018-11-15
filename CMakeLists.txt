set(COMPONENT_ADD_INCLUDEDIRS
    "include/periph"
    "include/periph/display"
    "include/periph/font"
    "include/periph/sensor"
)

set(COMPONENT_PRIV_INCLUDEDIRS "include")

set(COMPONENT_SRCS
    "periph"
    "periph/display"
    "periph/font"
    "periph/sensor"
)

set(COMPONENT_REQUIRES driver)

register_component()