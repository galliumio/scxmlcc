set( G ${CMAKE_CURRENT_SOURCE_DIR} )

add_executable( gallium_simple_act simple_act.cpp )
scxmlcc_generator(
	INPUTS ${G}/simple_act.scxml
	CLIARGUMENTS --gallium
	TARGETS gallium_simple_act
)
