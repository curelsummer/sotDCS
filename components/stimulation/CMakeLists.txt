idf_component_register(SRCS "sti_common.c"
                            "sti.c"

                    INCLUDE_DIRS "."
                    REQUIRES waveform e_pres sequence timer dac8562 current)

