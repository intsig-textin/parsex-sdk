from setuptools import setup, find_packages

setup(
    name='ParseGenius',
    version='0.1.0',
    author='doughboii',
    description='A text-in parser engine for pdf to markdown processing',
    long_description=open('README.md').read(),
    long_description_content_type='text/markdown',
    url='https://github.com/intsig/x2md_tools/python/ParseGenius_engine',
    packages=find_packages(),
    install_requires=[
    ],
    classifiers=[
        'Programming Language :: Python :: 3',
        'License :: OSI Approved :: MIT License',
        'Operating System :: OS Independent',
    ],
    python_requires='>=3.6',
)
