# -*- coding: utf-8 -*-
#
# Configuration file for the Sphinx documentation builder.

from pathlib import PurePath

docs = PurePath(__file__).parent

# -- Project information -----------------------------------------------------

version = ''
release = ''
copyright = '2020, Χαροκόπειο Πανεπιστήμιο'
author = 'Νικόλαος Καπονικολός (21628), Ιωάννης Σομός (21685)'
project = 'Παράλληλοι Υπολογιστές και Αλγόριθμοι'

# -- General configuration ---------------------------------------------------

language = 'el'
master_doc = 'report'
source_suffix = '.rst'
pygments_style = 'trac'
templates_path = ['_templates']
exclude_patterns = ['_build', '.venv']
extensions = ['matplotlib.sphinxext.plot_directive']

# -- Options for matplotlib --------------------------------------------------

plot_pre_code = ''
plot_include_source = False
plot_html_show_formats = False
plot_html_show_source_link = False
plot_basedir = str(docs / 'plots')
plot_working_directory = str(docs)

# -- Options for HTML output -------------------------------------------------

html_title = ''
html_theme = 'alabaster'
html_theme_options = {'nosidebar': True}

# -- Options for Latex output ------------------------------------------------

today_fmt = r'''
Χαροκόπειο Πανεπιστήμιο \linebreak
Τμήμα Πληροφορικής και Τηλεματικής \linebreak
\linebreak %d %B %Y
'''
latex_engine = 'xelatex'
latex_elements = {
    'papersize': 'a4paper',
    'pointsize': '13pt',
    'figure_align': 'htbp',
    'tableofcontents': '',
    'extraclassoptions': 'openany,oneside',
    'fncychap': r'''
     \usepackage[Bjornstrup]{fncychap}
    ''',
    'babel': r'''
     \usepackage[greek,english]{babel}
    ''',
    'fontpkg': r'''
     \setmainfont{Lato}
     \setsansfont{Lato}
     \setmonofont{Hack}
    ''',
}
latex_documents = [(
    'report', 'Report.tex',
    project, author, 'report'
)]
