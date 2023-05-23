
# edit subfolder, put your examples here
set(EXAMPLES 
        take_picture #TODO replace by glob
        simple_timing
        take_picture_rgb
        take_picture_dynamic
        take_picture_dynamic_crop
    )

# add all the examples that you need
foreach(EXAMPLE ${EXAMPLES})
    include(${CMAKE_CURRENT_LIST_DIR}/${EXAMPLE}/${EXAMPLE}.cmake)
endforeach(EXAMPLE)
