idf_component_register(SRCS "gatts_server.c"
                    INCLUDE_DIRS "."
                    REQUIRES  sysenv bt nvs_flash monitor stimulation waveform)