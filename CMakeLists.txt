CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(db-plugin C)

### Global setting ###
SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(EXEC_PREFIX "\${prefix}")
SET(LIBDIR "\${prefix}/lib")
SET(INCLUDEDIR "\${prefix}/include")
SET(PKGCONFIGDIR "${PREFIX}/lib/pkgconfig" CACHE PATH PKGCONFIGDIR)
SET(CMAKE_INSTALL_PREFIX "${PREFIX}")

# Set required packages
INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED glib-2.0 tcore db-util)

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)


INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include/)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS}")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wextra -fvisibility=hidden -fPIC")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -O2 -Werror -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wdeclaration-after-statement -Wmissing-declarations -Wredundant-decls -Wcast-align -Wcast-qual -Wall -Wno-array-bounds -Wno-empty-body -Wno-ignored-qualifiers -Wshadow -Wwrite-strings -Wswitch-default -Wno-unused-but-set-parameter -Wno-unused-but-set-variable")

### Purge unused code ###
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdata-sections -ffunction-sections -Wl,--gc-sections") 

ADD_DEFINITIONS("-DFEATURE_TLOG_DEBUG")
ADD_DEFINITIONS("-DTCORE_LOG_TAG=\"SP_DB\"")
ADD_DEFINITIONS("-DPLUGIN_VERSION=${VERSION}")
ADD_DEFINITIONS("-DEXPORT_API=__attribute__((visibility(\"default\")))")

MESSAGE(${CMAKE_C_FLAGS})
MESSAGE(${pkgs_LDFLAGS})

SET(SRCS
		src/database_main.c
)


# library build
ADD_LIBRARY(db-plugin SHARED ${SRCS})
TARGET_LINK_LIBRARIES(db-plugin ${pkgs_LDFLAGS})
SET_TARGET_PROPERTIES(db-plugin PROPERTIES PREFIX "" OUTPUT_NAME db-plugin)



# install
INSTALL(TARGETS db-plugin
		LIBRARY DESTINATION lib/telephony/plugins)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION /usr/share/license RENAME tel-plugin-database)
