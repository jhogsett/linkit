import utils

comment_marker           = "#"
set_variable_marker      = "$"
directive_marker         = "%"
configuration_marker     = "="
final_macro_marker       = "!"
get_variable_start       = "<"
get_variable_end         = ">"
set_macro_start          = "["
set_macro_end            = "]"
run_macro_start          = "("
run_macro_end            = ")"
allocate_sequencer_start = "{"
allocate_sequencer_end   = "}"
python_start             = "`"
python_end               = "`"
proxy_macro_start        = "'"
proxy_macro_end          = "'"
set_template_start       = "[["
set_template_end         = "]]"
expand_template_start    = "(("
expand_template_end      = "))"
reset_block_start        = "{{"
reset_block_end          = "}}"
meta_template_start      = "((("
meta_template_end        = ")))"
multi_macro_start        = "[[["
multi_macro_end          = "]]]"
conditional_start        = "<<<"
conditional_end          = ">>>"
refresh_block_start      = "{{{"
refresh_block_end        = "}}}"

set_template_non_start         = multi_macro_start
set_template_non_end           = multi_macro_end
expand_template_non_start      = meta_template_start
expand_template_non_end        = meta_template_end

get_variable_delimiters        = {get_variable_start : get_variable_end}
set_macro_delimiters           = {set_macro_start : set_macro_end}
run_macro_delimiters           = {run_macro_start : run_macro_end}
allocate_sequencer_delimiters  = {allocate_sequencer_start : allocate_sequencer_end}
python_delimiters              = {python_start : python_end}
proxy_macro_delimiters         = {proxy_macro_start : proxy_macro_end}

set_template_delimiters        = {set_template_start : set_template_end}
expand_template_delimiters     = {expand_template_start : expand_template_end}
reset_block_delimiters         = {reset_block_start : reset_block_end}
double_angle_delimiters        = {"<<" : ">>"}   # future use

meta_template_delimiters       = {meta_template_start : meta_template_end}
multi_macro_delimiters         = {multi_macro_start : multi_macro_end}
conditional_delimiters         = {conditional_start : conditional_end}
refresh_block_delimiters       = {refresh_block_start : refresh_block_start}

empty_delimiters               = {}
empty_delimiter                = []
empty_python                   = python_delimiters.keys()[0] + python_delimiters.values()[0]

include_marker                 = directive_marker + "include"
allow_mutability_marker        = directive_marker + "allow-mutability"

# for isolating shorter delimiters
get_variable_non_delimiters    = utils.merge_dicts(double_angle_delimiters, conditional_delimiters)
set_macro_non_delmiters        = utils.merge_dicts(set_template_delimiters, multi_macro_delimiters)
run_macro_non_delimiters       = utils.merge_dicts(expand_template_delimiters, meta_template_delimiters)
set_template_non_delimiters    = multi_macro_delimiters
expand_template_non_delimiters = meta_template_delimiters

# for proper segmentation while parsing lines
expand_template_grouping       = python_delimiters                                          # keep python expression contents together in template arguments
meta_template_grouping         = utils.merge_dicts(set_macro_delimiters, python_delimiters) # keep set macro arguments and python expressions together

# for locating by type for renaming
macro_marker_delimiters        = utils.merge_dicts(run_macro_delimiters, set_macro_delimiters)
template_marker_delimiters     = utils.merge_dicts(expand_template_delimiters, set_template_delimiters)
meta_and_multi_delimiters      = utils.merge_dicts(meta_template_delimiters, multi_macro_delimiters)

