
INCLUDE_DIRECTORIES( Include ../UFconfig )

SET( C_SOURCE 
	Source/amd_aat.c
	Source/amd_1.c
	Source/amd_2.c
	Source/amd_dump.c
	Source/amd_postorder.c
	Source/amd_post_tree.c
	Source/amd_defaults.c
	Source/amd_order.c
	Source/amd_control.c
	Source/amd_info.c
	Source/amd_valid.c
	Source/amd_preprocess.c
)

ADD_LIBRARY( amd 
${C_SOURCE}
) 
SET_TARGET_PROPERTIES( amd PROPERTIES COMPILE_FLAGS -DDINT )

ADD_LIBRARY( amd_l 
${C_SOURCE}
Source/amd_global.c
) 
SET_TARGET_PROPERTIES( amd_l PROPERTIES COMPILE_FLAGS -DDLONG )