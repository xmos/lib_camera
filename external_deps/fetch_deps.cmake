include(FetchContent)

FetchContent_Declare(
    fwk_io
    GIT_REPOSITORY https://github.com/xmos/fwk_io
    GIT_TAG        develop
    GIT_SHALLOW    FALSE
    SOURCE_DIR     ${EXT_DEPS_DIR}/fwk_io
)
FetchContent_Populate(fwk_io)
