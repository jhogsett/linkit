
# language definitions
get_variable_delimiters       = {"<" : ">"}
set_macro_delimiters          = {"[" : "]"}
run_macro_delimiters          = {"(" : ")"}
allocate_sequencer_delimiters = {"{" : "}"}
python_delimiters             = {"`" : "`"}
place_template_delimiters     = {"((" : "))"}
place_template_grouping       = {"`" : "`" }           # keep python expression contents together in template arguments
meta_template_delimiters      = {"(((" : ")))"}
meta_template_grouping        = {"[" : "]", "`" : "`"} # keep set macro arguments and python expressions together in meta-template arguments
multi_macro_delimiters        = {"[[[" : "]]]"}
refresh_block_start           = "{{{"
refresh_block_end             = "}}}"
reset_block_start             = "{{"
reset_block_end               = "}}"

# compiler use
proxy_macro_delimiters = {"'" : "'"}

# helpers
macro_marker_delimiters = {"(" : ")", "[" : "]"}
template_marker_delimiters = {"((" : "))", "[[" : "]]"}

