import os
import platform
from wcli_lib import cmake_builder
import re


class ProjectPaths(object):
    BUILD_PATH = os.path.abspath("./build")  # DEPRECATED
    INSTALL_PATH = os.path.abspath("./Install")  # DEPRECATED

    BUILD_FOLDER_FORMAT = "{system}_{arch}_{build_type}_Standalone"

    SOURCE_DIR = "Source"
    ASSETS_DIR = "Assets"
    SHADERS_DIR = "Shaders"
    DOCS_DIR = "Docs"
    CMAKE_DIR = "cmake"
    INCLUDE_DIR = "Include"
    TEST_DIR = "Tests"

    PROJECT_FOLDER_STRUCTURE = [
        "Source",
        "Assets",
        "Shaders",
        "Docs",
        "cmake"
    ]

    MODULE_STRUCTURE = [
        "Shaders",
        "Assets",
        "Docs",
        "Tests",
        "Source/{module_name}.cpp",
        "Include/{module_name}.h",
        "README.md"
    ]

    @staticmethod
    def check_dir(path):
        dir_path = os.path.dirname(path)
        if not os.path.exists(dir_path):
            os.makedirs(dir_path)
        
        return path

    # build paths
    @staticmethod
    def get_build_folder_name(arch, build_type):
        return ProjectPaths.BUILD_FOLDER_FORMAT.format(
            system=platform.system(),
            arch=arch,
            build_type=build_type
        )
    
    @staticmethod
    def get_build_folder(arch, build_type, build_path):
        return os.path.join(
            build_path,
            ProjectPaths.get_build_folder_name(arch, build_type)
        )
    
    @staticmethod
    def get_build_source_folder(arch, build_type, build_path):
        return os.path.join(
            ProjectPaths.get_build_folder(arch, build_type, build_path),
            "Source"
        )

    @staticmethod
    def get_build_target_path(arch, build_type, target, build_path):
        return os.path.join(
            ProjectPaths.get_build_source_folder(arch, build_type, build_path),
            target,
            target
        )

    # install paths
    @staticmethod
    def get_install_path(arch, build_type):
        return os.path.join(
            ProjectPaths.INSTALL_PATH,
            ProjectPaths.get_build_folder_name(arch, build_type)
        )
    
    @staticmethod
    def get_bin_folder(arch, build_type):
        return os.path.join(
            ProjectPaths.INSTALL_PATH,
            ProjectPaths.get_build_folder_name(arch, build_type),
            "bin"
        )

    @staticmethod
    def get_lib_folder(arch, build_type):
        return os.path.join(
            ProjectPaths.INSTALL_PATH,
            ProjectPaths.get_build_folder_name(arch, build_type),
            "lib"
        )

    @staticmethod
    def get_target_bin_path(arch, build_type, target):
        return os.path.join(
            ProjectPaths.get_bin_folder(arch, build_type),
            target
        )
    
    def __init__(self, project_path, engine_path=None):
        self.project_path = project_path
        self.engine_path = engine_path

        self.engine_binaries_paths = None
        self.engine_headers_paths = None
    
    def _get_modules_paths(self, root):
        source_path = os.path.join(root, "Source")
        for module in os.listdir(source_path):
            cmake_file = os.path.join(source_path, module, "CMakeLists.txt")
            if os.path.exists(cmake_file):
                yield os.path.join(source_path, module)

    def get_engine_modules_paths(self):
        assert self.engine_path is not None, "Engine path not set"

        return self._get_modules_paths(self.engine_path)

    def get_project_modules_paths(self):
        return self._get_modules_paths(self.project_path)

    def get_root_cmakelists_path(self):
        return os.path.join(self.project_path, "CMakeLists.txt")

    def get_module_path(self, module_name):
        return os.path.join(self.project_path, "Source", module_name)

class ProjectManager(object):
    def __init__(self, project_path, engine_path=None):
        self.paths = ProjectPaths(project_path, engine_path)
        self.project = os.path.basename(project_path)

    def update_root_cmakelists(self, cmake_path=None, force=False):
        cmake_path = cmake_path or self.paths.get_root_cmakelists_path()

        if os.path.exists(cmake_path) and not force:
            return

        cmake_builder.CMakeBuilder().build(
                cmake_builder.CMakeTemplates.ProjectCMakeLists,
                {
                    "project": self.project,
                    "project_path": self.paths.project_path,
                    "engine_path": self.paths.engine_path
                },
                cmake_path
            )

    def update_module(self, module_name, force=False):
        module_path = self.paths.get_module_path(module_name)
        if not os.path.exists(module_path):
            os.makedirs(module_path)
        
        cmake_path = os.path.join(module_path, "CMakeLists.txt")
        if (not os.path.exists(cmake_path)) or force:
            cmake_builder.CMakeBuilder().build(
                cmake_builder.CMakeTemplates.ModuleCMakeLists,
                {
                    "module_name": module_name,
                    "project": self.project,
                    "project_path": self.paths.project_path,
                    "engine_path": self.paths.engine_path
                },
                cmake_path
            )

        file_pattern = re.compile(r"[.][^.]+$")

        for structure_path in self.paths.MODULE_STRUCTURE:
            structure_path = structure_path.format(module_name=module_name)
            structure_path = os.path.join(module_path, structure_path)
            
            if os.path.exists(structure_path) and not force:
                continue

            if file_pattern.search(structure_path):                
                with open(ProjectPaths.check_dir(structure_path), "w") as file:
                    file.write("")
            else:
                os.makedirs(structure_path)

    def update_project(self):
        if not os.path.exists(self.paths.project_path):
            os.makedirs(self.paths.project_path)
        
        for folder in ProjectPaths.PROJECT_FOLDER_STRUCTURE:
            folder_path = os.path.join(self.paths.project_path, folder)
            if not os.path.exists(folder_path):
                os.makedirs(folder_path)
        
        self.update_root_cmakelists()

        self.update_module(self.project)

