file(REMOVE_RECURSE
  "../msg_gen"
  "../msg_gen"
  "../src/MultiColSLAM/msg"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/MultiColSLAM_gennodejs.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
