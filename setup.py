import sys
from setuptools import setup, find_packages, Extension
from os import path, makedirs
from shutil import copy
import platform

here = path.abspath(path.dirname(__file__))

with open(path.join(here, 'README.md'), encoding='utf-8') as f:
    long_description = f.read()

#makedirs("./hybris/lib", exist_ok=True)
#copy(f"./bin/{platform.system()}/libhybris.so", "./hybris/lib/libhybris.so")
#package_data = ['lib/libhybris.so']

py_ver = sys.version_info
with open("requirements.txt", "r") as f:
    requirements = list(f.readlines())
if py_ver[1] < 5:
    requirements.append('typing')
if py_ver[1] < 4:
    requirements.append('enum')

setup(
    name='hybris-py',
    version='0.1.1',
    python_requires='>=3.3, <4',
    ext_modules=[Extension("hybris.chybris", sources=["chybris/chybris.c"], define_macros=[("ADD_CECBENCHMARK", None)])],
    description='A Python binding for Hybris PSO Optimizer',
    long_description=long_description,
    long_description_content_type='text/markdown',
    url='https://github.com/kaeryv/hybris-py',
    author='Kaeryv',
    author_email='nicolas.roy@unamur.be',
    classifiers=[  
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Developers',
        'Topic :: Games/Entertainment',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3 :: Only',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
    ],

    keywords='pso easy-to-use fuzzy optimizer discrete continuous',

    packages=['hybris'],
    install_requires=requirements,
    package_data={ 
        #'hybris': package_data,
    },
 
    project_urls={  # Optional
        'Bug Reports': 'https://github.com/kaeryv/hybris/issues',
        'Source': 'https://github.com/kaeryv/hybris/',
    },
)
