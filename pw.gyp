{
    "variables": {
        "pw_dir%": "."
    },
    "includes": [
        "common.gypi"
    ],
    "targets": [
        {
            "target_name": "pw",
            "type": "executable",
            "sources": [
                "<(pw_dir)/src/main.c",
                "<(pw_dir)/src/audio.h",
                "<(pw_dir)/src/audio.c",
                "<(pw_dir)/src/background.h",
                "<(pw_dir)/src/background.c",
                "<(pw_dir)/src/music.h",
                "<(pw_dir)/src/music.c",
                "<(pw_dir)/src/network.h",
                "<(pw_dir)/src/network.c",
                "<(pw_dir)/src/particle.h",
                "<(pw_dir)/src/particle.c",
                "<(pw_dir)/src/radar.h",
                "<(pw_dir)/src/radar.c",
                "<(pw_dir)/src/resources.h",
                "<(pw_dir)/src/resources.c",
                "<(pw_dir)/src/scripting.h",
                "<(pw_dir)/src/scripting.c",
                "<(pw_dir)/src/status.h",
                "<(pw_dir)/src/status.c",
                "<(pw_dir)/src/weapon.h",
                "<(pw_dir)/src/weapon.c",
                "<(pw_dir)/src/font5x5.h",
                "<(pw_dir)/src/gamedefs.h"
            ],
            "include_dirs": [
            ],
            "dependencies": [
            ],
            "cflags!": [
                "<!(sdl-config --cflags)",
                "<!(guile-config compile)"
            ],
            "conditions": [
                ['OS=="linux"', {
                    'libraries': [
                        "-lopenal",
                        "-lm",
                        "<!(sdl-config --libs)",
                        "<!(guile-config link)",
                        "-ltcl",
                        "-lsndfile",
                        "-lvorbisfile",
                        "-logg"
                    ]
                }],
                ['OS=="mac"', {
                    'libraries': [
                        "-framework OpenAL",
                        "-lm",
                        "<!(sdl-config --libs)",
                        "<!(guile-config link)",
                        "-ltcl",
                        "-lsndfile",
                        "-lvorbisfile",
                        "-logg"
                ]
                }]
            ]
        }
    ]
}
