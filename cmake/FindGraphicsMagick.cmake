FIND_PATH(GraphicsMagick_INCLUDE_DIR NAMES Magick++.h
    HINTS
    $ENV{GRAPHICSMAGICKDIR}
    PATH_SUFFIXES include GraphicsMagick graphicsmagick include/GraphicsMagick include/graphicsmagick
    PATHS
    /usr
    /usr/include
    /usr/include/GraphicsMagick
    /usr/include/graphicsmagick
    /usr/local
    /usr/local/include
    /usr/local/include/GraphicsMagick
    /usr/local/include/graphicsmagick
    /opt
    /opt/graphicsmagick
    /opt/GraphicsMagick
)

SET(GraphicsMagick_LIBRARIES)

IF(GraphicsMagick_FIND_COMPONENTS)
    FOREACH(component ${GraphicsMagick_FIND_COMPONENTS})
        FIND_LIBRARY(GraphicsMagick_LIBRARY_${component} NAMES ${component}
            HINTS
            $ENV{GRAPHICSMAGICKDIR}
            PATH_SUFFIXES lib graphicsmagick GraphicsMagick
            PATHS
            /usr
            /usr/lib
            /usr/lib/graphicsmagick
            /usr/lib/GraphicsMagick
            /usr/lib64
            /usr/lib64/graphicsmagick
            /usr/lib64/GraphicsMagick
            /usr/local
            /usr/local/lib
            /usr/local/lib/graphicsmagick
            /usr/local/lib/GraphicsMagick
            /opt
            /opt/graphicsmagick
            /opt/GraphicsMagick
        )
        LIST(APPEND GraphicsMagick_LIBRARIES ${GraphicsMagick_LIBRARY_${component}})
    ENDFOREACH(component)
ENDIF(GraphicsMagick_FIND_COMPONENTS)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GraphicsMagick DEFAULT_MSG GraphicsMagick_LIBRARIES GraphicsMagick_INCLUDE_DIR)

IF(GRAPHICSMAGICK_FOUND)
    SET(GraphicsMagick_INCLUDE_DIRS ${GraphicsMagick_INCLUDE_DIR})
ELSE(GRAPHICSMAGICK_FOUND)
    SET(GraphicsMagick_INCLUDE_DIRS)
ENDIF(GRAPHICSMAGICK_FOUND)

