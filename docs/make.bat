@ECHO OFF

pushd %~dp0

REM Command file for Sphinx documentation

if "%SPHINXBUILD%" == "" (
	set SPHINXBUILD=sphinx-build
)

if "%1" == "" goto help

%SPHINXBUILD% >NUL 2>NUL
if errorlevel 9009 (
	echo.
	echo.The 'sphinx-build' command was not found. Make sure you have Sphinx
	echo.installed, then set the SPHINXBUILD environment variable to point
	echo.to the full path of the 'sphinx-build' executable. Alternatively you
	echo.may add the Sphinx directory to PATH.
	echo.
	echo.If you don't have Sphinx installed, grab it from
	echo.http://sphinx-doc.org/
	exit /b 1
)

set SOURCEDIR=zh
set BUILDDIR=output/zh

if "%1" == "clean" (
    rmdir /S /Q .\output
    rmdir /S /Q .\zh\_static
	rmdir /S /Q .\zh\apps
	rmdir /S /Q .\zh\middleware
) else (
    %SPHINXBUILD% -M %1 %SOURCEDIR% %BUILDDIR% %SPHINXOPTS% %O%
    rmdir /S /Q .\zh\_static
    rmdir /S /Q .\zh\apps
    rmdir /S /Q .\zh\middleware
)

set SOURCEDIR=en
set BUILDDIR=output/en

if "%1" == "clean" (
    rmdir /S /Q .\en\_static
	rmdir /S /Q .\en\apps
	rmdir /S /Q .\en\middleware
) else (
    %SPHINXBUILD% -M %1 %SOURCEDIR% %BUILDDIR% %SPHINXOPTS% %O%
    rmdir /S /Q .\en\_static
    rmdir /S /Q .\en\apps
    rmdir /S /Q .\en\middleware
)

goto end

:help
%SPHINXBUILD% -M help %SOURCEDIR% %BUILDDIR% %SPHINXOPTS% %O%

:end
popd
