file(REMOVE_RECURSE
  "../msg_gen"
  "../msg_gen"
  "../src/MultiColSLAM/msg"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/rosgraph_msgs_generate_messages_py.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
