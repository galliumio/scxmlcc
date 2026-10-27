set( G ${CMAKE_CURRENT_SOURCE_DIR} )

add_executable( gallium_hello_world hello_world.cpp )
scxmlcc_generator(
	INPUTS ${G}/hello_world.scxml
	CLIARGUMENTS --gallium
	TARGETS gallium_hello_world
)

add_executable( gallium_four_states four_states.cpp )
scxmlcc_generator(
	INPUTS ${G}/four_states.scxml
	CLIARGUMENTS --gallium
	TARGETS gallium_four_states
)

add_subdirectory( simple_act )
