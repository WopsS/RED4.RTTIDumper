option(JSON_MultipleHeaders "" ON)

add_subdirectory(deps/json)

mark_as_advanced(
  JSON_BuildTests
  JSON_CI
  JSON_Coverage
  JSON_Diagnostics
  JSON_ImplicitConversions
  JSON_Install
  JSON_MultipleHeaders
  JSON_SystemInclude
)
