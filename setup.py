import os
import re
import sys
from glob import glob

# https://github.com/pypa/setuptools/issues/1347
from os.path import abspath, dirname, join, normpath, relpath
from shutil import rmtree

from setuptools import Command, setup
from setuptools.command.install import install
from setuptools.extension import Extension

here = normpath(abspath(dirname(__file__)))


class CleanCommand(Command):
    """Custom clean command to tidy up the project root."""

    CLEAN_FILES = (
        "./build ./dist ./*.pyc ./*.tgz ./*.egg-info ./__pycache__ ./*.so".split(" ")
    )

    # Support the "all" option. Setuptools expects it in some situations.
    user_options = [
        ("all", "a", "provided for compatibility, has no extra functionality")
    ]

    boolean_options = ["all"]

    def initialize_options(self):
        self.all = None

    def finalize_options(self):
        pass

    def run(self):
        global here

        for path_spec in self.CLEAN_FILES:
            # Make paths absolute and relative to this path
            abs_paths = glob(normpath(join(here, path_spec)))
            for path in [str(p) for p in abs_paths]:
                if not path.startswith(here):
                    # Die if path in CLEAN_FILES is absolute + outside this directory
                    raise ValueError("{} is not a path inside {}".format(path, here))
                print("removing {}".format(relpath(path)))
                try:
                    rmtree(path)
                except:
                    os.remove(path)


deps = []


class InstallCommand(install):
    description = "Adds custom flags to normal install."
    user_options = install.user_options + [
        ("no-deps", None, "Do not install runtime dependencies."),
    ]

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

    def initialize_options(self):
        super().initialize_options()
        self.no_deps = False

    def finalize_options(self):
        super().finalize_options()
        global deps
        self.no_deps = bool(self.no_deps)
        if self.no_deps:
            deps = []
        else:
            deps = runtime_deps


try:
    from Cython.Build import cythonize
    import numpy
    import pybind11  # noqa: F401
except ImportError:
    print("numpy/cython/pybind11 are not installed:")
    print(">> pip install numpy cython pybind11")
    exit(1)

with open("recipe/meta.yaml", "r") as f:
    config = f.read()
    name = re.search('{% set name = "(.+)" %}', config).group(1)
    version = re.search('{% set version = "([.\d]+)" %}', config).group(1)

DISTNAME = name
VERSION = version
DESCRIPTION = "Noise contrastive data visualization"
with open("SHORT_README.md", "r") as f:
    LONG_DESCRIPTION = f.read()
MAINTAINER = "Aleksandr Artemenkov"
MAINTAINER_EMAIL = "alartum@gmail.com"
URL = "https://github.com/stat-ml/ncvis"
LICENSE = "MIT"
PROJECT_URLS = {"Source Code": "https://github.com/stat-ml/ncvis"}

# Can be excluded from the install dependencies with --no-deps
runtime_deps = ["scipy"]
# Add all sources except main
src = glob("src/*.cpp")
try:
    src.remove("src/main.cpp")
except ValueError:
    pass


if sys.platform.startswith("darwin"):
    extra_compile_args = [
        "-O3",
        "-std=c++14",
        "-fpic",
        "-ffast-math",
        "-fopenmp=libiomp5",
    ]
    libraries = ["m", "iomp5"]
elif sys.platform.startswith("linux"):
    extra_compile_args = ["-O3", "-std=c++14", "-ffast-math", "-fopenmp"]
    libraries = ["m", "gomp"]
elif sys.platform.startswith("win32"):
    extra_compile_args = ["/O2", "/fp:fast", "/openmp"]
    libraries = ["/openmp"]

extensions = [
    Extension(
        "ncvis",
        ["wrapper/*.pyx", *src],
        extra_compile_args=extra_compile_args,
        libraries=libraries,
        include_dirs=[numpy.get_include()],
        language="c++",
        define_macros=[("NPY_NO_DEPRECATED_API", "NPY_1_7_API_VERSION")],
    )
]

metadata = dict(
    name=DISTNAME,
    maintainer=MAINTAINER,
    maintainer_email=MAINTAINER_EMAIL,
    description=DESCRIPTION,
    license=LICENSE,
    url=URL,
    project_urls=PROJECT_URLS,
    version=VERSION,
    long_description=LONG_DESCRIPTION,
    long_description_content_type="text/markdown",
    classifiers=[
        "Intended Audience :: Science/Research",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: C++",
        "Programming Language :: Python :: 3",
        "Operating System :: OS Independent",
    ],
    install_requires=deps,
    python_requires=">=3",
    cmdclass={
        "clean": CleanCommand,
        "install": InstallCommand,
    },
)

setup(ext_modules=cythonize(extensions), **metadata)
