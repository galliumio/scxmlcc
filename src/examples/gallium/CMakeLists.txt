set( G ${CMAKE_CURRENT_SOURCE_DIR} )

add_executable( gallium_hello_world hello_world.cpp )
scxmlcc_generator(
	INPUTS ${G}/hello_world.scxml
	CLIARGUMENTS --gallium
	TARGETS gallium_hello_world
)
