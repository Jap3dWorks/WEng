import jinja2
import os

# https://jinja.palletsprojects.com/en/3.0.x/templates/
# https://jinja.palletsprojects.com/en/3.0.x/api/

class CMakeTemplates:
    CMakeLists = "CMakeLists.template"
    FindModule = "FindModule.template"


def _check_dir(path) -> str:
    dir_path = os.path.dirname(path)
    if not os.path.exists(dir_path):
        os.makedirs(dir_path)
    
    return path

class CMakeBuilder(object):
    TEMPLATES_FOLDER = os.path.abspath(
        os.path.dirname(__file__) + "/templates"
    )
    
    def get_template_path(self, template_name) -> str:
        return os.path.join(
            CMakeBuilder.TEMPLATES_FOLDER,
            template_name
        )

    def get_template(self, template_name) -> jinja2.Template:
        template_path = self.get_template_path(template_name)

        if not os.path.exists(template_path):
            raise FileExistsError(
                "Template not found: " + template_path
            )

        with open(template_path) as file:
            return jinja2.Template(
                file.read()
            )

    def __init__(self) -> None:
        pass

    def build(self, template, data, output_path) -> None:
        template = self.get_template(template)

        with open(_check_dir(output_path), "w") as file:
            file.write(
                template.render(data)
            )


