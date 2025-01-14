# Template to make a tool
function(define_composer_tool TOOL_NAME TOOL_SOURCES TOOL_HEADERS DEPS)

  project(pcl_cc_tool_${TOOL_NAME})

  #MESSAGE ("Making plugin " pcl_cc_tool_${TOOL_NAME})
  QT4_WRAP_CPP(TOOL_HEADERS_MOC ${TOOL_HEADERS})
  set(TOOL_TARGET pcl_cc_tool_${TOOL_NAME})
  # MESSAGE ("Files:"  ${TOOL_SOURCES} ${TOOL_HEADERS_MOC})
  PCL_ADD_LIBRARY(${TOOL_TARGET} ${SUBSYS_NAME} ${TOOL_SOURCES} ${TOOL_HEADERS} ${TOOL_HEADERS_MOC})
  if(WIN32)
    set_target_properties (${TOOL_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CLOUD_COMPOSER_PLUGIN_DIR}
                                                    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CLOUD_COMPOSER_PLUGIN_DIR})
  else(WIN32)
    set_target_properties (${TOOL_TARGET} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CLOUD_COMPOSER_PLUGIN_DIR})
  endif(WIN32)
  ADD_DEFINITIONS(${QT_DEFINITIONS})
  ADD_DEFINITIONS(-DQT_PLUGIN)
  ADD_DEFINITIONS(-DQT_NO_DEBUG)
  ADD_DEFINITIONS(-DQT_SHARED)

  add_dependencies(${TOOL_TARGET} pcl_cc_tool_interface ${DEPS})
  target_link_libraries(${TOOL_TARGET} pcl_cc_tool_interface pcl_common pcl_io ${DEPS} ${QT_LIBRARIES})

endfunction(define_composer_tool TOOL_NAME TOOL_SOURCES TOOL_HEADERS DEPS)
