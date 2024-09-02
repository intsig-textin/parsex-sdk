from setuptools import setup, find_packages

setup(
    name='TextInParseX',
    version='1.0.0',
    author='bin_zhang',
    description='A text-in parser engine for pdf to markdown processing',
    long_description=open('../README.md').read(),
    long_description_content_type='text/markdown',
    url='https://github.com/intsig-textin/parsex-sdk',
    packages=find_packages(),
    install_requires=[
        'opencv-python-headless',
        'numpy',
        'requests'
    ],
    classifiers=[
        'Programming Language :: Python :: 3',
        'License :: OSI Approved :: MIT License',
        'Operating System :: OS Independent',
    ],
    python_requires='>=3.6',
)
