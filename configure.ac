AC_INIT([fighter],[1.1])

AM_INIT_AUTOMAKE([-Wall])

AC_CONFIG_SRCDIR([src/main.c])

AC_CONFIG_FILES([Makefile
                 src/Makefile])

AC_SEARCH_LIBS([pthread_create], [pthread], [], [AC_MSG_ERROR([pthread not found])])

AC_SEARCH_LIBS([SDL_Init], [SDL], [], [AC_MSG_ERROR([SDL not found])])

AC_SEARCH_LIBS([Tcl_CreateInterp], [tcl], [], [AC_MSG_ERROR([tcl not found])])

AC_SEARCH_LIBS([scm_init_guile], [guile], [], [AC_MSG_ERROR([guile not found])])

AC_SEARCH_LIBS([alcOpenDevice], [openal], [], [AC_MSG_ERROR([openal not found])])

AC_SEARCH_LIBS([Mix_OpenAudio], [SDL_mixer], [], [AC_MSG_ERROR([mixer not found])])

AC_SEARCH_LIBS([sf_open], [sndfile], [], [AC_MSG_ERROR([sndfile not found])])

AC_SEARCH_LIBS([alutLoadWAVFile], [alut], [], [AC_MSG_ERROR([alut not found])])

dnl Pay attention to the order vorbisfile, vorbis, ogg

AC_SEARCH_LIBS([ov_open], [vorbisfile], [], [AC_MSG_ERROR([vorbisfile not found])])

AC_SEARCH_LIBS([ov_open], [vorbis], [], [AC_MSG_ERROR([vorbis not found])])

AC_SEARCH_LIBS([ov_open], [ogg], [], [AC_MSG_ERROR([ogg not found])])

dnl AC_SEARCH_LIBS([lua_pushboolean], [lua], [], [AC_MSG_ERROR([lua not found])])

AC_CONFIG_HEADERS([config.h])

AC_PROG_CC

AM_PROG_CC_C_O

AC_ISC_POSIX

AC_HEADER_STDC

AC_OUTPUT
