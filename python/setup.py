from setuptools import setup, find_packages

setup(
    name='text_in_parser_engine',
    version='0.1.0',
    author='doughboii',
    description='A text-in parser engine for document processing',
    long_description=open('README.md').read(),
    long_description_content_type='text/markdown',
    url='https://github.com/intsig/x2md_tools/python/text_in_parser_engine',
    packages=find_packages(),
    install_requires=[
        'json',
    ],
    classifiers=[
        'Programming Language :: Python :: 3',
        'License :: OSI Approved :: MIT License',
        'Operating System :: OS Independent',
    ],
    python_requires='>=3.6',
)
