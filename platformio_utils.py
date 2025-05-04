Import("env", "projenv")

try:
    import jinja2
except ImportError:
    env.Execute("$PYTHONEXE -m pip install jinja2")
    import jinja2

from glob import glob
import os
import shutil
import os.path
import subprocess

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

    if(os.path.exists("data/config.txt") == False):
        shutil.copyfile("data/config.default", "data/config.txt")

env.AddPreAction("$BUILD_DIR/spiffs.bin", pre_spiffs_callback)

git_commit_hash = (
    subprocess.check_output(['git', 'show', '--pretty=format:%h', '--no-patch'])
    .strip()
    .decode("utf-8")
)

git_status = (
    subprocess.check_output(['git', 'status', '--porcelain'])
    .strip()
    .decode("utf-8")
)

git_version = git_commit_hash

if git_status:
    git_version += "-dirty"

print("Git version: " + git_version)

projenv.Append(CPPDEFINES=[
  ("GIT_VERSION", env.StringifyMacro(git_version))
])