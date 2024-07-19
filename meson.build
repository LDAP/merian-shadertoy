project(
    'merian-shadertoy',
    'cpp',
    version : '1.0.0',
    default_options : [
        'warning_level=3',
        'cpp_std=c++20',
        'b_ndebug=if-release',
        'buildtype=release',
    ]
)

# Debug configuration
if get_option('buildtype').startswith('debug')
  add_project_arguments('-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG', language : 'cpp')
endif

if get_option('performance_profiling')
  add_project_arguments('-DMERIAN_PROFILER_ENABLE', language : 'cpp')
endif

# Dependencies
merian = dependency('merian', version: ['>=1.0.0'], fallback: ['merian', 'merian_dep'], default_options: {'shaderc': 'enabled'})

# Main targets
# Keep sorted
src_files = files(
    'src/main.cpp',
)
inc_dirs = [
]

exe = executable(
    'merian-shadertoy',
    src_files,
    dependencies: [
        merian,
    ],
    include_directories: inc_dirs,
    install : true
)
