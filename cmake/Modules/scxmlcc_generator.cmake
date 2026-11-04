# Generate statemachine from input file
# Usage:
#   INPUTS: Input file(s)
#   TARGETS: Name of target(s), which depends on the statemachine
#   CLIARGUMENTS: Optional scxmlcc cli argument(s) (e.g. --debug=clog, --cpp14, --gallium)
#                 When --gallium is present, --stringevents is not added automatically.
#                 Combining --gallium with --baremetal, --threadsafe, --stringevents, or --namespace
#                 in CLIARGUMENTS is a configure error (same rules as scxmlcc).
#
# The generated file(s) are placed within current build folder. Every input
# file produces ${base}.h; with --gallium also ${base}_event_map.h (see main.cpp).
#
# Be aware: You can't use the scxmlcc_generator command with the same INPUTS arguments twice, e.g.:
# scxmlcc_generator(INPUT ${CMAKE_CURRENT_SOURCE_DIR}/microwave-02-cplusplus.scxml TARGETS testlib)
# scxmlcc_generator(INPUT ${CMAKE_CURRENT_SOURCE_DIR}/microwave-02-cplusplus.scxml TARGETS testlib2)
#
# Instead use a single command and specify both targets:
# scxmlcc_generator(INPUT ${CMAKE_CURRENT_SOURCE_DIR}/microwave-02-cplusplus.scxml TARGETS testlib testlib2)

function (scxmlcc_generator )

  set(options "")
  set(oneValueArgs "")
  set(multiValueArgs INPUTS TARGETS CLIARGUMENTS)

  cmake_parse_arguments(PARSED_ARG
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
    ${ARGN}
  )

   # Resolve scxmlcc for add_custom_command COMMAND (target or find_program).
   # When building in-tree, DEPENDS includes $<TARGET_FILE:scxmlcc> so rebuilding the
   # generator reruns codegen without touching .scxml (file dep works on CMake 3.10+).
   # Installed scxmlcc via find_program: no binary path to depend on at configure time.
   if ( TARGET scxmlcc )
      set(SCC scxmlcc )
      set(_scc_dep scxmlcc)
   else()
      find_program( SCC scxmlcc )
      set(_scc_dep "")
   endif()

   if(NOT PARSED_ARG_INPUTS)
     message(FATAL_ERROR "Missing INPUTS parameter for scxmlcc_generator command")
   endif()
   if(NOT PARSED_ARG_TARGETS)
     message(FATAL_ERROR "Missing TARGETS parameter for scxmlcc_generator command")
   endif()

   list(FIND PARSED_ARG_CLIARGUMENTS "--gallium" _gallium_idx)
   list(FIND PARSED_ARG_CLIARGUMENTS "-g" _gallium_short_idx)
   if(_gallium_idx EQUAL -1 AND _gallium_short_idx EQUAL -1)
     set(_stringevents_cli --stringevents)
   else()
     set(_stringevents_cli "")
     foreach(_incompat IN ITEMS --baremetal -b --threadsafe -t --stringevents -s --namespace -n)
       list(FIND PARSED_ARG_CLIARGUMENTS "${_incompat}" _incompat_idx)
       if(NOT _incompat_idx EQUAL -1)
         message(FATAL_ERROR "scxmlcc_generator: --gallium is incompatible with ${_incompat}")
       endif()
     endforeach()
   endif()

   set(outputs "")

   foreach(input ${PARSED_ARG_INPUTS})
     get_filename_component(ext ${input} EXT)
     if(NOT ext STREQUAL ".scxml")
        message("skipping ${input}")
        continue()
     endif()

     get_filename_component(base ${input} NAME_WE)
     set(output ${CMAKE_CURRENT_BINARY_DIR}/${base}.h)

     set(_map_output "")
     if(NOT _gallium_idx EQUAL -1 OR NOT _gallium_short_idx EQUAL -1)
       set(_map_output ${CMAKE_CURRENT_BINARY_DIR}/${base}_event_map.h)
     endif()

     # DEPENDS: chart source; when building in-tree, the scxmlcc binary (file path, not
     # target name — cmake_minimum_required is 3.10; target DEPENDS need 3.16+).
     set(_cmd_deps ${input})
     if(_scc_dep)
       list(APPEND _cmd_deps "$<TARGET_FILE:${_scc_dep}>")
     endif()

     set(_outputs ${output})
     if(NOT _map_output STREQUAL "")
       list(APPEND _outputs ${_map_output})
     endif()

     add_custom_command(
          OUTPUT ${_outputs}
          COMMAND ${SCC} ${PARSED_ARG_CLIARGUMENTS} ${_stringevents_cli} -i ${input}  -o ${output}
          DEPENDS ${_cmd_deps}
     )
     list(APPEND outputs ${_outputs})
   endforeach()

   # Append hash to avoid problems with same named input files
   string(MD5 hash "${PARSED_ARG_INPUTS}")
   set(intermediateTarget "scxmlcc_${base}_${hash}")

   add_custom_target(
        ${intermediateTarget}
        DEPENDS ${outputs}
   )

   foreach(target ${PARSED_ARG_TARGETS})
      add_dependencies(${target} ${intermediateTarget})
   endforeach()



endfunction()
