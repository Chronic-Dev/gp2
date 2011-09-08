'all': {
    'iPhone3,1_4.3.5': {
        '<': '.armv7',
    },
},
'world1': {
    '.base': {},
    '.armv7': {
        '<':            '.base',
        'arch':         'armv7',
        
        'is_armv7': 1,
        
        '#launchd': {
        # mov r0, #1; bx lr
       -1:              '@ + 01 20 70 47',
        # ldr r0, [r0] -> _launch_data_new_errno
        0:              '+ 00 68 .. .. .. .. 22 46 01 34',
        # lsr r0, r0, #2 -> _setrlimit
        1:              '60 69 29 46 + 80 08',
        # add r0, #3 -> __exit
        2:              '+ 03 30 17 f0 bc ee',
        #2:              'b0 f1 ff 3f .. .. + 03 30 16 f0',
        # ldmia r0, {r0-r3} -> _audit_token_to_au32
        3:              '8d e8 0f 00 0c f1 14 00 + 0f c8',
        # str r2, [sp, #4] -> _launch_data_unpack
        4:              '02 98 00 93 0C 99 13 46 + 01 92',
        # str r3, [sp, #8] -> _launch_data_dict_iterate
        5:              '6A 46 79 44 01 93 01 33 + 02 93',
        # pop {r4, r7, pc}
        6:              '@ + 90 bd',
        # sub.w sp, r7, #0xc; pop {r4-r7, pc}
        7:              '@ + a7 f1 0c 0d f0 bd',
        # mov r0, r4; mov r1, r5; mov r2, r6;
        # blx _strlcpy; pop {r4-r7, pc}
        8:              '16 46 .. .. .. .. .. .. + 20 46 29 46 32 46',
        # str r0, [r5]; pop {r4-r7, pc}
        9:              '@ + 28 60 f0 bd',
        # ldr r0, [r4]; blx _pthread_detach
       10:              '7A 44 20 46 .. .. .. .. 00 28 .. .. + 20 68',
        # mov r0, r6; pop {r4-r7, pc}
       11:              '@ + 30 46 f0 bd',
       
       # mov r0, #0; bx lr  (launchd stat hook so no need to create .launchd_use_gmalloc)
       12:              '@ + 00 20 70 47',
       },
        '#dyld_shared_cache_armv7':{
            # pop {r0-r3, pc}
            'k7': '@ + 0f bd',
            # pop {r4-r7, pc}
            'k11': '@ + f0 bd',
            # blx r4; pop {r4, r5, r7, pc}
            'k12': '@ + a0 47 b0 bd',

        },

    },

    '.armv6': {
        '<':            '.base',
        'arch':         'armv6',
        
        'is_armv7': 0,
        
        '#launchd': {
        # mov r0, #1; bx lr
       -1:              '@ - 01 00 a0 e3 1e ff 2f e1',
        # ldr r0, [r0] -> _launch_data_new_errno
        0:              '- 00 00 90 e5 .. .. .. .. 04 20 a0 e1',
        # lsr r0, r0, #2 -> _setrlimit
        1:              '05 10 a0 e1 - 20 01 a0 e1 .. .. .. .. 01 00 70 e3 04 00 00 1A',
        # add r0, #3 -> __exit
        2:              '01 00 00 .. - 03 00 80 e2',
        # ldmia r0, {r0-r3} -> _audit_token_to_au32
        3:              '14 00 8c e2 - 0f 00 90 e8',
        # str r2, [sp, #4] -> _launch_data_unpack
        4:              '02 30 a0 e1 - 04 20 8d e5',
        # str r3, [sp, #8] -> _launch_data_dict_iterate
        5:              '0d 20 a0 e1 - 08 30 8d e5',
        # pop {r4, r7, pc}
        6:              '@ - 90 80 bd e8',
        # sub.w sp, r7, #0xc; pop {r4-r7, pc}
        7:              '@ - 0c d0 47 e2 f0 80 bd e8',

        # mov r0, r4; mov r1, r6; mov r2, r5;
        # blx _strlcpy; pop {r4-r7, pc}
       -8:              '- 04 00 a0 e1 06 10 a0 e1 05 20 a0 e1 .. .. .. .. f0 80 bd e8',
        # str r0, [r5]; pop {r4-r7, pc}
       -9:              '@ - 00 00 85 e5 b0 80 bd e8',
        # ldr r0, [r4]; blx _pthread_detach
       10:              '00 00 50 e3 .. .. .. .. - 00 00 94 e5 .. .. .. .. 00 10 50 e2',
        # mov r0, r6; pop {r4-r7, pc}
       11:              '@ - 06 00 a0 e1 f0 80 bd e8',
       # mov r0, #0; bx lr  (launchd stat hook so no need to create .launchd_use_gmalloc)
       12:              '@ - 00 00 A0 E3 1E FF 2F E1',
       },
        '#dyld_shared_cache_armv6':{
            # pop {r0-r3, pc}
            'k7': '@ + 0f bd',
            # pop {r4-r7, pc}
            'k11': '@ + f0 bd',
            # blx r4; pop {r4, r5, r7, pc}
            'k12': '@ + a0 47 b0 bd',

        },

    },

},
