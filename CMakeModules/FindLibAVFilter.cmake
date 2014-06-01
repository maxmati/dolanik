
FIND_PATH(AVFILTER_INCLUDE_DIR libavfilter/avfilter.h PATHS /usr/include/ /usr/local/include/)

FIND_LIBRARY(AVFILTER_LIBRARY NAMES avfilter PATH /usr/lib /usr/local/lib) 

INCLUDE(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set AVCODEC_FOUND to TRUE if 
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(avfilter
  REQUIRED_VARS AVFILTER_LIBRARY AVFILTER_INCLUDE_DIR)

IF(AVFILTER_FOUND)
SET(AVFILTER_INCLUDE_DIRS ${AVFILTER_INCLUDE_DIR})
SET(AVFILTER_LIBRARIES ${AVFILTER_LIBRARY})
ELSE(AVFILTER_FOUND)
MESSAGE(SEND_ERROR "Could not find libavfilter - (get it at http://ffmpeg.org)")
ENDIF(AVFILTER_FOUND)
