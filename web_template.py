Import("env")

try:
    import jinja2
except ImportError:
    env.Execute("$PYTHONEXE -m pip install jinja2")
    import jinja2

from glob import glob
import os

def pre_spiffs_callback(target, source, env):
    template_dir = 'src_data/'
    loader = jinja2.FileSystemLoader(template_dir)
    environment = jinja2.Environment(loader=loader)


    for page in glob("src_data/*.j2"):

        page_name = os.path.basename(page)

        template = environment.get_template(page_name)

        with open("data/" + page_name.replace('.j2', ''), 'w') as f:
            f.write(template.render(the="variables", go="here"))
            print("Created: " + page_name.replace('.j2', ''))

env.AddPreAction("$BUILD_DIR/spiffs.bin", pre_spiffs_callback)



