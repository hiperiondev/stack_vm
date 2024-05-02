project = 'Stack-VM'
copyright = '2024, Emiliano Augusto Gonzalez (egonzalez-hiperion@gmail.com)'
author = 'Emiliano Augusto Gonzalez (LU3VEA)'
release = '2.1.1'

extensions = [
        'sphinx.ext.autodoc',
        'sphinx.ext.doctest',
        'sphinx.ext.mathjax',
        'sphinx.ext.viewcode',
        'sphinx.ext.imgmath', 
        'sphinx.ext.todo',
        'breathe',
]

exclude_patterns = ['conf.py', 'Doxyfile']

import os
on_rtd = os.environ.get('READTHEDOCS', None) == 'True'

import subprocess
subprocess.call('doxygen', shell=True)

import sphinxawesome_theme
html_theme = 'sphinxawesome_theme'
html_logo = '../images/logo.png'
#html_theme_options = {
#    "logo_light": "../images/logo.png",
#    "logo_dark": "../images/logo.png"
#}
html_favicon = '../images/favicon-32x32.png'
source_suffix = {
    '.rst': 'restructuredtext',
}

# Breathe Configuration
breathe_projects = {"StackVN" : "../vm"}
breathe_default_project = "stack-vm"
breathe_projects_source = {
    "stack-vm" : (
        "../", ["vm/vm.h"]
     )
}

# exclude prefix
c_id_attributes = ["VM_API"]
c_paren_attributes = ["VM_API"]

cpp_id_attributes = ["VM_API"]
cpp_paren_attributes = ["VM_API"]

# add lexer for highlighting
#import sys
#def setup(sphinx):
#    sys.path.insert(0, os.path.abspath('../tools/highlighters/Pygments'))
#    from berry import BerryLexer
#    sphinx.add_lexer("lexerfile", BerryLexer)
#
#try:
#    import sphinxawesome_theme
#    is_imported = True
#except:
#    is_imported = False
#if is_imported:
#    version = sphinxawesome_theme.__version__
#    release = sphinxawesome_theme.__version__
#    html_theme = 'sphinxawesome_theme'
