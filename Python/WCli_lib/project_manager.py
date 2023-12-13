import os
import platform
from WCli_lib import cmake_builder


class ProjectPaths(object):
    BUILD_PATH = os.path.abspath("./build")
    INSTALL_PATH = os.path.abspath("./Install")

    BUILD_FOLDER_FORMAT = "{system}_{arch}_{build_type}_Standalone"

    PROJECT_FOLDER_STRUCTURE = [
        "Source",
        "Assets"
        "Shaders",
        "Docs",
        "cmake"
    ]

    @staticmethod
    def check_dir(path):
        dir_path = os.path.dirname(path)
        if not os.path.exists(dir_path):
            os.makedirs(dir_path)
        
        return path

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

    @staticmethod
    def get_bin_folder(arch, build_type):
        return os.path.join(
            ProjectPaths.INSTALL_PATH,
            ProjectPaths.get_build_folder_name(arch, build_type),
            "bin"
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


class ProjectManager(object):
    def __init__(self, project_path, engine_path=None):
        self.paths = ProjectPaths(project_path, engine_path)
        self.project = os.path.basename(project_path)

    def update_project(self):
        if not os.path.exists(self.paths.project_path):
            os.makedirs(self.paths.project_path)
        
        for folder in ProjectPaths.PROJECT_FOLDER_STRUCTURE:
            folder_path = os.path.join(self.paths.project_path, folder)
            if not os.path.exists(folder_path):
                os.makedirs(folder_path)
        
        cmake_path = self.paths.get_root_cmakelists_path()
        cmake_builder.CMakeBuilder().build(
            cmake_builder.CMakeTemplates.CMakeLists,
            {
                "project": self.project,
                "project_path": self.paths.project_path,
                "engine_path": self.paths.engine_path
            },
            cmake_path
        )


