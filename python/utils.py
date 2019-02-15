


import os

def load_file(filename, default_ext=".???"):
    file_lines = []

    # add presumed extension
    if not filename.endswith(default_ext):
        filename = filename + default_ext

    # save a path to the file for use later
    # file_path = os.path.dirname(filename)
    # this would be helpful if processing includes

    with open(filename) as file:
        file_lines = [line.strip() for line in file]

    return file_lines


