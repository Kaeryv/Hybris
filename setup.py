import sys
from setuptools import setup, find_packages
from os import path

here = path.abspath(path.dirname(__file__))
platform = sys.platform

# Get the long description from the README file
with open(path.join(here, 'README.md'), encoding='utf-8') as f:
    long_description = f.read()

# install only the system's corresponding lib
if platform == 'win32':
    package_data = ['libhybris.so']
#elif platform == 'darwin':
#    package_data = ['libraylib.2.0.0.dylib']
elif platform.startswith('linux') or platform == 'linux':
    package_data = ['libhybris.so']
else:
    assert False, "ERROR"
    ## or install all if the above fails
    #package_data = ['libraylib_shared.dll',
    #                 'libraylib.so.2.0.0',
    #                 'libraylib.2.0.0.dylib']

py_ver = sys.version_info
requirements = []
if py_ver[1] < 5:
    requirements.append('typing')
if py_ver[1] < 4:
    requirements.append('enum')

# Arguments marked as "Required" below must be included for upload to PyPI.
# Fields marked as "Optional" may be commented out.

setup(
    name='hybris-py',  # Required
    # Versions should comply with PEP 440:
    # https://www.python.org/dev/peps/pep-0440/
    version='0.1.1',  # Required
    python_requires='>=3.3, <4',
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
        'hybris': package_data,
    },

    project_urls={  # Optional
        'Bug Reports': 'https://github.com/kaeryv/hybris/issues',
        'Source': 'https://github.com/kaeryv/hybris/',
    },
)
