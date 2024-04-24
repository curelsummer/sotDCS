idf_component_register( SRCS "wave_common.c"
                             "./tDCS/tdcs.c"
                             "./tACS/tacs.c"
                             "./CES/ces.c"
                             "./rtACS/rtacs.c"

                        INCLUDE_DIRS "."
                                     "tDCS"
                                     "tACS"
                                     "CES"
                                     "rtACS")

