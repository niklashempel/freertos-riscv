set(srcs "attack.c")
idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES console esp_timer)

# Disable stack protection for vulnerability demonstration
target_compile_options(${COMPONENT_LIB} PRIVATE 
    -fno-stack-protector
    -O0  # No optimization for predictable stack layout
)
