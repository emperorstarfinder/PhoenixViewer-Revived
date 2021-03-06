# -*- cmake -*-

project(llimagej2coj)

include(00-Common)
include(LLCommon)
include(LLImage)
include(OpenJPEG)

include_directories(
    ${LLCOMMON_INCLUDE_DIRS}
    ${LLIMAGE_INCLUDE_DIRS}
    ${OPENJPEG_INCLUDE_DIR}
    )

if (LIBOPENJPEG2)
  set(llimagej2coj_SOURCE_FILES llimagej2coj2.cpp)
else (LIBOPENJPEG2)
  set(llimagej2coj_SOURCE_FILES llimagej2coj.cpp)
endif (LIBOPENJPEG2)


set(llimagej2coj_HEADER_FILES
    CMakeLists.txt
    llimagej2coj.h
    )

set(COREIMAGE_LIBRARIES)

if (DARWIN)
  list(APPEND llimagej2coj_SOURCE_FILES
    llimagej2cquartz.mm
    )
  list(APPEND llimagej2coj_HEADER_FILES
    llimagej2cquartz.h
    )
  
  find_library(QUARTZCORE_LIBRARY QuartzCore)
  find_library(ACCELERATE_LIBRARY Accelerate)

  list(APPEND COREIMAGE_LIBRARIES ${QUARTZCORE_LIBRARY})
  list(APPEND COREIMAGE_LIBRARIES  ${ACCELERATE_LIBRARY})
endif (DARWIN)

set_source_files_properties(${llimagej2coj_HEADER_FILES}
                            PROPERTIES HEADER_FILE_ONLY TRUE)

list(APPEND llimagej2coj_SOURCE_FILES ${llimagej2coj_HEADER_FILES})

# Pass proper definition to preprocessor to generate shared lib
IF(WIN32)
  IF(BUILD_SHARED_LIBS)
    ADD_DEFINITIONS(-DOPJ_EXPORTS)
  ELSE(BUILD_SHARED_LIBS)
    ADD_DEFINITIONS(-DOPJ_STATIC)
  ENDIF(BUILD_SHARED_LIBS)
ENDIF(WIN32)

add_library (llimagej2coj ${llimagej2coj_SOURCE_FILES})
add_dependencies(llimagej2coj prepare)
target_link_libraries(
    llimagej2coj
    ${OPENJPEG_LIBRARIES}
    ${COREIMAGE_LIBRARIES}
    )
